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

#include <iostream>
#include <fstream>
#include "scanner.h"
#include "parser.hpp"
#include "stl_driver.h"

constexpr const char* CYAN  = "\033[36m";
constexpr const char* RESET = "\033[0m";
constexpr const char* RED  = "\033[31m";


using namespace STLRom;
using namespace std;

int main(int argc, char **argv) {
    STLDriver i;
    std::ifstream inFile;
    bool res;

    // i.trace_parsing = true; // Enable parser debug output
    // i.trace_scanning = true; // Enable scanner debug output

    if (argc > 1) {
        res = i.parse_file(argv[1], false);
    } else {
        cout << CYAN << "Parsing from standard input. Type your input and press Ctrl+D (Linux/Mac) or Ctrl+Z (Windows) to end input." << RESET << endl;
        res = i.parse_stream(std::cin);
    }

    cout << CYAN << "Parse complete. Result = " << res << RESET << endl;
    return res;
}
