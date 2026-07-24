#ifndef STL_DATA_H
#define STL_DATA_H

#include "transducer.h"

namespace STLRom
{
    /** A class for STL data */
    class STLData
    {
        public:
            trace_data data_vector; // data_vector is a vector of signals
            map<string, int> signal_map; 

            STLData() = default;

            ~STLData() = default;

            // constructor with number of signals
            explicit STLData(int nb_signals) : data_vector(nb_signals) {
                for (int i = 0; i < nb_signals; i++) {
                    signal_map["x" + to_string(i)] = i;
                }
            }

            // constructor with vector of signals
            explicit STLData(trace_data data_vector) : data_vector(std::move(data_vector)) {
                for (int i = 0; i < data_vector.size(); i++) {
                    signal_map["x" + to_string(i)] = i;
                }
            }

            // constructor with vector of signals and signal map
            explicit STLData(trace_data data_vector, map<string, int> signal_map) : 
            data_vector(std::move(data_vector)),
            signal_map(std::move(signal_map)) {}

            // other necessary constructors
            // Copy constructor
            STLData(const STLData &other)
                : data_vector(other.data_vector), signal_map(other.signal_map)
            {
            }

            // Copy assignment operator
            STLData &operator=(const STLData &other)
            {
                if (this != &other)
                {
                    data_vector = other.data_vector;
                    signal_map = other.signal_map;
                }
                return *this;
            }

            // Move constructor
            STLData(STLData &&other) noexcept
                : data_vector(std::move(other.data_vector)), signal_map(std::move(other.signal_map))
            {
            }

            // Move assignment operator
            STLData &operator=(STLData &&other) noexcept
            {
                if (this != &other)
                {
                    data_vector = std::move(other.data_vector);
                    signal_map = std::move(other.signal_map);
                }
                return *this;
            }

            /** append new sample to data, default linear interpolation */
            void add_sample(vector <double> sample);

            /** append new sample to data */
            void add_sample(vector <double> sample, bool interp);

            /** append new sample to signal, default linear interpolation */
            void add_signal_sample(string phi, double t, double v);

            /** append new sample to signal, default linear interpolation */
            void add_signal_sample(string phi, double t, double v, double d);

            /** append new sample to signal */
            void add_signal_sample(string phi, double t, double v, double d, bool interp);

            /** get signal index */
            int get_signal_idx(const string &sig) const;

            /** get signal by index */
            Signal get_signal(int idx) const;

            /** get signal by name */
            Signal get_signal(const string &sig) const;

            // get size (nb of signals)
            int get_size() const;

            // reset signal data
            void reset_signal_data();

            // check if ALL signals are empty
            bool is_empty() const;

            void set_data_vector(const trace_data &data_vector);

            inline void display_signal_names() const
            {
                for (const auto &signal : signal_map)
                {
                    cout << signal.first << ": " << signal.second << endl;
                }
            }

            friend ostream &operator<<(ostream &out, const STLData &data)
            {
                if (data.is_empty())
                {
                    out << "No data yet.";                  
                }
                else
                {
                    for (const auto &signal : data.signal_map)
                    {
                        out << "# Signal " << signal.first << ":"<< endl;
                        if (data.data_vector[signal.second].empty())
                        {
                            out << "No data yet." << endl;
                        }
                        else
                        {
                            out << data.data_vector[signal.second].size() << " samples from t0=" << data.data_vector[signal.second].beginTime << " to t_end=" << data.data_vector[signal.second].endTime << endl;
                        }
                    }
                } 
                return out; 
            }
    };
}

#endif