#include <stdio.h>
#include "stl_driver.h"
#include <string>
#include <vector>

using namespace std;
using namespace STLRom;

int main(int argc, char** argv) {
    // TODO complete with other destructors?

    STLDriver stl_driver = STLDriver();
    string s ="signal x\n";
    s+="interval i = [0,1] \n";
    s+="mu_in := x[t] in [1,2] \n";
    s+="phi := alw_[i] mu_in \n";
 
    bool parse_success = stl_driver.parse_string(s);
    
    if (parse_success) {
        cout << "Formula parsed successfully" << endl;    
        stl_driver.print();                    
        }
    else {
        cout << "Something went wrong." <<endl;
        return 1; 
    }
    stl_driver = STLDriver(); // call the destructors

    stl_driver.print();
    return 0;

}
