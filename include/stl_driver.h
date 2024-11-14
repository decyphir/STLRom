// $Id: stl_driver.h 176 2016-04-05 03:50:15Z donze $ 	
/** \file stl_driver.h Declaration of the stl_transducer::Driver class. */

#ifndef STL_DRIVER_H
#define STL_DRIVER_H

#include <string>
#include <vector>
#include <map>
#include <deque>
#include "transducer.h"
#include "stl_parser.h"
#include "tools.h"
#include "signal.h"

typedef STLRom::Parser::token token;
typedef STLRom::Parser::token_type token_type;

namespace STLRom {
    string signal_map_to_string(map<string, int>); // TODO move in tools ?
	
    /** A class for standalone STL monitor */
    class STLMonitor {
    public:
        trace_data data; 
        map<string, double> param_map;
        map<string, int> signal_map;

        double rob; 
        double lower_rob;
        double upper_rob;

        double start_time, end_time;
        transducer * formula;
        Semantics semantics; 
        Interpol interpol;

        STLMonitor() : interpol(Interpol::LINEAR), semantics(Semantics::SPACE), formula(nullptr), rob(0.0), lower_rob(0.0), upper_rob(0.0),start_time(0.0), end_time(0.0) {}
 
        // Copy constructor
        STLMonitor(const STLMonitor& other) 
            : interpol(other.interpol), 
             semantics(other.semantics), 
              data(other.data), 
              param_map(other.param_map),
              signal_map(other.signal_map),
              rob(other.rob), lower_rob(other.lower_rob), upper_rob(other.upper_rob), start_time(other.start_time), end_time(other.end_time) {
            if (other.formula) {
                formula = other.formula->clone();
            } else {
                formula = nullptr;
            }
        }

        // Copy assignment operator
        STLMonitor& operator=(const STLMonitor& other) {
            if (this != &other) {
                interpol = other.interpol;
                semantics = other.semantics;
                data = other.data;
                param_map = other.param_map;
                signal_map = other.signal_map;
                rob = other.rob;
                lower_rob = other.lower_rob;
                upper_rob = other.upper_rob;
                start_time = other.start_time;
                end_time = other.end_time;
                if (formula) {
                    delete formula;
                }
                if (other.formula) {
                    formula = other.formula->clone();
                } else {
                    formula = nullptr;
                }
            }
            return *this;
        
        }

        // Move constructor
        STLMonitor(STLMonitor &&other) noexcept
            : interpol(other.interpol),
              semantics(other.semantics),
              data(std::move(other.data)),
              param_map(std::move(other.param_map)),
              signal_map(std::move(other.signal_map)),   
              rob(other.rob), lower_rob(other.lower_rob), upper_rob(other.upper_rob), start_time(other.start_time), end_time(other.end_time), formula(other.formula)
        {
            other.formula = nullptr;
        }

        // Move assignment operator
        STLMonitor &operator=(STLMonitor &&other) noexcept
        {
            if (this != &other)
            {
                data = std::move(other.data);
                param_map = std::move(other.param_map);
                signal_map= std::move(other.signal_map);   
                interpol = other.interpol;
                semantics=other.semantics;
                rob = other.rob;
                lower_rob = other.lower_rob;
                upper_rob = other.upper_rob;
                start_time = other.start_time;
                end_time = other.end_time;
                if (formula)
                {
                    delete formula;
                }
                formula = other.formula;
                other.formula = nullptr;
            }
            return *this;
        }

        ~STLMonitor()
        {
            if (formula)
            {
                delete formula;
            }
        }
        
        inline void set_semantics(const std::string& sem) {
		if (sem == "SPACE") {
			semantics = Semantics::SPACE;
		} else if (sem == "LEFT_TIME") {
			semantics = Semantics::LEFT_TIME;
		} else if (sem == "RIGHT_TIME") {
			semantics = Semantics::RIGHT_TIME;
		} else {
			throw std::invalid_argument("Invalid semantics string");
		};
        };
        
        inline std::string get_semantics() const {
            switch (semantics) {
                case Semantics::SPACE:
                    return "SPACE";
                case Semantics::LEFT_TIME:
                    return "LEFT_TIME";
                case Semantics::RIGHT_TIME:
                    return "RIGHT_TIME";
                default:
                    throw std::invalid_argument("Invalid semantics value");
            }
        }

        inline void set_interpol(const std::string &interp)
        {
            cout << "interpol:" << interp << endl;
            if (interp == "PREVIOUS")
            {
                cout << "prev" << endl;
                interpol = Interpol::PREVIOUS;
            }
            else if (interp == "LINEAR")
            {
                cout << "lin" << endl;
                interpol = Interpol::LINEAR;
            }
            else
            {
                throw std::invalid_argument("Invalid interpol string");
            }
        }

