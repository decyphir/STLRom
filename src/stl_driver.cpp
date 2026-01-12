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
#include "command.h"
#include "transducer.h"
#include "parser.hpp" // this is needed for symbol_type

#include <sstream>

using namespace STLRom;

STLDriver::STLDriver() :
    m_commands(),
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_location(0)

{

}

int STLDriver::parse() {
    m_location = 0;
    return m_parser.parse();
}

void STLDriver::clear() {
    m_location = 0;
    m_commands.clear();
}

std::string STLDriver::str() const {
    std::stringstream s;
    s << "STLDriver: " << m_commands.size() << " commands received from command line." << endl;
    for(int i = 0; i < m_commands.size(); i++) {
        s << " * " << m_commands[i].str() << endl;
    }
    return s.str();
}

void STLDriver::switchInputStream(std::istream *is) {
    m_scanner.switch_streams(is, NULL);
    m_commands.clear();    
}

void STLDriver::addCommand(const Command &cmd)
{
    m_commands.push_back(cmd);
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

