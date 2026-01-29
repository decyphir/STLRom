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
    STLDriver driver;
    std::ifstream inFile;
    bool res;

    // i.trace_parsing = true; // Enable parser debug output
    // i.trace_scanning = true; // Enable scanner debug output
    // i.verbose_parser = true; // Enable verbose parser output

    bool from_file = false;
    bool verbose_parser = false;
    std::string filename;

    auto print_help = []() {
        cout << CYAN <<
            "Usage: STLRomParser [options]\n"
            "\nOptions:\n"
            "  -f, --file <filename>   Parse input from file\n"
            "  -v, --verbose           Enable verbose parser output\n"
            "  -d, --debug             Enable parser and scanner tracing\n"
            "  -h, --help              Show this help message\n" << RESET;
    };

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-f" || arg == "--file") {
            if (i + 1 >= argc) {
                cerr << RED << "error: " << arg << " requires a filename" << endl << RESET;
                return 1;
            }
            from_file = true;
            filename = argv[++i];
        } else if (arg == "-v" || arg == "--verbose") {
            verbose_parser = true;
            driver.verbose_parser = true; // Enable verbose parser output
        } else if (arg == "-d" || arg == "--debug") {
            // Enable debug output for scanner and parser
            driver.trace_scanning = true;
            driver.trace_parsing = true;
        } else if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        } else {
            cerr << RED << "error: unknown option '" << arg << "'" << endl;
            cerr << RED << "use --help for usage" << endl << RESET;
            return 1;
        }
    }

    if (from_file) {
        res = driver.parse_file(filename, verbose_parser);
    } else {
        cout << CYAN << "Parsing from standard input. Type your input and press Ctrl+D (Linux/Mac) or Ctrl+Z (Windows) to end input." << RESET << endl;
        res = driver.parse_stream(std::cin);
    }

    cout << CYAN << "Parse complete. Result = " << boolalpha << res << RESET << endl;

    driver.disp();
    return res;
}
