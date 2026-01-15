/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Krzysztof Narkiewicz <krzysztof.narkiewicz@STLRom.com>
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

#include "stl_driver.h"
#include "transducer.h"
#include "parser.hpp" // this is needed for symbol_type

#include <sstream>

using namespace STLRom;

constexpr const char* CYAN  = "\033[36m";
constexpr const char* RED  = "\033[31m";
constexpr const char* RESET = "\033[0m";

STLDriver::STLDriver() :
    m_scanner(*this),
    m_parser(m_scanner, *this),
    interpol(Interpol::LINEAR),
    semantics(Semantics::SPACE),
    trace_scanning(false),
    trace_parsing(false),
    verbose_parser(false)
    // report(""),
    // nb_test_pos(0),
    // nb_test_total(0),
    // error_flag(false)
{

}

STLDriver::STLDriver(trace_data _trace) :
    m_scanner(*this),
    m_parser(m_scanner, *this),
    data(std::move(_trace)),
    interpol(Interpol::LINEAR),
    semantics(Semantics::SPACE),
    trace_scanning(false),
    trace_parsing(false),
    verbose_parser(false)
    // report(""),
    // nb_test_pos(0),
    // nb_test_total(0),
    // error_flag(false)
{

}

STLDriver::~STLDriver()
{
    for (auto &pair : formula_map)
    {
        delete pair.second;
    }
}

STLDriver::STLDriver(const STLDriver &other) :
    m_scanner(*this),
    m_parser(m_scanner, *this),
    semantics(other.semantics),
    interpol(other.interpol),
    trace_scanning(other.trace_scanning),
    trace_parsing(other.trace_parsing),
    verbose_parser(other.verbose_parser),
    streamname(other.streamname),
    param_map(other.param_map),
    signal_map(other.signal_map),
    data(other.data)
    // stl_test_map(other.stl_test_map),
    // trace_test_queue(other.trace_test_queue),
    // report(other.report),
    // test_log(other.test_log),
    // nb_test_pos(other.nb_test_pos),
    // nb_test_total(other.nb_test_total),
    // error_flag(other.error_flag)
{
    // Deep copy of formula_map
    for (const auto &pair : other.formula_map)
    {
        formula_map[pair.first] = pair.second->clone();
    }
}

STLDriver &STLDriver::operator=(const STLDriver &other)
{
    if (this != &other)
    {
        // Clean up existing formula_map
        for (auto &pair : formula_map)
        {
            delete pair.second;
        }
        formula_map.clear();

        semantics = other.semantics;
        interpol = other.interpol;
        trace_scanning = other.trace_scanning;
        trace_parsing = other.trace_parsing;
        verbose_parser = other.verbose_parser;
        streamname = other.streamname;
        param_map = other.param_map;
        signal_map = other.signal_map;
        data = other.data;
        // stl_test_map = other.stl_test_map;
        // trace_test_queue = other.trace_test_queue;
        // report = other.report;
        // test_log = other.test_log;
        // nb_test_pos = other.nb_test_pos;
        // nb_test_total = other.nb_test_total;
        // error_flag = other.error_flag;

        // Deep copy of formula_map
        for (const auto &pair : other.formula_map)
        {
            formula_map[pair.first] = pair.second->clone();
        }
    }
    return *this;
}

STLDriver::STLDriver(STLDriver &&other) noexcept :
    m_scanner(*this),
    m_parser(m_scanner, *this),
    semantics(other.semantics),
    interpol(other.interpol),
    trace_scanning(other.trace_scanning),
    trace_parsing(other.trace_parsing),
    verbose_parser(other.verbose_parser),
    streamname(std::move(other.streamname)),
    param_map(std::move(other.param_map)),
    signal_map(std::move(other.signal_map)),
    data(std::move(other.data)),
    formula_map(std::move(other.formula_map))
    // stl_test_map(std::move(other.stl_test_map)),
    // trace_test_queue(std::move(other.trace_test_queue)),
    // report(std::move(other.report)),
    // test_log(std::move(other.test_log)),
    // nb_test_pos(other.nb_test_pos),
    // nb_test_total(other.nb_test_total),
    // error_flag(other.error_flag)
{
    other.formula_map.clear();
}

