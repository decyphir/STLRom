#include <stdio.h>
#include "stl_driver.h"
#include "tools.h"
#include <string>
#include <vector>

using namespace std;
using namespace STLRom;

int main(int argc, char **argv)
{

    /*  Read formula(s).

        Formulas can be defined in a file or a string. At minima there needs to be:
        - a signal(s) declaration
        - one formula assignement, e.g.:

        signal x,y
        phi := alw_[0, 5.] (x[t]>0 and y[t]<2)

    */

    int num_samples = 10; // default value
    int num_traces = 10;
    int num_repeat = 10;
    for (int i = 1; i < argc; ++i)
    {
        if (string(argv[i]) == "-n" && i + 1 < argc)
        {
            num_samples = stoi(argv[++i]);
        }
        if (string(argv[i]) == "-N" && i + 1 < argc)
        {
            num_traces = stoi(argv[++i]);
        }
        if (string(argv[i]) == "-r" && i + 1 < argc)
        {
            num_repeat = stoi(argv[++i]);
        }
    }
    cout << "Number of samples: " << num_samples << endl;
    cout << "Number of traces: " << num_traces << endl;
    cout << "Number of repeat: " << num_repeat << endl;

    for (int i_trace = 0; i_trace < num_traces; i_trace++)
    {
        STLDriver stl_driver = STLDriver();
        string s = "../test/stress.stl";
        bool parse_success = stl_driver.parse_file(s);

        if (!parse_success)
        {
            cout << "Something went wrong parsing formula. Maybe wrong file path for " << s << endl;
            return 1;
        }

        trace_data data = rand_trace_data(num_samples, 2);
        stl_driver.data = data;
        for (int i_repeat = 0; i_repeat < num_repeat; i_repeat++)
        {
            cout << "youpi tralalido" << endl
                 << std::flush;
            stl_driver.get_online_rob("phi2");
        }
        cout << endl;
    }

    return 0;
}
