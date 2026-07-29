#ifndef STL_MONITOR_H
#define STL_MONITOR_H

#include <string>
#include <vector>
#include <map>
#include <deque>
#include "transducer.h"
#include "tools.h"
#include "signal.h"
#include "stl_data.h"

namespace STLRom
{
    /** A class for standalone STL monitor */
    class STLMonitor
    {
    public:
        STLData *data; // TODO: do we need to delete data? is it on the heap?
        STLData owned_data;
        map<string, double> param_map;

        double rob;
        double lower_rob;
        double upper_rob;
        bool up_to_date; 

        double start_time, end_time;
        transducer *formula;
        Semantics semantics;

        STLMonitor() : semantics(Semantics::SPACE), formula(nullptr), data(nullptr), rob(0.0), lower_rob(0.0), upper_rob(0.0), up_to_date(false), start_time(0.0), end_time(0.0) {}

        STLMonitor(STLData* dataptr) : STLMonitor()
        {
            data = dataptr;
        }

        // Copy constructor
        STLMonitor(const STLMonitor &other)
            : semantics(other.semantics),
              owned_data(other.owned_data),
              param_map(other.param_map),
              rob(other.rob), lower_rob(other.lower_rob), upper_rob(other.upper_rob), 
              up_to_date(other.up_to_date),
              start_time(other.start_time), end_time(other.end_time)
        {
            if (other.formula)
            {
                formula = other.formula->clone();
            }
            else
            {
                formula = nullptr;
            }

            if (other.data == &other.owned_data)
            {
                data = &owned_data;
            } 
            else
            {
                data = other.data;
            }
        }

        // Copy assignment operator
        STLMonitor &operator=(const STLMonitor &other)
        {
            if (this != &other)
            {
                semantics = other.semantics;
                owned_data = other.owned_data;
                param_map = other.param_map;
                rob = other.rob;
                lower_rob = other.lower_rob;
                upper_rob = other.upper_rob;
                up_to_date= other.up_to_date;
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

                if (other.data == &other.owned_data)
                {
                    data = &owned_data;
                } 
                else
                {
                    data = other.data;
                }
            }
            return *this;
        }

        // Move constructor
        STLMonitor(STLMonitor &&other) noexcept
            : semantics(other.semantics),
              owned_data(std::move(other.owned_data)),
              param_map(std::move(other.param_map)),
              rob(other.rob), lower_rob(other.lower_rob), upper_rob(other.upper_rob), 
              up_to_date(other.up_to_date),
              start_time(other.start_time), end_time(other.end_time), formula(other.formula)
        {
            other.formula = nullptr;
            if (other.data == &other.owned_data)
            {
                data = &owned_data;
            } 
            else
            {
                data = other.data;
            }
        }

        // Move assignment operator
        STLMonitor &operator=(STLMonitor &&other) noexcept
        {
            if (this != &other)
            {
                owned_data = std::move(other.owned_data);
                param_map = std::move(other.param_map);
                semantics = other.semantics;
                rob = other.rob;
                lower_rob = other.lower_rob;
                upper_rob = other.upper_rob;
                up_to_date = other.up_to_date;
                start_time = other.start_time;
                end_time = other.end_time;
                if (formula)
                {
                    delete formula;
                }
                formula = other.formula;
                other.formula = nullptr;

                if (other.data == &other.owned_data)
                {
                    data = &owned_data;
                } 
                else
                {
                    data = other.data;
                }
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

        string get_signals_names() const;

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
            else if (sem == "BOOLEAN")
            {
                semantics = Semantics::BOOLEAN;
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
            case Semantics::BOOLEAN:
                return "BOOLEAN";
            default:
                throw std::invalid_argument("Invalid semantics value");
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
                    up_to_date = false;
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

        inline void set_formula(transducer* formula)
        {
            this->formula = formula;
            rob = 0.0; lower_rob = 0.0; upper_rob = 0.0;
            set_eval_time(0., 0.);
            up_to_date = false;
        }

        inline void reset_signal_data()
        {
            data->reset_signal_data();
            up_to_date = false;
        };

        inline double get_lower_rob() { return lower_rob; };
        inline double get_upper_rob() { return upper_rob; };
        // inline double get_rob() { return rob; };

        // append new sample to data
        void add_sample(vector<double> s);
        void add_sample(vector<double> s, bool interp);

        // set signals data 
        void set_signals(const std::vector<Signal>& signals);

        /** load signals from csv file */
        void load_csv(const vector<string>& files);

        /** write signals to csv file */
        void write_csv(const std::string& directory) const;


        inline void set_eval_time(double t_start, double t_end)
        {
            start_time = t_start;
            end_time = t_end;
            up_to_date = false;
        }


        double get_rob();
        double get_rob(double);

        vector<double> get_online_rob();
        vector<double> get_online_rob(double);

        Signal get_rob_signal();
        Signal get_rob_signal(double);
        Signal get_rob_signal(double, double);

        vector<Signal> get_online_rob_signal();
        vector<Signal> get_online_rob_signal(double);
        vector<Signal> get_online_rob_signal(double, double);

        robustness_map_t get_robustness_map();
        robustness_map_t get_online_robustness_map();


    
        // display stuff
        inline void display_signal_names() const
        {
            data->display_signal_names();
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

        friend ostream &operator<<(ostream &out, const STLMonitor &monitor)
        {
            // TODO harmonize with STLDriver's ?
            out << "STL Monitor Object" << endl;
            
            bool first = true;
            
            if (monitor.data)
            {
                out << "Signal Names: ";
                for (const auto &signal : monitor.data->signal_map)
                {
                    out << signal.first;
                    if (&signal != &(*std::prev(monitor.data->signal_map.end())))
                    {
                        out << ", ";
                    }
                }
            }

            out << endl;

            out << "Parameters: ";
            for (const auto &param : monitor.param_map)
            {
                out << param.first << ": " << param.second;
                if (&param != &(*std::prev(monitor.param_map.end())))
                {
                    out << ", ";
                }
            }
            if (monitor.param_map.size() == 0)
                out << "No parameters set.";
            out << endl;
            
            out << "\nFormula: ";
            if (monitor.formula)
            {
                out << *monitor.formula;
            }
            else
            {
                out << "No formula set.";
            }
            out << endl;

            out << "\nData: ";

            if (monitor.data)
            {
                out << *monitor.data;
            } else {
                out << "No data set." << endl;
            }
            out << endl;

            out << "Robustness on [" << monitor.start_time << "," << monitor.end_time << "]:";
            if (monitor.up_to_date)
                out << endl << "at t=" << monitor.start_time << ":    lower_rob=" << monitor.lower_rob << "   <=    estimate=" << monitor.rob << "   <=    upper_rob= " << monitor.upper_rob <<  endl; 
            else
                out << " not up to date.";
            
            return out;
        }
    };
}
#endif