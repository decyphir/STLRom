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
    return 0;

    static volatile double fmin = 1e-307;
    Signal low_epsilon = Signal(0., fmin, 1);
    low_epsilon.appendSample(3., 1.);
    Signal up_epsilon = Signal(0., fmin, 1);
    up_epsilon.appendSample(3., 1.);
    Tube eps_tube = Tube(low_epsilon, up_epsilon);

    Tube result = tube + eps_tube;
    cout << result.lower_signal << endl;
    cout << result.upper_signal << endl;
    
    return int(result.lower_signal == result.upper_signal);
}
