#include "stl_data.h"
#include "stl_monitor.h"
#include "tools.h"
#include "transducer.h"
#include "stl_driver.h"
#include "tube.h"

using namespace std;
using namespace STLRom;

int main(int argc, char **argv)
{
    Signal low_sig = Signal(0., 1., 1);
    low_sig.appendSample(2., 2.);
    low_sig.appendSample(3., -2.);
    Signal up_sig = Signal(0., 1., 1);
    up_sig.appendSample(2., 2.5);
    up_sig.appendSample(3., -1.);

    Tube tubetest = Tube();
    cout << tubetest << endl;

    Tube tube = Tube(low_sig, up_sig);
    cout << tube << endl;
    tube.inflate(0.2);
    cout << tube << endl;

    static volatile double fmin = 1e-307;
    Signal low_epsilon = Signal(0., -fmin, 1);
    low_epsilon.appendSample(3., -fmin);
    Signal up_epsilon = Signal(0., fmin, 1);
    up_epsilon.appendSample(3., fmin);
    Tube eps_tube = Tube(low_epsilon, up_epsilon);

    Signal low_sig2 = Signal(0., 1., 1);
    low_sig2.appendSample(3., 1.);
    Signal up_sig2 = Signal(0., 1., 1);
    up_sig2.appendSample(3., 1.);
    Tube tube2 = Tube(low_sig2, up_sig2);

    Tube result = tube2 + eps_tube;
    cout << result << endl;

    // Other test file?
    STLData data = STLData({tube});
    STLDriver stl_driver;
    stl_driver.data = data;
    cout << stl_driver << endl;
    cout << stl_driver.data.tube_vector.back() << endl;
    cout << stl_driver.data.data_vector.back() << endl;


    return int(result.lower_signal == result.upper_signal);
}
