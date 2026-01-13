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

STLDriver::STLDriver() :
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_location(0),
    interpol(Interpol::LINEAR),
    semantics(Semantics::SPACE),
    trace_scanning(false),
    trace_parsing(false),
    // report(""),
    // nb_test_pos(0),
    // nb_test_total(0),
    error_flag(false)
{

}

STLDriver::STLDriver(trace_data _trace) :
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_location(0),
    data(std::move(_trace)),
    interpol(Interpol::LINEAR),
    semantics(Semantics::SPACE),
    trace_scanning(false),
    trace_parsing(false),
    // report(""),
    // nb_test_pos(0),
    // nb_test_total(0),
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

STLDriver::STLDriver(const STLDriver &other) :
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_location(other.m_location),
    semantics(other.semantics),
    interpol(other.interpol),
    trace_scanning(other.trace_scanning),
    trace_parsing(other.trace_parsing),
    streamname(other.streamname),
    param_map(other.param_map),
    signal_map(other.signal_map),
    data(other.data),
    // stl_test_map(other.stl_test_map),
    // trace_test_queue(other.trace_test_queue),
    // report(other.report),
    // test_log(other.test_log),
    // nb_test_pos(other.nb_test_pos),
    // nb_test_total(other.nb_test_total),
    error_flag(other.error_flag)
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

        m_location = other.m_location;
        semantics = other.semantics;
        interpol = other.interpol;
        trace_scanning = other.trace_scanning;
        trace_parsing = other.trace_parsing;
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
        error_flag = other.error_flag;

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
    m_location(other.m_location),
    semantics(other.semantics),
    interpol(other.interpol),
    trace_scanning(other.trace_scanning),
    trace_parsing(other.trace_parsing),
    streamname(std::move(other.streamname)),
    param_map(std::move(other.param_map)),
    signal_map(std::move(other.signal_map)),
    data(std::move(other.data)),
    formula_map(std::move(other.formula_map)),
    // stl_test_map(std::move(other.stl_test_map)),
    // trace_test_queue(std::move(other.trace_test_queue)),
    // report(std::move(other.report)),
    // test_log(std::move(other.test_log)),
    // nb_test_pos(other.nb_test_pos),
    // nb_test_total(other.nb_test_total),
    error_flag(other.error_flag)
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

        m_location = other.m_location;
        semantics = other.semantics;
        interpol = other.interpol;
        trace_scanning = other.trace_scanning;
        trace_parsing = other.trace_parsing;
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
        error_flag = other.error_flag;

        other.formula_map.clear();
    }
    return *this;
}


// TODO: debug levels in lexer and parser

bool STLDriver::parse_stream(std::istream &in) {
    m_scanner.switch_streams(&in, NULL);
    return (m_parser.parse() == 0);
}

bool STLDriver::parse_file(const std::string &filename) {
    std::ifstream in(filename.c_str());
    
    if (!in.good()) {
        std::cerr << "STLDriver::parse_file(): Could not open file: " << filename << std::endl;
        return false;
    }

    return parse_stream(in);
}

bool STLDriver::parse_string(const std::string &input) {
    std::istringstream iss(input);
    return parse_stream(iss);
}


int STLDriver::parse() {
    m_location = 0;
    return m_parser.parse();
}

void STLDriver::clear() {
    m_location = 0;
    // TODO : clear AST, etc.
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


void STLDriver::increaseLocation(unsigned int loc) {
    m_location += loc;
    //cout << "increaseLocation(): " << loc << ", total = " << m_location << endl;
}

unsigned int STLDriver::location() const {
    return m_location;
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