        inline std::string get_interpol()
        {
            switch (interpol)
            {
            case Interpol::PREVIOUS:
                return "PREVIOUS";
            case Interpol::LINEAR:
                return "LINEAR";
            default:
                throw std::invalid_argument("Invalid interpol value");
            }
        }

        // Change parameter value and update robustness
        // Maybe add an option to reset rather than update ?
        inline void set_param(const std::string &param, double value)
        {
            auto it = param_map.find(param);
            if (it != param_map.end())
            {
                double prev_value = it->second;
                if (value != prev_value)
                {
                    it->second = value;
                }
            }
            else
            {
                throw std::invalid_argument("Parameter does not exist in param_map");
            }
        };

        inline double get_param(const std::string &param) const
        {
            auto it = param_map.find(param);
            if (it != param_map.end())
            {
                return it->second;
            }
            else
            {
                throw std::invalid_argument("Parameter does not exist in param_map");
            }
        };
        
        inline void reset_signal_data()
        {
            data.clear();
        };

        inline double get_lower_rob() { return lower_rob; };
        inline double get_upper_rob() { return upper_rob; };
        inline double get_rob() { return rob; };

        // append new sample to data 
        void add_sample(vector<double> s);
        inline void set_eval_time(double t_start, double t_end) {
            start_time = t_start;
            end_time = t_end;
        }
        
        double update_rob();
        
        string get_signal_names() const;

        inline Signal get_signal() const {
            return formula->get_signal();    
        }

       // display stuff
        inline void display_signal_names() const {
            for (const auto& signal : signal_map) {
            cout << signal.first << ": " << signal.second << endl;
            }
        }

        inline void display_formula() const {
            if (formula) {
            cout << *formula << endl;
            } else {
            cout << "No formula set." << endl;
            }
        }
    
    friend ostream& operator<<(ostream& os, const STLMonitor& monitor) {
        os << "Signal Names: ";
        bool first = true;
        for (const auto& signal : monitor.signal_map) {
            os << signal.first;
            if (&signal != &(*std::prev(monitor.signal_map.end()))) {
                os << ", ";
            }
        }
        os << endl;
        os << "Parameters: ";
        for (const auto& param : monitor.param_map) {
            os << param.first << ": " << param.second;
            if (&param != &(*std::prev(monitor.param_map.end()))) {
                os << ", ";
            }
        }
        os << endl;
        os << "Formula: ";
        if (monitor.formula) {
            os << *monitor.formula;
        } else {
            os << "No formula set.";
        }
        os << endl;
        
        return os;
    }
};
    
/** A class encapsulating an STL formula plus feedback messages to be written in
 * the report based on its satisfaction or violation status.  */
    class stl_test {
    public:
        string test_id;
        map<string, double> param_map;
        transducer* formula;
        string report_positive;
        string report_negative;
        bool is_error;

        stl_test(){
            is_error = false;
            formula = nullptr;
        };

        stl_test(const string & _test_id,const map<string, double> &_param_map, transducer* _formula, const string &_report_pos,
                 const string &_report_neg, bool _is_err):
            test_id(_test_id),
            param_map(_param_map),
            formula(_formula),
            report_positive(_report_pos),
            report_negative(_report_neg),
            is_error(_is_err)
        {
        };

        ~stl_test() {
            delete formula;
        }

    };

    struct trace_test {
        string id;
        string env;
        double sim_time;
        bool visu;
        deque<stl_test> tests;
    };

    /** The Driver class brings together all components. It creates an instance of
     * the Parser and Scanner classes and connects them. Then the input stream is
     * fed into the scanner object and the parser gets it's token
     * sequence. Furthermore the driver object is available in the grammar rules as
     * a parameter. Therefore the driver class contains a reference to the
     * structure into which the parsed data is saved. */
    class STLDriver {
    public:

        /// DATA

        /** enable debug output in the flex scanner */
        bool trace_scanning;

        /** enable debug output in the bison parser */
        bool trace_parsing;

        /** stream name (file or input stream) used for error messages. */
        std::string streamname;

        /** Reserved words for STL parser */
        static map<string, token_type> reserved;

        /** parameters in formulas */
        map<string, double> param_map;

        /** signals in formulas */
        map<string, int> signal_map;

        /** formulas defined by the driver */
        map<string, transducer*> formula_map;
    
        /** data array - time is first column */
        trace_data data;
    
        // append new sample to data 
        inline void add_sample(vector<double> s) { // doesn't check time and size consistency (TODO)
                data.push_back(s);    
        };

        /** tests (sets of formulas)  */
        map<string, stl_test> stl_test_map;
        deque<trace_test> trace_test_queue;

        string report;
        string test_log;
        int nb_test_pos;
        int nb_test_total;
        bool error_flag;

