    #include <stdio.h>
#include "stl_driver.h"
#include <string>
#include <vector>

using namespace std;
using namespace STLRom;

int main(int argc, char** argv) {

    STLDriver driver = STLDriver();

    const char * spec = 
    "signal x, y\n"
    "param a = 0, b = 0.5\n"
    "x_sup := x[t] > 0\n"
    "y_sup := y[t] > 0\n"
    "alw_x := alw_[a, b] x_sup\n"
    "ev_x  := ev_ [a, b] x_sup\n"
    "x_until_y := x_sup until_[a, b] y_sup\n"
    "hist_x := hist_[a,b] x_sup\n"
    "once_x := once_[a,b] x_sup\n"
    "x_since_y := x_sup since_[a, b] y_sup\n";

    driver.parse_string(spec);

    driver.load_csv({"../test/test_data/x.csv", "../test/test_data/y.csv"});

    const vector<string> phis_past = {
        "alw_x", 
        "ev_x", 
        "hist_x", 
        "once_x",
        "x_until_y",
        "x_since_y"};

    const vector<pair<double, double>> params = {
        {0.0, 0.5},
        {0.3, 0.8}
    };

    const vector<pair<double, double>> rob_windows = {
        {0.0, 4.5},
        {0.0, 5.0},
        {0.8, 4.5},
        {0.8, 5.0}
    };

    for (const auto& phi : phis_past) {
        auto monitor = driver.get_monitor(phi);

        for (const auto& interv : params) {
            double a = interv.first;
            double b = interv.second;
            monitor.set_param("a", a);
            monitor.set_param("b", b);

            for (const auto& window : rob_windows) {
                double t_begin = window.first;
                double t_end = window.second;
                cout << *monitor.formula << "[" << a << "," << b << "] " << t_begin << " " << t_end << endl;

                Signal rob = monitor.get_rob_signal(t_begin, t_end);
            }
        }
    }
}
