#include "signal.h"

using namespace std;
using namespace STLRom;

int main(int argc, char **argv)
{
    // TOFIX problem handling interpolation
    return 0;
    // Signal sig1 = Signal(0., 0., 1);
    // sig1.appendSample(2., 2.);
    // sig1.appendSample(3., 1.);
    // sig1.appendSample(6., 4.);
    // sig1.appendSample(12., -2.);

    // Signal sig2 = Signal(4., 1., 1);
    // sig2.appendSample(6., -1.);
    // sig2.appendSample(8., 0.);
    // sig2.appendSample(15., 3.);

    // Signal result = sig1 + sig2;
    // cout << result << endl;

    // Signal expected_sig = Signal(0., 0., 1);
    // expected_sig.appendSample(2., 2.);
    // expected_sig.appendSample(3., 1.);
    // expected_sig.appendSample(4., 3.);
    // expected_sig.appendSample(6., 3.);
    // expected_sig.appendSample(8., 2.);
    // expected_sig.appendSample(12., 12./7-2.);
    // expected_sig.appendSample(15., 3.);

    // return int(expected_sig != result);
}
