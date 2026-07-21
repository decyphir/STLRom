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
            explicit STLData(int nb_signals) : data_vector(nb_signals) {}

            // constructor with vector of signals
            explicit STLData(trace_data data_vector) : data_vector(std::move(data_vector)) {}

            // other necessary constructors
            // Copy constructor
            STLData(const STLData &other)
                : data_vector(other.data_vector)
            {
            }

            // Copy assignment operator
            STLData &operator=(const STLData &other)
            {
                if (this != &other)
                {
                    data_vector = other.data_vector;
                }
                return *this;
            }

            // Move constructor
            STLData(STLData &&other) noexcept
                : data_vector(std::move(other.data_vector))
            {
            }

            // Move assignment operator
            STLData &operator=(STLData &&other) noexcept
            {
                if (this != &other)
                {
                    data_vector = std::move(other.data_vector);
                }
                return *this;
            }

            // add sample
            void add_sample(vector <double> sample);

            void add_sample(vector <double> sample, bool interp);
            // add constant sample


            // get size (nb of signals)
            int get_size() const;

            // reset signal data
            void reset_signal_data();

            // check if ALL signals are empty
            bool is_empty() const;

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
            }
    };
}

#endif