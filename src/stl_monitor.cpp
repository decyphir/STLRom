#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include "tools.h"
#include "signal.h"
#include "stl_monitor.h"

namespace STLRom
{

	void STLMonitor::add_sample(vector<double> s)
	{
		data->add_sample(s);
		up_to_date = false;
	}

	void STLMonitor::add_sample(vector<double> s, bool interp)
	{
		data->add_sample(s, interp);
		up_to_date = false;
	}

	void STLMonitor::set_signals(const std::vector<Signal>& signals)
	{
		data->set_data_vector(signals); // copy TODO: check if it's actually a copy
		up_to_date = false;
	}

	// TODO: better convert to STLData
	void STLMonitor::load_csv(const vector<string>& files)
	{
		if (files.size() != data->signal_map.size()) {
			throw std::invalid_argument("Number of files does not match the number of declared signals.");
		}

		for (int i = 0; i < files.size(); i++) {
			data->data_vector[i].read_from_file(files[i]);
		}
	}

	// TODO: better convert to STLData
	void STLMonitor::write_csv(const std::string& directory) const
	{
		string dir = directory;
		
		if (dir.back() != '/') dir += '/';

		for (const auto &signal : data->signal_map)
		{
		    string filename = dir + signal.first + ".csv";

			data->data_vector[signal.second].write_to_file(filename);
		}
	}

	double STLMonitor::get_rob(double t0 = 0.)
	{
			
		if (data->is_empty())
		{
			cout << "Empty data" << endl;
			return 0.;
		}

		start_time = t0;
		end_time = t0;
		if (formula)
		{
			formula->set_trace_data_ptr(data->data_vector);
			formula->set_param_map_ptr(param_map);
			
			formula->init_valid_interval();
			double s = formula->valid_start, e = formula->valid_end;
			if (s > e + ZERO_POS)
			{
				throw std::invalid_argument(formula->get_formula_string() +
					" is not backed by the data at any time (the data is too short for its time windows)");
			}
			if (t0 < s - ZERO_POS || t0 > e + ZERO_POS)
			{
				std::ostringstream msg;
				msg << formula->get_formula_string() << " is only defined on [" << s << ", " << e
				    << "]; t = " << t0 << " is outside";
				throw std::invalid_argument(msg.str());
			}
			Signal::semantics = semantics;
			formula->reset();
			formula->set_horizon(t0, t0);
			up_to_date = true;
		}
		return formula->compute_robustness();
	}

	double STLMonitor::get_rob()
	{
		return get_rob(0.);
	}

	vector<double> STLMonitor::get_online_rob(double t0 = 0.)
	{
		vector<double> out_rob;
		if (data->is_empty())
		{
			cout << "Empty data" << endl;
			return out_rob;
		}

		if(formula)
		{
			formula->set_trace_data_ptr(data->data_vector);
			formula->set_param_map_ptr(param_map);
			formula->init_valid_interval(); // stores each node's valid times, for the bounds
			Signal::semantics = semantics;
			formula->reset();
			formula->set_horizon(t0, t0);
			double rob = formula->compute_robustness();
			double lower_rob = formula->compute_lower_rob();
			double upper_rob = formula->compute_upper_rob();
			out_rob = {rob, lower_rob, upper_rob};

			return out_rob;
		} else {
			cerr << "Formula not defined." << endl;
			return {};
		}
	}

	vector<double> STLMonitor::get_online_rob()
	{
		return get_online_rob(0.);
	}


	Signal STLMonitor::get_rob_signal() {
        return get_rob_signal(start_time, end_time);
    }

    Signal STLMonitor::get_rob_signal(double t) {        
		return get_rob_signal(t,t);
    }
	Signal STLMonitor::get_rob_signal(double t_start, double t_end)
    {
		if (data->is_empty())
		{
			cout << "Empty data" << endl;
			return Signal();
		}
		start_time = t_start;
		end_time  = t_end;
		if (formula)
        {
			// Ensure formula reads the right data
			formula->set_trace_data_ptr(data->data_vector); // TODO: convert to STLData
			formula->set_param_map_ptr(param_map);
			Signal::semantics=semantics;
			formula->reset();
			// Only compute where the data backs the whole formula
			formula->init_valid_interval();
			double s = formula->valid_start, e = formula->valid_end;
			double from = fmax(s, t_start), to = fmin(e, t_end);
			if (from > to + ZERO_POS)
			{
				// nothing is backed by the data, rob is undefined
				rob = std::numeric_limits<double>::quiet_NaN();
				up_to_date = true;
				return Signal();
			}
			to = fmax(to, from); // the request missed the valid times only by rounding: use the valid edge
			formula->set_horizon(from, to);
			rob = formula->compute_robustness();
    		up_to_date = true;
			formula->z.resize(from, to);
		}

		return formula->z;
    }	



    vector<Signal> STLMonitor::get_online_rob_signal() {
        return get_online_rob_signal(start_time, end_time);
    }

    vector<Signal> STLMonitor::get_online_rob_signal(double t) {        
		return get_online_rob_signal(t,t);
    }
	vector<Signal> STLMonitor::get_online_rob_signal(double t_start, double t_end)
    {
		if (data->is_empty())
		{
			cout << "Empty data" << endl;
			vector<Signal> out_rob;
			return out_rob;
		}
		start_time = t_start;
		end_time  = t_end;
		if (formula)
        {
			// Ensure formula reads the right data
			formula->set_trace_data_ptr(data->data_vector); // TODO : convert to STLData
			formula->set_param_map_ptr(param_map);
			formula->init_valid_interval(); // stores each node's valid times, for the bounds
			double s = formula->valid_start, e = formula->valid_end;
			Signal::semantics=semantics;
			formula->reset();				
			formula->set_horizon(t_start, t_end);
			rob = formula->compute_robustness();
            lower_rob = formula->compute_lower_rob();
            upper_rob = formula->compute_upper_rob();
			up_to_date = true;
			double from = fmax(s, t_start), to = fmin(e, t_end);
			if (from > to + ZERO_POS) formula->z = Signal();
			else formula->z.resize(from, fmax(to, from));
		}


        return {formula->z, formula->z_low, formula->z_up};
    }
	

	robustness_map_t STLMonitor::get_robustness_map() {
		robustness_map_t rob_map;
		if (formula)
		{
			formula->fill_robustness_map(rob_map, 0);
		}
		else
		{
			cout << "No formula defined, returning empty robustness map." << endl; // TODO: does this happen?
		}
		return rob_map;
	}

	robustness_map_t STLMonitor::get_online_robustness_map() {
		robustness_map_t rob_map;
		if (formula)
		{
			formula->fill_online_robustness_map(rob_map, 0);
		}
		else
		{
			cout << "No formula defined, returning empty robustness maps." << endl; // TODO: does this happen?

		}
		return rob_map;
	}


	string STLMonitor::get_signals_names() const
	{
		return signal_map_to_string(data->signal_map);
	}
	
}