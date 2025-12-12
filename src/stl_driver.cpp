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
#include "parser.hpp" // this is needed for symbol_type

#include <sstream>

using namespace STLRom;

map<string, token_type> STLDriver::reserved = map<string, token_type>(); // filled in stl_scanner.lpp

STLDriver::STLDriver() :
    m_commands(),
    m_scanner(*this),
    m_parser(m_scanner, *this),
    m_location(0)

{
    initReserved();
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
    cout << "increaseLocation(): " << loc << ", total = " << m_location << endl;
}

unsigned int STLDriver::location() const {
    return m_location;
}

void initReserved() {
    STLDriver::reserved["F"]      = token::TOKEN_DIAMOND;
    STLDriver::reserved["F_"]     = token::TOKEN_DIAMOND;
    STLDriver::reserved["ev"]     = token::TOKEN_DIAMOND;
    STLDriver::reserved["ev_"]    = token::TOKEN_DIAMOND;

    STLDriver::reserved["G"]      = token::TOKEN_BOX;
    STLDriver::reserved["G_"]     = token::TOKEN_BOX;
    STLDriver::reserved["alw"]    = token::TOKEN_BOX;
    STLDriver::reserved["alw_"]   = token::TOKEN_BOX;

    STLDriver::reserved["U"]      = token::TOKEN_UNTIL;
    STLDriver::reserved["U_"]     = token::TOKEN_UNTIL;
    STLDriver::reserved["until"]  = token::TOKEN_UNTIL;
    STLDriver::reserved["until_"] = token::TOKEN_UNTIL;

    STLDriver::reserved["and"]    = token::TOKEN_AND;
    STLDriver::reserved["or"]     = token::TOKEN_OR;
    STLDriver::reserved["not"]    = token::TOKEN_NOT;

    STLDriver::reserved["true"]   = token::TOKEN_BOOL_TRUE;
    STLDriver::reserved["false"]  = token::TOKEN_BOOL_FALSE;

    STLDriver::reserved["t"]      = token::TOKEN_TIME;
    STLDriver::reserved["abs"]    = token::TOKEN_ABS;
    STLDriver::reserved["param"]  = token::TOKEN_PARAM_DECL;
    STLDriver::reserved["test"]   = token::TOKEN_TEST;
    STLDriver::reserved["signal"] = token::TOKEN_SIGNAL_DECL;

    STLDriver::reserved["IDENTIFIER"] = token::TOKEN_CONSTANT_IDENTIFIER; // default for unknown identifiers
}

Parser::symbol_type make_keyword(const std::string &kw, const location_type &loc) {
    auto it = STLDriver::reserved.find(kw);
    if(it != STLDriver::reserved.end()) {
        return Parser::symbol_type(it->second, std::move(loc));
    } else {
        return Parser::symbol_type(STLDriver::reserved["IDENTIFIER"], std::move(loc));
    }
}
