// $Id: stl_driver.cpp 378 2014-04-27 17:47:40Z donze $
/** \file driver.cc Implementation of the example::Driver class. */

#include "stdafx.h"
#include <fstream>
#include <sstream>

#include "stl_driver.h"
#include "transducer.h"
#include "stl_scanner.h"

namespace STLRom
{
	string signal_map_to_string(map<string, int> signal_map)
	{
	// get number of signals
		int n_signals = signal_map.size();
		// cout << "n_signals:" << n_signals << endl;
		string signames[n_signals];

		//     for (auto ii = signal_map->begin(); ii != signal_map->end(); ii++){
		for (const auto &ii : signal_map)
		{
			string sig = ii.first;
			int idx = ii.second - 1;
			signames[idx] = sig;
		}

		string str_out = signames[0];
		for (int idx = 1; idx < n_signals; idx++)
		{
			str_out += " " + signames[idx];
		}

		return str_out;
	} 

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

	double STLMonitor::update_rob()
        {
            if (formula)
            {
				// Ensure formula reads the right data
				formula->set_trace_data_ptr(data);
				formula->set_param_map_ptr(param_map);
				Signal::semantics=semantics;
				Signal::interpol= interpol;
				//formula->reset();				
				rob = formula->compute_robustness();
                lower_rob = formula->compute_lower_rob();
                upper_rob = formula->compute_upper_rob();
                current_time = data.back()[0];             
            }
            return rob;
        };


	map<string, token_type> STLDriver::reserved = map<string, token_type>(); // filled in stl_scanner.lpp

	STLDriver::STLDriver()
		: trace_scanning(false),
		  trace_parsing(false),
		  report(""),
		  lexer(nullptr),
		  nb_test_total(0),
		  nb_test_pos(0),
		  error_flag(false)
	{
	}

	STLDriver::STLDriver(trace_data _trace)
		: data(std::move(_trace)),
		  trace_scanning(false),
		  trace_parsing(false),
		  report(""),
		  lexer(nullptr),
		  nb_test_total(0),
		  nb_test_pos(0),
		  error_flag(false)
	{
	}

	STLDriver::~STLDriver()
	{
		for (auto &pair : formula_map)
		{
			delete pair.second;
		}
	}

	STLDriver::STLDriver(const STLDriver &other)
		: trace_scanning(other.trace_scanning),
		  trace_parsing(other.trace_parsing),
		  streamname(other.streamname),
		  param_map(other.param_map),
		  signal_map(other.signal_map),
		  data(other.data),
		  stl_test_map(other.stl_test_map),
		  trace_test_queue(other.trace_test_queue),
		  report(other.report),
		  test_log(other.test_log),
		  nb_test_pos(other.nb_test_pos),
		  nb_test_total(other.nb_test_total),
		  error_flag(other.error_flag)
	{

		for (const auto &pair : other.formula_map)
		{
			formula_map[pair.first] = pair.second->clone();
		}
	}

	STLDriver &STLDriver::operator=(const STLDriver &other)
	{
		if (this != &other)
		{
			trace_scanning = other.trace_scanning;
			trace_parsing = other.trace_parsing;
			streamname = other.streamname;
			param_map = other.param_map;
			signal_map = other.signal_map;
			data = other.data;
			stl_test_map = other.stl_test_map;
			trace_test_queue = other.trace_test_queue;
			report = other.report;
			test_log = other.test_log;
			nb_test_pos = other.nb_test_pos;
			nb_test_total = other.nb_test_total;
			error_flag = other.error_flag;

			for (auto &pair : formula_map)
			{
				delete pair.second;
			}
			formula_map.clear();

			for (const auto &pair : other.formula_map)
			{
				formula_map[pair.first] = pair.second->clone();
			}
		}
		return *this;
	}

	STLDriver::STLDriver(STLDriver &&other) noexcept
		: trace_scanning(other.trace_scanning), trace_parsing(other.trace_parsing), streamname(std::move(other.streamname)), param_map(std::move(other.param_map)),
		  signal_map(std::move(other.signal_map)), formula_map(std::move(other.formula_map)), data(std::move(other.data)), stl_test_map(std::move(other.stl_test_map)),
		  trace_test_queue(std::move(other.trace_test_queue)), report(std::move(other.report)), test_log(std::move(other.test_log)), nb_test_pos(other.nb_test_pos),
		  nb_test_total(other.nb_test_total), error_flag(other.error_flag)
	{
		other.formula_map.clear();
	}

