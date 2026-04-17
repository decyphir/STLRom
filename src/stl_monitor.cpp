#include <fstream>
#include <sstream>
#include "tools.h"
#include "signal.h"
#include "stl_monitor.h"

namespace STLRom
{
    	string STLMonitor::get_signal_names() const
	{
		return signal_map_to_string(signal_map);
	}

	void STLMonitor::add_sample(vector<double> s)
	{
		if (s.size() != signal_map.size() + 1) 
	    {
        	throw std::invalid_argument("Sample size does not match the number of signals.");
    	}
    
    // We don't need to check, that will be done at the Signal level
    //if (!data.empty() && s[0] <= data.back()[0])
    //{
    //    throw std::invalid_argument("Sample time must be strictly greater than the last sample time.");
    //}
		
    	double t = s[0];
    	for(int i=1; i<s.size();i++)
	        data[i-1].appendSample(s[0],s[i]);
		
		up_to_date = false;
	}

	void STLMonitor::set_signals(const std::vector<Signal>& signals)
{
    if (signals.size() != signal_map.size()) {
        throw std::invalid_argument("Number of signals does not match the number of declared signals.");
    }

    data = signals; // copy
}

	bool STLMonitor::load_csv(const std::string& file)
{
    return read_trace(file, data);
}

	bool STLMonitor::write_csv(const std::string& file) const
{	
	return write_trace(file, data);
}


	Signal STLMonitor::eval_rob() {
        return eval_rob(start_time, end_time);
    }

    Signal STLMonitor::eval_rob(double t) {        
		return eval_rob(t,t);
    }
	Signal STLMonitor::eval_rob(double t_start, double t_end)
    {
		start_time = t_start;
		end_time  = t_end;
		if (formula)
        {
			// Ensure formula reads the right data
			formula->set_trace_data_ptr(data);
			formula->set_param_map_ptr(param_map);
			Signal::semantics=semantics;
			Signal::interpol = interpol;
			formula->reset();				
			formula->set_horizon(t_start, t_end);
			rob = formula->compute_robustness();
    		up_to_date = true;
		}			
        return formula->z;
    }	



    vector<Signal> STLMonitor::eval_online_rob() {
        return eval_online_rob(start_time, end_time);
    }

    vector<Signal> STLMonitor::eval_online_rob(double t) {        
		return eval_online_rob(t,t);
    }
	vector<Signal> STLMonitor::eval_online_rob(double t_start, double t_end)
    {
		start_time = t_start;
		end_time  = t_end;
		if (formula)
        {
			// Ensure formula reads the right data
			formula->set_trace_data_ptr(data);
			formula->set_param_map_ptr(param_map);
			Signal::semantics=semantics;
			Signal::interpol = interpol;
			formula->reset();				
			formula->set_horizon(t_start, t_end);
			rob = formula->compute_robustness();
            lower_rob = formula->compute_lower_rob();
            upper_rob = formula->compute_upper_rob();
			up_to_date = true;
		}			
        return {formula->z, formula->z_low, formula->z_up};
    }
	


	string STLMonitor::get_signals_names() const
	{
    	return signal_map_to_string(signal_map);
	}
	
}