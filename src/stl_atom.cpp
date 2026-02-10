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

namespace STLRom {

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
        double t_prev, v_prev, d_prev;

        while(itL != childL->z.end() || itR != childR->z.end()) {
            double tL = (*itL).time;
            double tR = (*itR).time;
            double dL = (*itL).derivative;
            double dR = (*itR).derivative;
            double vR, vL;

            // Stop when we overtake the overlap
            if(fmin(tL, tR) > endTime) break;

            double t, vt, dt, dgeq;

            bool advance_L = false;
            bool advance_R = false;
            bool equals = false;

            if(tL < tR) {
                t = tL;
                advance_L = true;

                vL = (*itL).value;
                vR = (*itR).valueAt(t);
            } else if (tL > tR) {
                t = tR;
                advance_R = true;

                vL = (*itL).value;
                vR = (*itR).valueAt(t);
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
                break;
            case comparator::GREATERTHAN:
                vt = vL - vR;
                dt = dL - dR;
                break;
            case comparator::EQUAL:
                if (fabs(vL-vR) < Signal::Eps) {
                        vt = Signal::BigM;
                        dt = 0.;
                        dgeq = (vL > vR) ? dR - dL : dL - dR;
                        equals = true;
                } else {
                    vt = -fabs(vL-vR);
                    dt = (vL > vR) ? dR - dL : dL - dR;
                }
                break;
            }

            if (!first_pass) {
                // Note: 0 derivative is ok because it does not pass this check
                if ((v_prev < 0 && d_prev > 0) || (v_prev > 0 && d_prev < 0)) {
                    double t_zero_cross = t_prev-v_prev/d_prev;
                    if (t_zero_cross < t) {
                        // cout << t_prev << " " << t_zero_cross << " " << t << endl;
                        if (comp == comparator::EQUAL) {
                            double t_minus = t_prev + (-Signal::Eps-v_prev) / d_prev; // t at which v is -eps
                            z.appendSample(t_minus, -Signal::Eps, -d_prev*v_prev/fabs(v_prev));

                            z.appendSample(t_zero_cross, Signal::BigM, 0.);

                            double t_plus = t_prev + (Signal::Eps-v_prev) / d_prev; // t at which v is +eps
                            if (t_plus < t) z.appendSample(t_plus, -Signal::Eps, d_prev*v_prev/fabs(v_prev));

                        }
                        else if (v_prev < 0) z.appendSample(t_zero_cross, 0., d_prev);
                        else z.appendSample(t_zero_cross, 0., d_prev);
                    }
                }
            }


            z.appendSample(t, vt, dt);

            if (equals && dgeq != 0) {
                double t_plus = t + (Signal::Eps) / fabs(dgeq); // t at which v is +eps or -eps
                z.appendSample(t_plus, -Signal::Eps, dgeq);

            }

            t_prev = t;
            v_prev = vt;
            d_prev = dt;

            if (advance_L) itL++;
            if (advance_R) itR++;

            first_pass = false;
        }

        z.endTime = endTime;

        Signal z_space;
        switch (Signal::semantics) {
			case Semantics::SPACE:
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

        if (t>end_time)  
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

