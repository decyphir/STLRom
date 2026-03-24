#include <transducer.h>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "tools.h"
#include <exception>
#include "signal_expr.h"
#ifdef IS_PC
#include <minmax.h>
#endif
//#define DEBUG__

namespace STLRom {

    enum CrossType { PLUS_EPS, ZERO, MINUS_EPS };
    // struct used to track epsilon crossings
    struct Crossing {
        double t;
        CrossType type;
        bool isAscending;
    };

    constant_transducer::constant_transducer(const string &p) {

        value = 0.;
        if (!s_to_d(p, value))  param = p;

    }
       
    double stl_atom::compute_robustness() {
#ifdef DEBUG__
        printf(">> stl_atom::compute_robustness:              IN.\n");
        cout<< "start_time:" << start_time << " end_time:" << end_time << endl;
#endif
    
        childL->compute_robustness();
        childR->compute_robustness();

        z.beginTime = fmax(childL->z.beginTime, childR->z.beginTime);
        double endTime = fmin(childL->z.endTime, childR->z.endTime);


        auto itL = childL->z.begin();
        auto itR = childR->z.begin();

        // Skip elements outside the overlap in the beginning
        while (itL != childL->z.end() && itL->time < z.beginTime) ++itL;
        while (itR != childR->z.end() && itR->time < z.beginTime) ++itR;

        // Iterate over both simultaneously
        bool first_pass = true;
        double t_prev, v_prev, d_prev, v_prev_neq, d_prev_neq;

        bool previous_was_equal = false;


        double tL = -1, tR = -1;

        while(tL < endTime || tR < endTime) {
            tL = (itL != childL->z.end()) ? itL->time : std::numeric_limits<double>::infinity();
            tR = (itR != childR->z.end()) ? itR->time : std::numeric_limits<double>::infinity();

            double dL = (*itL).derivative;
            double dR = (*itR).derivative;
            double vR, vL;

            // Stop when we overtake the overlap
            if(fmin(tL, tR) > endTime) break;

            double t, vt, dt, d_neq, v_neq;

            bool advance_L = false;
            bool advance_R = false;
            bool equals = false;
            bool first_eq_ineq = false; // first point in a subseries of equality points or inequality points (for comp::equal)

            if(tL < tR) {
                t = tL;
                advance_L = true;

                vL = (*itL).value;
                vR = (*itR).valueAt(t);
            } else if (tL > tR) {
                t = tR;
                advance_R = true;

                vL = (*itL).valueAt(t);
                vR = (*itR).value;
            } else { // equality (might cause issues)
                t = tL;
                advance_L = true;
                advance_R = true;

                vL = (*itL).value;
                vR = (*itR).value;
            }
            


            

            // fill z at time t
            switch (comp)
            {
            case comparator::LESSTHAN:
                vt = vR - vL;
                dt = dR - dL;

                v_neq = vt;
                d_neq = dt;

                if (fabs(vt) < Signal::Eps || (vt == Signal::Eps && dt < 0) || (vt == -Signal::Eps && dt > 0)) {
                    vt -= Signal::Eps;
                }
                // if (vt < Signal::Eps || (vt == Signal::Eps && dt < 0)) {
                //     vt -= Signal::Eps;
                // }
                break;
            case comparator::GREATERTHAN:
                vt = vL - vR;
                dt = dL - dR;

                v_neq = vt;
                d_neq = dt;

                if (fabs(vt) < Signal::Eps || (vt == Signal::Eps && dt < 0) || (vt == -Signal::Eps && dt > 0)) {
                    vt -= Signal::Eps;
                }

                // if (vt < Signal::Eps || (vt == Signal::Eps && dt < 0)) {
                //     vt -= Signal::Eps;
                // }
                break;
            case comparator::EQUAL:
                v_neq = vL - vR;
                d_neq = dL - dR;
                vt = -fabs(vL-vR);
                // dt = (vL > vR) ? dR - dL : dL - dR;
                if (vL > vR || (vL == vR && dL > dR)) {
                    dt = dR - dL;
                } else {
                    dt = dL - dR;
                }
                equals = false;
                if ((v_neq < Signal::Eps && v_neq > -Signal::Eps) || (v_neq == Signal::Eps && d_neq < 0) || (v_neq == -Signal::Eps && d_neq > 0)) {
                    vt += Signal::Eps;   
                    equals = true;
                }
                if(first_pass || equals != previous_was_equal) first_eq_ineq = true; // first point in a consecutive subseries at which the or inequality holds (change of state)                
                previous_was_equal = equals;
                break;
            }

            if (!first_pass) {

                // array to track +/- epsilon crossings
                Crossing events[3];
                int crossing_count = 0;
          
                // check for +epsilon crossing
                bool plus_descending_cross = v_prev_neq > Signal::Eps && d_prev_neq < 0 && v_neq < Signal::Eps;
                bool plus_ascending_cross = v_prev_neq < Signal::Eps && d_prev_neq > 0 && v_neq > Signal::Eps;
                bool plus_epsilon_cross = plus_ascending_cross || plus_descending_cross;
                double t_plus_epsilon_cross;
                if (plus_epsilon_cross) {
                    t_plus_epsilon_cross = t_prev + (Signal::Eps-v_prev_neq) / d_prev_neq; // t at which v is +eps
                    events[crossing_count++] = {t_plus_epsilon_cross, CrossType::PLUS_EPS, plus_ascending_cross};
                }

                // check for -epsilon crossing
                bool minus_descending_cross = v_prev_neq > -Signal::Eps && d_prev_neq < 0 && v_neq < -Signal::Eps;
                bool minus_ascending_cross = v_prev_neq < -Signal::Eps && d_prev_neq > 0 && v_neq > -Signal::Eps;
                bool minus_epsilon_cross = minus_ascending_cross || minus_descending_cross;
                double t_minus_epsilon_cross;
                if (minus_epsilon_cross) {
                    t_minus_epsilon_cross = t_prev + (-Signal::Eps-v_prev_neq) / d_prev_neq; // t at which v is -eps
                    events[crossing_count++] = {t_minus_epsilon_cross, CrossType::MINUS_EPS, minus_ascending_cross};
                }

                // check for zero crossing
                bool zero_descending_cross = v_prev_neq > 0 && d_prev_neq < 0 && v_neq < 0;
                bool zero_ascending_cross = v_prev_neq < 0 && d_prev_neq > 0 && v_neq > 0;
                bool zero_epsilon_cross = zero_ascending_cross || zero_descending_cross;
                double t_zero_epsilon_cross;
                if (zero_epsilon_cross) {
                    t_zero_epsilon_cross = t_prev + (-v_prev_neq) / d_prev_neq; // t at which v is 0
                    events[crossing_count++] = {t_zero_epsilon_cross, CrossType::ZERO, zero_ascending_cross};
                }

                // sort events by time
                for (int i = 0; i < crossing_count - 1; ++i) {
                    for (int j = i + 1; j < crossing_count; ++j) {
                        if (events[i].t > events[j].t) {
                            std::swap(events[i], events[j]);
                        }
                    }
                }

                for (int i = 0; i < crossing_count; i++) {
                    auto e = events[i];

                    if (e.type == PLUS_EPS) {
                        if (comp == comparator::EQUAL) {
                            if (e.isAscending) {
                                z.appendSample(e.t, -Signal::Eps, -fabs(d_prev));
                            } else {
                                z.appendSample(e.t, ZERO_POS, fabs(d_prev));
                            }
                        } else { // comp is LESSTHAN or GREATERTHAN
                            if (e.isAscending) {
                                z.appendSample(e.t, Signal::Eps, d_prev);
                            } else {
                                z.appendSample(e.t, ZERO_NEG, d_prev);
                            }
                        }
                    } else if (e.type == MINUS_EPS) {
                        if (comp == comparator::EQUAL) { 
                            if (e.isAscending) {
                                z.appendSample(e.t, ZERO_POS, fabs(d_prev));
                            } else {
                                z.appendSample(e.t, -Signal::Eps, -fabs(d_prev));
                            }
                        } else { // comp is LESSTHAN or GREATERTHAN
                            if (e.isAscending) {
                                z.appendSample(e.t, -2*Signal::Eps, d_prev);
                            } else {
                                z.appendSample(e.t, -Signal::Eps, d_prev);
                            }
                        }
                    } else { // zero crossing
                        if (comp == comparator::EQUAL) {
                            cout << "appending zero equal ";
                            z.appendSample(e.t, Signal::Eps, -fabs(d_prev));
                        }
                    }
                }
    
            }

            z.appendSample(t, vt, dt);

            


            t_prev = t;
            v_prev = vt;
            d_prev = dt;
            d_prev_neq = d_neq;
            v_prev_neq = v_neq;

            if (advance_L) itL++;
            if (advance_R) itR++;

            first_pass = false;
        }

        //z.endTime = endTime;        
        z.endTime = end_time; // I know, can be confusing. 
        
        Signal z_space;
        switch (Signal::semantics) {
			case Semantics::SPACE:
                z.simplify();
	        	return z.front().value;    
			case Semantics::LEFT_TIME:
                z_space = z;
                z.compute_left_time_rob(z_space);
                return z.front().value;
            case Semantics::RIGHT_TIME:
                z_space = z;
                z.compute_right_time_rob(z_space);
                return z.front().value;
            case Semantics::BOOLEAN:
                z_space = z;
                z.compute_boolean(z_space);
                return z.front().value;
			default:
				throw std::invalid_argument("Invalid semantics value");
		}    
        
#ifdef DEBUG__
        cout << "OUT z:" << z << endl;
        printf("<< stl_atom::compute_robustness:              OUT.\n");
#endif
    }
    
