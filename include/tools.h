/*
 * tools.h
 *
 *  Created on: Mar 31, 2014
 *      Author: alex
 */

#ifndef TOOLS_H_
#define TOOLS_H_
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include "signal.h"



using namespace std;
using namespace STLRom;
typedef vector<Signal> trace_data; // small TODO: this is defined in transducer.h too, should not be probably

/**  string to double conversion */
inline bool s_to_d(std::string const& s,
                             double &x )
{
  std::istringstream i(s);
  x =0;
  char c;
  if (!(i >> x) || ( i.get(c)))
    return false;
  else
	return true;
}
string signal_map_to_string(map<string, int>); // TODO move in tools ?
	
/** double to string conversion */
inline string d_to_s(
	double const &x)
{
	std::ostringstream o;
	o << x;
	return o.str();
}

/** Returns a m x n random trace */ 
// TODO reimplement that 
// trace_data rand_trace_data( int m, int n);

const std::string current_date_time();

void print(const trace_data&); // TODO that too, maybe
bool read_trace(const string &trace_file_name, trace_data &data); // Yeah, we'll need a csv importer I guess
bool write_trace(const std::string& filename, const trace_data& signals);


template <typename BinaryOp, typename QuartOp>
void merge_signals_with_op(Signal &out, const Signal &zL, const Signal &zR, BinaryOp op_v, QuartOp op_d)
{
    auto itL = zL.begin();
    auto itR = zR.begin();

    double beginTime = std::max(zL.beginTime, zR.beginTime);
    double endTime   = std::min(zL.endTime,   zR.endTime);

    // Skip elements outside the overlap in the beginning
    while (itL != zL.end() && itL->time < beginTime) ++itL;
    while (itR != zR.end() && itR->time < beginTime) ++itR;

    auto last_itL = itL;
    auto last_itR = itR;

    while (true) {

        double tL = (itL != zL.end()) ? itL->time : std::numeric_limits<double>::infinity();
        double tR = (itR != zR.end()) ? itR->time : std::numeric_limits<double>::infinity();

        double t = std::min(tL, tR);
        if (t > endTime) break;

        double vL, vR;
        double dL, dR;

        if (tL < tR) {
            vL = itL->value;
            vR = last_itR->valueAt(t);

            dL = itL->derivative;
            dR = last_itR->derivative;

            last_itL = itL;
            ++itL;

        } else if (tR < tL) {
            vL = last_itL->valueAt(t);
            vR = itR->value;

            dL = last_itL->derivative;
            dR = itR->derivative;

            last_itR = itR;
            ++itR;

        } else {
            vL = itL->value;
            vR = itR->value;

            dL = itL->derivative;
            dR = itR->derivative;

            last_itL = itL; last_itR = itR;
            ++itL; ++itR;
        }

        out.appendSample(t, op_v(vL, vR), op_d(vL, vR, dL, dR));
    }
}



#endif /* TOOLS_H_ */
