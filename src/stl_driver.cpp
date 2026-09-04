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
    worker(&data),
    trace_scanning(false),
    trace_parsing(false),
    verbose_parser(false)
{

}

STLDriver::STLDriver(trace_data _trace) :
    m_scanner(*this),
    m_parser(m_scanner, *this),
    data(std::move(_trace)),
    worker(&data),
    trace_scanning(false),
    trace_parsing(false),
    verbose_parser(false)
{

}

STLDriver::~STLDriver()
{
    for (auto &pair : formula_map)
    {
        delete pair.second;
    }
    for (auto &pair : function_map)
    {
        delete pair.second;
    }
    worker.formula = nullptr; // FIXME: a better solution with smart pointers?
}

STLDriver::STLDriver(const STLDriver &other) :
    m_scanner(*this),
    m_parser(m_scanner, *this),
    trace_scanning(other.trace_scanning),
    trace_parsing(other.trace_parsing),
    verbose_parser(other.verbose_parser),
    streamname(other.streamname),
    worker(other.worker),
    data(other.data)
{
    // Deep copy of formula_map
    for (const auto &pair : other.formula_map)
    {
        formula_map[pair.first] = pair.second->clone();
    }
    // Deep copy of function_map
    for (const auto &pair : other.function_map)
    {
        function_map[pair.first] = pair.second->clone();
    }
    worker.data = &data;
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
        // Clean up existing function_map
        for (auto &pair : function_map)
        {
            delete pair.second;
        }
        function_map.clear();

        trace_scanning = other.trace_scanning;
        trace_parsing = other.trace_parsing;
        verbose_parser = other.verbose_parser;
        streamname = other.streamname;
        worker = other.worker;
        data = other.data;
 
        // Deep copy of formula_map
        for (const auto &pair : other.formula_map)
        {
            formula_map[pair.first] = pair.second->clone();
        }
        
        // Deep copy of function_map
        for (const auto &pair : other.function_map)
        {
            function_map[pair.first] = pair.second->clone();
        }
        
        worker.data = &data;
    }
    return *this;
}

STLDriver::STLDriver(STLDriver &&other) noexcept :
    m_scanner(*this),
    m_parser(m_scanner, *this),
    trace_scanning(other.trace_scanning),
    trace_parsing(other.trace_parsing),
    verbose_parser(other.verbose_parser),
    streamname(std::move(other.streamname)),
    worker(std::move(other.worker)),
    data(std::move(other.data)),
    formula_map(std::move(other.formula_map)),
    function_map(std::move(other.function_map))
 
{
    other.formula_map.clear();
    other.function_map.clear();

    worker.data = &data;
    other.worker.data = nullptr;
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
        
        // Clean up existing function_map
        for (auto &pair : function_map)
        {
            delete pair.second;
        }

        trace_scanning = other.trace_scanning;
        trace_parsing = other.trace_parsing;
        verbose_parser = other.verbose_parser;
        streamname = std::move(other.streamname);
        worker = std::move(other.worker);
        data = std::move(other.data);
        formula_map = std::move(other.formula_map);
        function_map = std::move(other.function_map);
 
        other.formula_map.clear();
        other.function_map.clear();

        worker.data = &data;
        other.worker.data = nullptr;
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
    for (auto formula = function_map.begin(); formula != function_map.end(); formula++)
		{
			if (formula->second != 0)
			{
				delete formula->second;
				formula->second = 0;
			}
		}

		function_map.clear();
        // TODO : clear worker
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
    return worker.get_param(param);
}

void STLDriver::set_param(const string &param, double n)
{
    worker.set_param(param, n);
}

double STLDriver::get_rob(const string &phi_in, double t0 = 0.)
{
    if (formula_map.find(phi_in) == formula_map.end())
    {
        cout << "Formula " << phi_in << " not found in formula_map." << endl;
        return 0.;
    }
    transducer *phi = formula_map[phi_in];
    
    worker.set_formula(phi);
    return worker.get_rob(t0);        
}

double STLDriver::get_rob(const string &phi_in)
{
    return get_rob(phi_in, 0.);
}


vector<double> STLDriver::get_online_rob(const string &phi_in, double t0 = 0.)
{
    if (formula_map.find(phi_in) == formula_map.end())
    {
        cout << "Formula " << phi_in << " not found in formula_map." << endl;
        vector<double> out_rob;
        return out_rob;
    }
    transducer *phi = formula_map[phi_in];
    worker.set_formula(phi);
    return worker.get_online_rob(t0);
}

Signal STLDriver::get_rob_signal(const string &phi_in, double t_start, double t_end)
{

    if (formula_map.find(phi_in) == formula_map.end())
    {
        cout << "Formula " << phi_in << " not found in formula_map." << endl;
        return Signal();
    }
    transducer *phi = formula_map[phi_in];
    
    worker.set_formula(phi);
    return worker.get_rob_signal(t_start, t_end);
}

Signal STLDriver::get_rob_signal(const string &phi_in)
{
    return get_rob_signal(phi_in, 0., 0.);
}

Signal STLDriver::get_rob_signal(const string &phi_in, double t)
{
    return get_rob_signal(phi_in, t, t);
}

vector<Signal> STLDriver::get_online_rob_signal(const string &phi_in, double t_start, double t_end)
{
    vector<Signal> out_rob;

    if (formula_map.find(phi_in) == formula_map.end())
    {
        cout << "Formula " << phi_in << " not found in formula_map." << endl;
        return out_rob;
    }
    transducer *phi = formula_map[phi_in];

    worker.set_formula(phi);
    return worker.get_online_rob_signal(t_start, t_end);
}