    double signal_transducer::compute_robustness() {
#ifdef DEBUG__
        printf(">> signal_transducer::compute_robustness:     IN.\n");
        cout<< "start_time:" << start_time << " end_time:" << end_time << endl;
#endif
        short i = signal_map[variable];
        double t=0., v=0.;  
        double res= 0.;
            
        if (i) {
            while ( (td_idx <= trace_data_ptr->size()-1)  && t<=end_time)   {
                t = (trace_data_ptr->at(td_idx))[0];
                v = (trace_data_ptr->at(td_idx))[i];
                // cout << "t: " << t << " v: " << v << endl;
                z.appendSample(t, v);
                td_idx++;
			}
		}

        //if (t>end_time)  
        z.endTime = end_time;
        
        if (z.endTime>start_time)
            z.resize(start_time, max(start_time,z.endTime),v);  // if we reached start_time trim everything before
        else
            z.resize(z.endTime, z.endTime, v);   // otherwise, only keep the last value seen before start_time

        res= z.front().value;    
#ifdef DEBUG__
        printf("<< signal_transducer::compute_robustness      OUT.\n");
#endif
        return res;
    }

    double constant_transducer::compute_robustness() {

#ifdef DEBUG__
        printf(">> constant_transducer::compute_robustness:IN.\n");
        cout<< "start_time:" << start_time << " end_time:" << end_time << endl;
#endif
        
        // TODO exception if parameter not found for some reason ? For now, we get 0. as a default silent value.
        if (!param.empty())
        {
            if (!get_param(param,value)) // should not be necessary
                cout << "Parameter " << param << " not found (?)." << endl;                
        }

        double t=0.;  
            
        while ((td_idx <= trace_data_ptr->size()-1) && t<=end_time)   {
            t = (trace_data_ptr->at(td_idx))[0];
            z.appendSample(t, value);
            td_idx++;
        }
		
        if (t>end_time)  
            z.endTime = end_time;
        
        if (z.endTime>start_time) {
            z.resize(start_time, max(start_time,z.endTime),value);  // if we reached start_time trim everything before
        }
        else {
            z.resize(z.endTime, z.endTime, value);   // otherwise, only keep the last value seen before start_time
        }
#ifdef DEBUG__
        printf("<< constant_transducer::compute_robustness OUT.\n");
#endif
        return value;
    }
}

