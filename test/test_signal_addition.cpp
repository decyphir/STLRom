#include "signal.h"

using namespace std;
using namespace STLRom;

int main(int argc, char **argv)
{
    Signal sig1, sig2;
    Signal result, expected_sig;

    // Test 1 - Simple
    sig1 = Signal(0., 0., 1);
    sig1.appendLinearSample(2., 2.);
    sig1.appendLinearSample(4., 0.);

    sig2 = Signal(0., 1., 1);
    sig2.appendConstantSample(4., 1.);
    
    expected_sig = Signal();
    expected_sig.appendLinearSample(0., 1.);
    expected_sig.appendLinearSample(2., 3.);
    expected_sig.appendLinearSample(4., 1.);
    
    result = sig1 + sig2;
    // cout << expected_sig << endl;
    // cout << result << endl;
    if (expected_sig != result) {
        return 1;
    }

    // Test 2 - Piecewise linear
    sig1 = Signal(0., 0., 1);
    sig1.appendLinearSample(2., 2.);
    sig1.appendLinearSample(3., 1.);
    sig1.appendLinearSample(6., 4.);
    sig1.appendLinearSample(12., -2.);

    sig2 = Signal();
    sig2.appendSample(0., 1., -.5);
    sig2.appendConstantSample(4., 1.);
    sig2.appendConstantSample(9., 1.);
    sig2.appendLinearSample(12., -2.);
    
    expected_sig = Signal();
    expected_sig.appendLinearSample(0., 1.);
    expected_sig.appendLinearSample(2., 2.);
    expected_sig.appendLinearSample(3., .5, .5);
    expected_sig.appendConstantSample(4., 3.);
    expected_sig.appendLinearSample(6., 5.);
    expected_sig.appendLinearSample(9., 2.);
    expected_sig.appendLinearSample(12., -4.);

    result = sig1 + sig2;
    // cout << expected_sig << endl;
    // cout << result << endl;
    if (expected_sig != result) {
        return 1;
    }


    // Test 3 - Simplify
    sig1 = Signal(0., 0., 1);
    sig1.appendLinearSample(2., 2.);
    sig1.appendLinearSample(4., 6.);

    sig2 = Signal(0., 1., 1);
    sig2.appendLinearSample(2., 1.);
    sig2.appendLinearSample(4., -1.);
    
    expected_sig = Signal();
    expected_sig.appendLinearSample(0., 1.);
    expected_sig.appendLinearSample(4., 5.);

    result = sig1 + sig2;
    // cout << expected_sig << endl;
    // cout << result << endl;
    if (expected_sig != result) {
        return 1;
    }

    return 0;
}
