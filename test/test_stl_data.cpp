    #include <stdio.h>
#include "stl_data.h"
#include <string>
#include <vector>

using namespace std;
using namespace STLRom;

int main(int argc, char** argv) {

    
    STLData data_empty = STLData();

    cout << "Empty constructor:" << endl;
    cout << data_empty << endl << endl;

    STLData data = STLData(3);

    cout << "Initializing with 3 empty signals:" << endl;
    cout << data << endl << endl;

    data.add_sample({0., 0., 2., 3.});
    data.add_sample({1., 10., -2., 4.});
    data.add_sample({2.5, 0., 3., 8.});
    data.add_sample({3., 1., 2., 0.});

    cout << data << endl << endl;

    cout << "Testing get_signal by name." << endl;
    Signal sig_x1 = data.get_signal("x1");
    cout << sig_x1 << endl;
    cout << endl;

    cout << "Testing get_signal by idx." << endl;
    Signal sig_0 = data.get_signal(0);
    cout << sig_0 << endl;


    cout << "Resetting then printing." << endl;

    data.reset_signal_data();
    cout << data << endl << endl;

    cout << "Testing display_signal_names():" << endl;
    data.display_signal_names();
    cout << endl;

    return 0;

}