STLDriver &STLDriver::operator=(STLDriver &&other) noexcept
{
    if (this != &other)
    {
        // Clean up existing formula_map
        for (auto &pair : formula_map)
        {
            delete pair.second;
        }

        semantics = other.semantics;
        interpol = other.interpol;
        trace_scanning = other.trace_scanning;
        trace_parsing = other.trace_parsing;
        verbose_parser = other.verbose_parser;
        streamname = std::move(other.streamname);
        param_map = std::move(other.param_map);
        signal_map = std::move(other.signal_map);
        data = std::move(other.data);
        formula_map = std::move(other.formula_map);
        // stl_test_map = std::move(other.stl_test_map);
        // trace_test_queue = std::move(other.trace_test_queue);
        // report = std::move(other.report);
        // test_log = std::move(other.test_log);
        // nb_test_pos = other.nb_test_pos;
        // nb_test_total = other.nb_test_total;
        // error_flag = other.error_flag;

        other.formula_map.clear();
    }
    return *this;
}



bool STLDriver::parse_stream(std::istream &in) {
    m_scanner.switch_streams(&in, NULL);
    return (parse() == 0);
}

bool STLDriver::parse_file(const std::string &filename) {
    return parse_file(filename, false);
}

bool STLDriver::parse_file(const std::string &filename, bool verbose) {
    std::ifstream in(filename.c_str());
    
    if (!in.good()) {
        std::cerr << RED << "STLDriver::parse_file(): Could not open file: " << filename << std::endl << RED;
        return false;
    }
    
    if (verbose) {
        cout << CYAN << "Parsing from file with content:" << RESET << endl;
        cout << CYAN << "----------------------------------------" << RESET << endl;
        cout << CYAN << in.rdbuf() << RESET << endl;
        cout << CYAN << "----------------------------------------" << RESET << endl;

        in.clear(); // clear eof flag
        in.seekg(0, ios::beg); // rewind to beginning
    }

    return parse_stream(in);
}

bool STLDriver::parse_string(const std::string &input) {
    std::istringstream iss(input);
    return parse_stream(iss);
}


int STLDriver::parse() {
    m_parser.set_debug_level(trace_parsing);
    m_scanner.set_debug(trace_scanning);
    return m_parser.parse();
}

void STLDriver::clear() {
    for (auto formula = formula_map.begin(); formula != formula_map.end(); formula++)
		{
			if (formula->second != 0)
			{
				delete formula->second;
				formula->second = 0;
			}
		}

		formula_map.clear();
		param_map.clear();
}

std::string STLDriver::str() const {
    return "TODO";
}

void STLDriver::switchInputStream(std::istream *is) {
    m_scanner.switch_streams(is, NULL);
    // TODO : clear AST, etc.
}

// TODO: improve error messages by adding location info from scanner
// TODO: call error messages from parser
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


vector<double> STLDriver::get_online_rob(const string &phi_in, double t0 = 0.)
{
    // static vector<double> out_rob;
    vector<double> out_rob;
    if (data.empty())
    {
        cout << "Empty data" << endl;
        return out_rob;
    }

    if (formula_map.find(phi_in) == formula_map.end())
    {
        cout << "Formula " << phi_in << " not found in formula_map." << endl;
        return out_rob;
    }
    transducer *phi = formula_map[phi_in];
    phi->set_trace_data_ptr(data);
    phi->set_param_map_ptr(param_map);
    Signal::semantics = semantics;
    Signal::interpol = interpol;
    phi->reset();
    phi->set_horizon(t0, t0);
    double rob = phi->compute_robustness();
    double lower_rob = phi->compute_lower_rob();
    double upper_rob = phi->compute_upper_rob();
    out_rob = {rob, lower_rob, upper_rob};

    return out_rob;
}

vector<double> STLDriver::get_online_rob(const string &phi_in)
{
    return get_online_rob(phi_in, 0.);
}

STLMonitor STLDriver::get_monitor(const string &id) const
{
    STLMonitor phi;
    auto it = formula_map.find(id); // FYI was map<string,transducer*>::const_iterator it;

    if (it != formula_map.end() && it->second != nullptr)
    {
        if (it->second != nullptr)
        {
            try
            {
                phi.interpol = interpol;
                phi.semantics = semantics;
                phi.data = data;
                phi.formula = (it->second)->clone();
                phi.signal_map = signal_map;
                phi.param_map = param_map;
                phi.start_time = 0.;
                phi.end_time = 0.;
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


void STLDriver::add_sample(vector<double> s)
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

string STLDriver::get_signals_names() const
{
    return signal_map_to_string(signal_map);
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