	STLDriver &STLDriver::operator=(STLDriver &&other) noexcept
	{
		if (this != &other)
		{
			trace_scanning = other.trace_scanning;
			trace_parsing = other.trace_parsing;
			streamname = std::move(other.streamname);
			param_map = std::move(other.param_map);
			signal_map = std::move(other.signal_map);
			data = std::move(other.data);
			stl_test_map = std::move(other.stl_test_map);
			trace_test_queue = std::move(other.trace_test_queue);
			report = std::move(other.report);
			test_log = std::move(other.test_log);
			nb_test_pos = other.nb_test_pos;
			nb_test_total = other.nb_test_total;
			error_flag = other.error_flag;

			for (auto &pair : formula_map)
			{
				delete pair.second;
			}
			formula_map = std::move(other.formula_map);
			other.formula_map.clear();
		}
		return *this;
	}

	bool STLDriver::parse_stream(std::istream &in)
	{
		Scanner scanner(&in);
		scanner.set_debug(trace_scanning);
		this->lexer = &scanner;

		Parser parser(*this);
		parser.set_debug_level(trace_parsing);

		return (parser.parse() == 0);
	}

	bool STLDriver::parse_file(const std::string &filename)
	{
		std::ifstream in(filename.c_str());
		if (!in.good())
			return false;
		return parse_stream(in);
	}

	bool STLDriver::parse_string(const std::string &input)
	{
		std::istringstream iss(input);
		return parse_stream(iss);
	}

	/** Error handling with associated line number. This can be modified to
	 * output the error e.g. to a dialog box. */
	void STLDriver::error(const class location &l,
						  const std::string &m)
	{
		std::cerr << l << ": " << m << std::endl;
	}

	/** General error handling. This can be modified to output the error
	 * e.g. to a dialog box. */
	void STLDriver::error(const std::string &m)
	{
		std::cerr << m << std::endl;
	}

	string STLDriver::get_signals_names() const
	{
		return signal_map_to_string(signal_map);
	}

	/** clear assigned formulas and trace_test_queue */
	void STLDriver::clear()
	{

		for (auto formula = formula_map.begin(); formula != formula_map.end(); formula++)
		{
			if (formula->second != 0)
			{
				delete formula->second;
				formula->second = 0;
			}
		}

		for (auto stest = stl_test_map.begin(); stest != stl_test_map.end(); stest++)
		{
			if (stest->second.formula != 0)
			{
				delete stest->second.formula;
				stest->second.formula = 0;
			}
		}

		formula_map.clear();
		param_map.clear();
		stl_test_map.clear();

		while (!trace_test_queue.empty())
		{
			transducer *formula = get_next_formula();
			while (formula != nullptr)
			{
				delete formula;
				trace_test_queue.front().tests.pop_front();
				formula = get_next_formula();
			}
			trace_test_queue.pop_front();
		}
		report.clear();
		test_log.clear();
		error_flag = false;

		nb_test_total = 0;
		nb_test_pos = 0;
	}

	void STLDriver::print(ostream &out) const
	{

		out << "\nAssigned formulas:" << endl;
		out << "-------------------" << endl;

		for (auto formula = formula_map.begin(); formula != formula_map.end(); formula++)
		{
			out << formula->first << ":" << endl;
			out << *(formula->second) << endl;
		}

		out << "\nDefault Parameters:" << endl;
		out << "---------------------" << endl;

		for (auto param = param_map.begin(); param != param_map.end(); param++)
		{
			out << param->first << "=";
			out << param->second << endl;
		}

		// Not using tests at the moments, will see if we keep/update them...
		// out << "\nTrace tests:" << endl;
		// out << "---------------" << endl;

		// string indent = "    ";
		// for (auto it = trace_test_queue.begin(); it != trace_test_queue.end(); it++)
		// {

		// 	out << (*it).id << ": ";
		// 	out << (*it).env << ", ";
		// 	out << "SimTime: " << (*it).sim_time << " Visu:" << (*it).visu << endl;
		// 	for (auto its = (*it).tests.begin(); its != (*it).tests.end(); its++)
		// 	{
		// 		// transducer->param_map =  param_map; FIXME probably needs a set_param for transducers
		// 		out << indent << its->test_id << endl;
		// 		for (auto elem = its->param_map.begin(); elem != its->param_map.end(); elem++)
		// 			out << indent << indent << elem->first << "=" << elem->second << endl;

		// 		out << indent << indent << *(*its).formula << endl;
		// 	}
		// 	out << endl;
		// }
		out << endl;
	}

	/** compute robustness for all formulas defined in the driver and write results in files */
	void STLDriver::dump()
	{

		// transducer::param_map = param_map; FIXME

		for (auto formula = formula_map.begin(); formula != formula_map.end(); formula++)
		{
			formula->second->trace_data_ptr = &data;
			formula->second->init_horizon();
			formula->second->compute_robustness();
			(formula->second->z).dumpFile(formula->first + ".out");
		}
	}

