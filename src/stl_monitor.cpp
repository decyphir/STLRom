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
		if (!data.empty() && s[0] <= data.back()[0])
		{
			throw std::invalid_argument("Sample time must be strictly greater than the last sample time.");
		}
		data.push_back(s);
	}

    double STLMonitor::eval_rob() {
        return eval_rob(start_time, end_time);
    }

    double STLMonitor::eval_rob(double t) {
        return eval_rob(t,t);
    }
	double STLMonitor::eval_rob(double t_start, double t_end)
        {
            if (formula)
            {
				// Ensure formula reads the right data
				formula->set_trace_data_ptr(data);
				formula->set_param_map_ptr(param_map);
				Signal::semantics=semantics;
				Signal::interpol= interpol;
				formula->reset();				
				formula->set_horizon(t_start, t_end);
				rob = formula->compute_robustness();
                lower_rob = formula->compute_lower_rob();
                upper_rob = formula->compute_upper_rob();
            }
            return rob;
        }
}