        string get_signals_names() const;

        /// CONSTRUCTORS

        /** construct a new parser driver context */
        STLDriver();

        /** construct a parser driver with data */
        STLDriver(trace_data _trace);

        ~STLDriver();

        /** Copy constructor */
        STLDriver(const STLDriver &other);

        /** Copy assignment operator */
        STLDriver &operator=(const STLDriver &other);

        /** Move constructor */
        STLDriver(STLDriver &&other) noexcept;

        /** Move assignment operator */
        STLDriver &operator=(STLDriver &&other) noexcept;

        /** Create a new trace_tests structure and add it to the queue */
        void add_trace_test(const string &test_id, const string &trace_cfg, double sim_time, bool visu);

        /** adds an stl_test from the stl_test_map to the current trace_test with the local params value
         * does nothing if the stl_test does not exist */
        void add_stl_test(const string &test_id);
        void add_stl_test(const string &test_id,  const map<string,double> &local_param_map);

        /** adds an stl_test to the current trace_tests - does nothing if the queue is empty */
        void add_stl_test(const string &test_id,  const map<string,double> &local_param_map, transducer* formula, const string &report_pos,
                          const string &report_neg, bool is_err);

        /** returns value of parameter param */
        double get_param(const string &param);
        
        void set_param(const string &param,double n);

        /// PARSER

        /** Invoke the scanner and parser for a stream.
         * @param in	input stream
         * @param sname	stream name for error messages
         * @return		true if successfully parsed
         */
        bool parse_stream(std::istream& in);

        /** Invoke the scanner and parser on an input string.
         * @param input	input string
         * @param sname	stream name for error messages
         * @return		true if successfully parsed
         */
        bool parse_string(const std::string& input);

        /** Invoke the scanner and parser on a file. Use parse_stream with a
         * std::ifstream if detection of file reading errors is required.
         * @param filename	input file name
         * @return		true if successfully parsed
         */
        bool parse_file(const std::string& filename);

        // To demonstrate pure handling of parse errors, instead of
        // simply dumping them on the standard error output, we will pass
        // them to the driver using the following two member functions.

        /** Error handling with associated line number. This can be modified to
         * output the error e.g. to a dialog box. */
        void error(const class location& l, const std::string& m);

        /** General error handling. This can be modified to output the error
         * e.g. to a dialog box. */
        void error(const std::string& m);

        /** Pointer to the current lexer instance, this is used to connect the
         * parser to the scanner. It is used in the yylex macro. */
        class Scanner* lexer;

        /// API

        /** runs the tests of the front of trace_tests_queue and pop them
         * assumes that data has been filled with the appropriate trace data */
        bool run_tests();

        /** monitor a single formula requires data is not empty */
        double test_formula(const string &);    
        vector<double>& get_online_rob(const string & phi_in);
        vector<double>& get_online_rob(const string & phi_in, double);


        /** gets next formula to test */
        inline transducer * get_next_formula() const {

            if (!trace_test_queue.front().tests.empty())
                return trace_test_queue.front().tests.front().formula;
            else
                return nullptr;
        }

        inline const stl_test * get_next_stl_test() const {

            if (!trace_test_queue.front().tests.empty())
                return &(trace_test_queue.front().tests.front());
            else
                return nullptr;
        }

        inline const trace_test * get_next_trace_test() const {
            if (!trace_test_queue.empty())
                return &(trace_test_queue.front());
            else
                return nullptr;
        }

        inline const bool trace_test_queue_empty() {
            return trace_test_queue.empty();
        }

        inline const string get_next_trace_test_env() {
            if (!trace_test_queue_empty())
                return trace_test_queue.front().env;
            else
                return string();
        }

        inline void pop_next_trace_test() {
            if (!trace_test_queue_empty())
                trace_test_queue.pop_front();
        }

        // TODO rid of run_tests altogether
        inline void run_next_trace_test() {
            run_tests();
        }

        // Creates an independant monitor with its own data
        STLMonitor get_monitor(const string& id) const;
                                        
        /// UTILITY FUNCTIONS

        /** clear assigned formulas and trace_test_queue */
        void clear();

        /** Prints driver to stream output */
        void print(ostream &out) const;

        /** Prints driver to standard output */
        inline void print() const { print(cout); };
        inline void disp() { print(cout); };

        /** Read a trace file */
        inline bool read_trace_file(string trace_file_name) {
            data.clear();
            return read_trace(trace_file_name, data);
        }

        /** dump all assigned formulas satisfaction function to a file */
        void dump();
        void dump_trace_file(const string&);
        bool dump_test_log_file(const string&);
        void print_trace(ostream &os);
        void print_trace();
    }
        ;

 // namespace STLRom
}
#endif // STL_DRIVER_H
