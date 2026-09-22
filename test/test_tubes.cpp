#include "stl_data.h"
#include "stl_monitor.h"
#include "tools.h"
#include "transducer.h"
#include "stl_driver.h"
#include "signal.h"

using namespace std;
using namespace STLRom;

int main(int argc, char **argv)
{
    Signal sig = Signal(0., 1., 1);
    sig.appendSample(2., 2.);
    sig.appendSample(3., -2.);
    
    cout << sig << endl;
    sig.inflate(0.2);
    cout << sig << endl;

    sig.inflate(0.); // lower = x = upper

    static volatile double fmin = 1e-307;
    Signal epsilon = Signal(0., fmin, 1);
    epsilon.appendSample(3., 1.);

    Signal result = sig + epsilon;
    cout << result << endl;

    if (result.lower_signal == result.upper_signal)
        return 1;

    // Other test file?
    STLData data = STLData({sig});
    STLDriver stl_driver;
    stl_driver.data = data;
    cout << stl_driver << endl;
    cout << stl_driver.data.data_vector.back() << endl;

    //

    string s ="signal x0\n";
    s+="mu := x0[t]>2\n";
    s+="phi:= alw_[0, 2] (x0[t]>0)";
    bool parse_success = stl_driver.parse_string(s);
    if (parse_success) {
        cout << "Formula parsed successfully" << endl;    
        }
    else {
        cout << "Something went wrong." <<endl;
        return 1; 
    }  
    // get_monitor
    auto mu = stl_driver.get_monitor("mu");
    auto phi = stl_driver.get_monitor("phi");
    phi.set_eval_time(0., 0.);
    
    Signal rho_phi = phi.get_rob_signal(); // Compute robustness only on the signal and not the tube? :(
    cout << "rho(phi): " << rho_phi << endl;


    vector<Signal> online_rho_phi = phi.get_online_rob_signal();
    cout << "online rho(phi): " << rho_phi[0] << endl;
    cout << "online rho(phi): " << rho_phi[1] << endl;
    cout << "online rho(phi): " << rho_phi[2] << endl; // upper should be > z ? 

    return 0;
}
