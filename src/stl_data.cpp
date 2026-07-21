#include "stl_data.h"

namespace STLRom
{
    int STLData::get_size() const
    {
        return data_vector.size();
    }

    void STLData::reset_signal_data()
    {
        for (auto &signal : data_vector)
            {
                signal.clear();
            }
    }

    bool STLData::is_empty() const
    {
        return std::all_of(data_vector.begin(), data_vector.end(),
            [](const Signal& s) { return s.empty(); });
    }

    void STLData::set_data_vector(const trace_data &data_vector)
    {
        this->data_vector = data_vector;
    }

    void STLData::add_sample(vector <double> sample, bool interp)
    {
        if (sample.size() != get_size() + 1) 
	    {
        	throw std::invalid_argument("Sample size does not match the number of signals.");
    	}

        double t = sample[0];
        for (int i = 1; i < sample.size(); i++) 
        {
            data_vector[i - 1].appendSample(t, sample[i], 0., interp);
        }

        // TODO: uptodate?
    }

    void STLData::add_sample(vector <double> sample)
    {
        add_sample(sample, true);
    }

    


}