	/** run all stl tests for the next trace_test and write results in report */
	bool STLDriver::run_tests()
	{

		error_flag = false;
		string indent("    ");
		if (!trace_test_queue.empty())
		{

			const stl_test *curr_test = get_next_stl_test();
			transducer *formula = get_next_formula();

			report.append("stl_test " + trace_test_queue.front().id + "\n");

			// Initialize global parameters
			// transducer->param_map =  param_map; FIXME

			while (formula != nullptr)
			{

				// initialize local parameters FIXME
				// for (auto elem = curr_test->param_map.begin(); elem != curr_test->param_map.end(); elem++){
				//	transducer->param_map[elem->first] = elem->second;
				//}

				formula->trace_data_ptr = &data;
				formula->init_horizon();
				double rob = formula->compute_robustness();

				test_log.append(curr_test->test_id + ": " + d_to_s(rob) + "\n");
				report.append(indent + curr_test->test_id + ": " + d_to_s(rob));

				if (rob > 0.)
				{
					nb_test_pos++;
					report += "  --> PB\n";
					if (!curr_test->report_positive.empty())
						report += indent + curr_test->report_positive + "\n";
					if (curr_test->is_error)
						error_flag = true;
				}
				else
				{
					report += "  --> OK\n";
					if (!curr_test->report_negative.empty())
						report += indent + curr_test->report_negative + "\n";
				}

				// delete formula;
				trace_test_queue.front().tests.pop_front();
				formula = get_next_formula();
				curr_test = get_next_stl_test();
			}
		}
		report += "\n";
		trace_test_queue.pop_front();
		return error_flag;
	}

	void STLDriver::add_trace_test(const string &test_id, const string &trace_cfg, double sim_time, bool visu)
	{
		trace_test T;
		T.id = test_id;
		T.env = trace_cfg;
		T.sim_time = sim_time;
		T.visu = visu;
		trace_test_queue.push_back(T);
	}

	double STLDriver::get_param(const string &param)
	{
		if (param_map.find(param) != param_map.end())
		{
			return param_map[param];
		}
		return 0;
	}

	void STLDriver::set_param(const string &param, double n)
	{
		// Update param map
		if (param_map.find(param) != param_map.end())
		{
			param_map[param] = n;
		}
		// Update param map for all formulas
		for (auto it = formula_map.begin(); it != formula_map.end(); it++)
		{
		((*it).second)->set_param(param, n);
		}
	}

	/** Note: test inserted in the stl_test_map have no local parameters */
	void STLDriver::add_stl_test(const string &test_id)
	{

		nb_test_total++;
		if (!trace_test_queue.empty())
		{
			map<string, stl_test>::iterator elem;
			if ((elem = stl_test_map.find(test_id)) != stl_test_map.end())
			{
				trace_test_queue.back().tests.push_back(elem->second);
				trace_test_queue.back().tests.back().formula = (elem->second).formula->clone();
				trace_test_queue.back().tests.back().param_map.clear();
			}
		}
	}

	void STLDriver::add_stl_test(const string &test_id, const map<string, double> &local_param_map)
	{

		nb_test_total++;
		if (!trace_test_queue.empty())
		{
			map<string, stl_test>::iterator elem;
			if ((elem = stl_test_map.find(test_id)) != stl_test_map.end())
			{

				//			cout << "Test " << elem->first;
				//			if (!local_param_map.empty()) {
				//				cout << " with local param:" << endl;
				//				for (auto elemp = local_param_map.begin(); elemp != local_param_map.end(); elemp++)
				//					cout << elemp->first << "=" <<  elemp->second << endl;
				//			}
				//			else
				//				cout << endl;

				trace_test_queue.back().tests.push_back(elem->second);
				trace_test_queue.back().tests.back().formula = (elem->second).formula->clone();
				trace_test_queue.back().tests.back().param_map = local_param_map;
			}
		}
	}

	void STLDriver::add_stl_test(const string &test_id, const map<string, double> &local_param_map, transducer *formula, const string &report_pos,
								 const string &report_neg, bool is_err)
	{

		nb_test_total++;
		if (!trace_test_queue.empty())
		{
			//		cout << "Inserting test " << test_id;
			//		if (!local_param_map.empty()) {
			//			cout << " with local param:" << endl;
			//			for (auto elemp = local_param_map.begin(); elemp != local_param_map.end(); elemp++)
			//				cout << elemp->first << "=" <<  elemp->second << endl;
			//		}
			//		else
			//			cout << endl;

			stl_test_map[test_id] = stl_test(test_id, local_param_map, formula->clone(), report_pos, report_neg, is_err);
			stl_test_map[test_id].param_map = local_param_map;
			stl_test_map[test_id].formula = formula->clone();
			trace_test_queue.back().tests.push_back(stl_test_map[test_id]);
			trace_test_queue.back().tests.back().formula = formula->clone();
		}
	}

