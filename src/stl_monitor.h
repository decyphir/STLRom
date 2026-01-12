#ifndef STL_MONITOR_H
#define STL_MONITOR_H

#include <string>
#include <vector>
#include <map>
#include <deque>
#include "transducer.h"
#include "tools.h"
#include "signal.h"

namespace STLRom
{
    /** A class for standalone STL monitor */
    class STLMonitor
    {
    public:
        trace_data data;
        map<string, double> param_map;
        map<string, int> signal_map;

        double rob;
        double lower_rob;
        double upper_rob;

        double start_time, end_time;
        transducer *formula;
        Semantics semantics;
        Interpol interpol;

        STLMonitor() : interpol(Interpol::LINEAR), semantics(Semantics::SPACE), formula(nullptr), rob(0.0), lower_rob(0.0), upper_rob(0.0), start_time(0.0), end_time(0.0) {}

        // Copy constructor
        STLMonitor(const STLMonitor &other)
            : interpol(other.interpol),
              semantics(other.semantics),
              data(other.data),
              param_map(other.param_map),
              signal_map(other.signal_map),
              rob(other.rob), lower_rob(other.lower_rob), upper_rob(other.upper_rob), start_time(other.start_time), end_time(other.end_time)
        {
            if (other.formula)
            {
                formula = other.formula->clone();
            }
            else
            {
                formula = nullptr;
            }
        }

        // Copy assignment operator
        STLMonitor &operator=(const STLMonitor &other)
        {
            if (this != &other)
            {
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
                if (formula)
                {
                    delete formula;
                }
                if (other.formula)
                {
                    formula = other.formula->clone();
                }
                else
                {
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
                signal_map = std::move(other.signal_map);
                interpol = other.interpol;
                semantics = other.semantics;
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

        inline void set_semantics(const std::string &sem)
        {
            if (sem == "SPACE")
            {
                semantics = Semantics::SPACE;
            }
            else if (sem == "LEFT_TIME")
            {
                semantics = Semantics::LEFT_TIME;
            }
            else if (sem == "RIGHT_TIME")
            {
                semantics = Semantics::RIGHT_TIME;
            }
            else
            {
                throw std::invalid_argument("Invalid semantics string");
            };
        };

        inline std::string get_semantics() const
        {
            switch (semantics)
            {
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
                interpol = Interpol::PREVIOUS;
            }
            else if (interp == "LINEAR")
            {
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
        inline void set_eval_time(double t_start, double t_end)
        {
            start_time = t_start;
            end_time = t_end;
        }

        double eval_rob();
        double eval_rob(double);
        double eval_rob(double, double);

        string get_signal_names() const;

        inline Signal get_signal() const
        {
            return formula->get_signal();
        }

        // display stuff
        inline void display_signal_names() const
        {
            for (const auto &signal : signal_map)
            {
                cout << signal.first << ": " << signal.second << endl;
            }
        }

        inline void display_formula() const
        {
            if (formula)
            {
                cout << *formula << endl;
            }
            else
            {
                cout << "No formula set." << endl;
            }
        }

        friend ostream &operator<<(ostream &os, const STLMonitor &monitor)
        {
            os << "Signal Names: ";
            bool first = true;
            for (const auto &signal : monitor.signal_map)
            {
                os << signal.first;
                if (&signal != &(*std::prev(monitor.signal_map.end())))
                {
                    os << ", ";
                }
            }
            os << endl;
            os << "Parameters: ";
            for (const auto &param : monitor.param_map)
            {
                os << param.first << ": " << param.second;
                if (&param != &(*std::prev(monitor.param_map.end())))
                {
                    os << ", ";
                }
            }
            os << endl;
            os << "Formula: ";
            if (monitor.formula)
            {
                os << *monitor.formula;
            }
            else
            {
                os << "No formula set.";
            }
            os << endl;

            return os;
        }
    };
}
#endif