// TODO : how to handle this in monitor? set_formula resets the monitor
robustness_map_t STLRom::STLDriver::get_robustness_map(const string &phi_in)
{
    robustness_map_t rob_map;
    if (formula_map.find(phi_in) == formula_map.end())
    {
        cout << "Formula " << phi_in << " not found in formula_map." << endl;
    } else {
        transducer *phi = formula_map[phi_in];
        phi->fill_robustness_map(rob_map, 0);
    }
    return rob_map;
}

robustness_map_t STLRom::STLDriver::get_online_robustness_map(const string &phi_in)
{
    robustness_map_t rob_map;
    if (formula_map.find(phi_in) == formula_map.end())
    {
        cout << "Formula " << phi_in << " not found in formula_map." << endl;
    } else {
        transducer *phi = formula_map[phi_in];

        phi->fill_online_robustness_map(rob_map, 0);
    }
    return rob_map;
}

vector<Signal> STLDriver::get_online_rob_signal(const string &phi_in)
{
    return get_online_rob_signal(phi_in, 0., 0.);
}

vector<Signal> STLDriver::get_online_rob_signal(const string &phi_in, double t)
{
    return get_online_rob_signal(phi_in, t, t);
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
                phi.semantics = worker.semantics;
                phi.owned_data = STLData(data);
                phi.data = &phi.owned_data;
                phi.formula = (it->second)->clone();
                phi.param_map = worker.param_map;
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
    data.add_sample(s);
}

void STLDriver::add_sample(vector<double> s, bool interp)
{
    data.add_sample(s, interp);
}

void STLDriver::add_signal_sample(string sig, double t, double v)
{
    data.add_signal_sample(sig, t, v);
}

void STLDriver::add_signal_sample(string sig, double t, double v, double d)
{
    data.add_signal_sample(sig, t, v, d);
}

void STLDriver::add_signal_sample(string sig, double t, double v, double d, bool interp)
{
    data.add_signal_sample(sig, t, v, d, interp);   
}


void STLDriver::set_signals(const std::vector<Signal>& signals)
{
    data.set_data_vector(signals);
}

// TODO manage with STLData
void STLDriver::load_csv(const vector<string>& files)
{
    if (files.size() != data.signal_map.size()) {
        throw std::invalid_argument("Number of files does not match the number of declared signals.");
    }

    for (int i = 0; i < files.size(); i++) {
        data.data_vector[i].read_from_file(files[i]);
    }
}

void STLDriver::write_csv(const std::string& directory) const
{
    string dir = directory;
    
    if (dir.back() != '/') dir += '/';

    for (const auto &signal : data.signal_map)
    {
        string filename = dir + signal.first + ".csv";

        data.data_vector[signal.second].write_to_file(filename);
    }
}

string STLDriver::get_signals_names() const
{
    return signal_map_to_string(data.signal_map);
}

// void STLDriver::print_trace(ostream &os)
// {
//     for (auto ii = data.begin(); ii != data.end(); ii++)
//     {
//         for (auto jj = (*ii).begin(); jj != (*ii).end(); jj++)
//         {
//             os << *jj << " ";
//         }
//         os << endl;
//     }
// }

// void STLDriver::print_trace()
// {
//     print_trace(cout);
// }

// void STLDriver::dump_trace_file(const string &filename)
// {
//     fstream ofs;
//     ofs.open(filename.c_str(), std::ofstream::out);
//     if (ofs.is_open())
//     {
//         for (auto ii = data.begin(); ii != data.end(); ii++)
//         {
//             for (auto jj = (*ii).begin(); jj != (*ii).end(); jj++)
//             {
//                 ofs << *jj << " ";
//             }
//             ofs << endl;
//         }
//         ofs.close();
//     }
//     else
//     {
//         cout << "Couldn't open file " << filename.c_str() << " for writing signal" << endl; // TODO implement exception
//     }
// }

/** compute robustness for all formulas defined in the driver and write results in files */
void STLDriver::dump()
{

    // transducer::param_map = param_map; FIXME fix what ??

    for (auto formula = formula_map.begin(); formula != formula_map.end(); formula++)
    {
        formula->second->trace_data_ptr = &data.data_vector;
        formula->second->init_horizon();
        formula->second->compute_robustness();
        (formula->second->z).dumpFile(formula->first + ".out");
    }
}

void STLDriver::print(ostream &out) const
{
    out << "# STLDriver object defined as" << endl;
    out << "signal ";    
    for (const auto &signal : data.signal_map)
    {
        out << signal.first;
        if (&signal != &(*std::prev(data.signal_map.end())))
        {
            out << ", ";
        }
    }
    out << endl;
    
    if (!worker.param_map.empty()) {
        out << "param ";        
        for (const auto &param : worker.param_map)
        {
            out << param.first<< "=" << param.second;            
            if (&param != &(*std::prev(worker.param_map.end())))
            {
                out << ", ";
            }
        }
        out << endl;
    }
    
    
    out << "\n# With formulas:" << endl;

    if (formula_map.empty())
        out <<  "# No formula yet." << endl;
    else 
    {
        for (auto formula = formula_map.begin(); formula != formula_map.end(); formula++)
        {
            out << formula->first << ":= " << *(formula->second) << endl;
        }
    }
    
    if (!function_map.empty()) 
    {
        out << "\n# With functions:" << endl;
        for (auto formula = function_map.begin(); formula != function_map.end(); formula++)
        {
            out << formula->first << ":= " << *(formula->second) << endl;
        }
    }

    
    out << "\n# Data:" << endl;
    
    out << data << endl;
}