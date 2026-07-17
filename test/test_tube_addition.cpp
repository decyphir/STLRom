#include "tube.h"

using namespace std;
using namespace STLRom;

int main(int argc, char **argv)
{
    Signal sig = Signal(0., 1., 1);
    sig.appendSample(2., 2.);
    sig.appendSample(3., -1.);
    Signal low_sig = Signal(0., 1., 1);
    low_sig.appendSample(2., 2.);
    low_sig.appendSample(3., -1.);
    Signal up_sig = Signal(0., 1., 1);
    up_sig.appendSample(2., 2.);
    up_sig.appendSample(3., -1.);

    Tube tube = Tube(sig, low_sig, up_sig);

    static volatile double fmin = 1e-307;
    Signal epsilon = Signal(0., fmin, 1);
    epsilon.appendSample(3., 1.);
    Signal low_epsilon = Signal(0., fmin, 1);
    low_epsilon.appendSample(3., 1.);
    Signal up_epsilon = Signal(0., fmin, 1);
    up_epsilon.appendSample(3., 1.);
    Tube eps_tube = Tube(epsilon, low_epsilon, up_epsilon);

    Tube result = tube + eps_tube;
    cout << result.signal << endl;
    cout << result.lower_signal << endl;
    cout << result.upper_signal << endl;
    
    return int(result.lower_signal == result.upper_signal);
}
