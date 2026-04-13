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
//bool read_trace(const string &trace_file_name, trace_data &data); // Yeah, we'll need a csv importer I guess

template <typename BinaryOp>
void merge_signals_with_op(Signal &out, const Signal &zL, const Signal &zR, BinaryOp op)
{
    auto itL = zL.begin();
    auto itR = zR.begin();

    double beginTime = std::max(zL.beginTime, zR.beginTime);
    double endTime   = std::min(zL.endTime,   zR.endTime);

    // Skip elements outside the overlap in the beginning
    while (itL != zL.end() && itL->time < beginTime) ++itL;
    while (itR != zR.end() && itR->time < beginTime) ++itR;

    while (true) {

        double tL = (itL != zL.end()) ? itL->time : std::numeric_limits<double>::infinity();
        double tR = (itR != zR.end()) ? itR->time : std::numeric_limits<double>::infinity();

        double t = std::min(tL, tR);
        if (t > endTime) break;

        double vL, vR;

        if (tL < tR) {
            vL = itL->value;
            vR = (*itR).valueAt(t);
            ++itL;

        } else if (tR < tL) {
            vL = (*itL).valueAt(t);
            vR = itR->value;
            ++itR;

        } else {
            vL = itL->value;
            vR = itR->value;
            ++itL;
            ++itR;
        }

        out.appendSample(t, op(vL, vR));
    }
}



#endif /* TOOLS_H_ */
