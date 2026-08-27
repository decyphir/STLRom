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
        for (auto &tube : tube_vector)
            {
                tube.clear();
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
    
    void STLData::set_tube_vector(const tube_data &tube_vector)
    {
        if (tube_vector.size() != signal_map.size()) {
			throw std::invalid_argument("Number of signals does not match the number of declared signals.");
		}
        this->tube_vector = tube_vector;
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
        int sig_idx = get_signal_idx(sig);
        if (sig_idx != -1)
            data_vector[sig_idx].appendSample(t,v,d,interp);   
    }

    int STLData::get_signal_idx(const string &sig) const
    {
        auto it = signal_map.find(sig);
        if (it == signal_map.end())
        {
            cout << "Signal " << sig << " not found in signal_map." << endl;
            return -1;
        }

        return it->second;
    }

    string STLData::get_signame_from_idx(int idx) const
    {
        if (idx >= 0 && idx < get_size())
        {
            for (const auto &it : signal_map)
            {
                if (it.second == idx)
                    return it.first;
            }
            // Shouldn't happen if signal_map and data_vector are kept in sync
            cout << "Signal index " << idx << " has no matching name in signal_map." << endl;
            return "";
        } else {
            cout << "Signal index " << idx << " out of bounds for " << get_size() << " defined signals." << endl;
            return "";
        }
    }

    Signal STLData::get_signal(int idx) const
    {
        if (idx >= 0 && idx < get_size())
            return data_vector[idx];
        else
            cout << "Signal index " << idx << " out of bounds for " << get_size() << " defined signals." << endl;
        
        Signal signal;
        return signal;
    }

    Signal STLData::get_signal(const string &sig) const
    {
        int idx = get_signal_idx(sig);
        if (idx != -1)
            return get_signal(idx);

        Signal signal;
        return signal;
    }

    Tube STLData::get_tube(int idx) const
    {
        if (idx >= 0 && idx < get_size() && !tube_vector.empty())
            return tube_vector[idx];
        else
            cout << "Tube index " << idx << " out of bounds for " << get_size() << " defined tubes (or tube_vector is empty)." << endl;
        
        Tube tube;
        return tube;
    }

    Tube STLData::get_tube(const string &tub) const
    {
        int idx = get_tube_idx(tub);
        if (idx != -1)
            return get_tube(idx);

        Tube tube;
        return tube;
    }

    int STLData::get_tube_idx(const string &tub) const
    {
        auto it = signal_map.find(tub);
        if (it == signal_map.end())
        {
            cout << "Tube " << tub << " not found in signal_map." << endl;
            return -1;
        }

        return it->second;
    }

    string STLData::get_tubename_from_idx(int idx) const
    {
        return get_signame_from_idx(idx);
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