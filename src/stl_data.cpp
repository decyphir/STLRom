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
        if (data_vector.size() != signal_map.size()) {
			throw std::invalid_argument("Number of signals does not match the number of declared signals.");
		}
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

    void STLData::add_signal_sample(string sig, double t, double v, double d, bool interp)
    {
        if (signal_map.find(sig) == signal_map.end())
        {
            cout << "Signal " << sig << " not found in signal_map." << endl;
            return;
        }
        int sig_idx = signal_map[sig];
        data_vector[sig_idx].appendSample(t,v,d,interp);   
    }

    void STLData::add_signal_sample(string sig, double t, double v)
    {
        add_signal_sample(sig, t,v,0., 1);
    }

    void STLData::add_signal_sample(string sig, double t, double v, double d)
    {
        add_signal_sample(sig, t,v,d, 1);
    }

    


}