	double STLDriver::test_formula(const string &phi_in)
	{

		if (data.empty())
		{
			cout << "Empty data" << endl;
			return 0.;
		}
		string funky_name = "f_u_n_k_y_p_h__i_n_a_m_e"; // seriously?
		string str_to_parse = funky_name + ":=" + phi_in;
		if (parse_string(str_to_parse))
		{
			transducer *phi = formula_map[funky_name]->clone();
			formula_map.erase(funky_name);
			phi->trace_data_ptr = &data;
			phi->init_horizon();
			double rob = phi->compute_robustness();
			delete phi;
			return rob;
		}
		else
		{
			cout << "Couldn't parse formula: " << phi_in << endl;
			return 0.;
		}
	}

	STLMonitor STLDriver::get_monitor(const string& id) const {
		STLMonitor phi;
		auto it = formula_map.find(id); // FYI was map<string,transducer*>::const_iterator it;

		if (it != formula_map.end() && it->second != nullptr)
		{	
			if (it->second != nullptr)
			{
				try
				{
					phi.formula = (it->second)->clone();
					phi.signal_map = signal_map;
					phi.param_map = param_map;
					phi.formula->set_horizon(0., 0.);

					//phi.formula->set_trace_data_ptr(phi.data); // Premature, pointer with change
					phi.current_time = 0.;
					phi.rob = 0.;
					phi.lower_rob = -Signal::BigM;
					phi.upper_rob = Signal::BigM;
				}
				catch (const std::exception &e)
				{
					std::cerr << "Error while cloning or setting up the formula: " << e.what() << std::endl;
					// Handle the error appropriately, possibly by returning an empty or default-initialized STLMonitor
					return STLMonitor();
				}
			}
		}
		else
		{
			cout << "WARNING: Formula " << id << " undefined." << endl;
		}
		
		return phi;
	}

	vector<double> &STLDriver::get_online_rob(const string &phi_in, double t0=0.)
	{
		// transducer->param_map = param_map;
		static vector<double> out_rob;
		if (data.empty())
		{
			cout << "Empty data" << endl;
			return out_rob;
		}
		string funky_name = "f_u_n_k_y_p_h__i_n_a_m_e"; // seriously?
		string str_to_parse = funky_name + ":=" + phi_in;
		if (parse_string(str_to_parse))
		{
			transducer *phi = formula_map[funky_name]->clone();
			formula_map.erase(funky_name);
			phi->set_trace_data_ptr(data);			
			phi->set_param_map_ptr(param_map);
			phi->start_time=t0;
			phi->end_time=t0;			
			phi->init_horizon();
			double rob = phi->compute_robustness();
			double lower_rob = phi->compute_lower_rob();
			double upper_rob = phi->compute_upper_rob();
			out_rob = {rob, lower_rob, upper_rob};
			delete phi;
			return out_rob;
		}
		else
		{
			cout << "Couldn't parse formula: " << phi_in << endl;
			return out_rob;
		}
	}

	vector<double> &STLDriver::get_online_rob(const string &phi_in)
	{
		return get_online_rob(phi_in, 0.);
	}


	void STLDriver::print_trace(ostream &os)
	{
		for (auto ii = data.begin(); ii != data.end(); ii++)
		{
			for (auto jj = (*ii).begin(); jj != (*ii).end(); jj++)
			{
				os << *jj << " ";
			}
			os << endl;
		}
	}

	void STLDriver::print_trace()
	{
		print_trace(cout);
	}

	void STLDriver::dump_trace_file(const string &filename)
	{
		fstream ofs;
		ofs.open(filename.c_str(), std::ofstream::out);
		if (ofs.is_open())
		{
			for (auto ii = data.begin(); ii != data.end(); ii++)
			{
				for (auto jj = (*ii).begin(); jj != (*ii).end(); jj++)
				{
					ofs << *jj << " ";
				}
				ofs << endl;
			}
			ofs.close();
		}
		else
		{
			cout << "Couldn't open file " << filename.c_str() << " for writing signal" << endl; // TODO implement exception
		}
	}

	bool STLDriver::dump_test_log_file(const string &filename)
	{

		fstream ofs;
		ofs.open(filename.c_str(), std::ofstream::out);
		if (ofs.is_open())
		{
			ofs << test_log << endl;
			ofs << "Number of positive STL tests: " << nb_test_pos << "/" << nb_test_total;
			ofs.close();
			return true;
		}
		else
		{
			cout << "Couldn't open file " << filename.c_str() << " for writing signal" << endl; // TODO implement exception
			return false;
		}
	}

} // namespace STLRom
