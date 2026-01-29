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
        // Assumes childL and childR have signals with same number of samples
        // TODO should be eventually improved with proper operations on signals
    
        childL->compute_robustness();
        childR->compute_robustness();
        auto itL = childL->z.begin();
        auto itR = childR->z.begin();
        for (; itL != childL->z.end() &&  itR != childR->z.end() ;itL++, itR++) {
            double t = (*itL).time;
            double vL = (*itL).value;
            double vR = (*itR).value;
            double dL = (*itL).derivative;
            double dR = (*itR).derivative;
		
            if (comp == comparator::LESSTHAN )
                z.appendSample(t, vR-vL, dR-dL);
            else
                z.appendSample(t, vL-vR, dL-dR);	
        }
        z.endTime = childL->z.endTime;

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

