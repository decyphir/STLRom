#include <stdio.h>
#include "stl_driver.h"
#include <string>
#include <vector>

using namespace std;
using namespace STLRom;

int main(int argc, char** argv) {

    /*  Fuzzy.

    */

    // STLDriver is the class implementing the parser.            
    
    STLDriver stl_driver = STLDriver();
    stl_driver.set_semantics("FUZZY");
    string s="../test/spec.stl";
    bool parse_success = stl_driver.parse_file(s);
    
    if (parse_success) {
        cout << "Formula parsed successfully" << endl;    
        stl_driver.print();                    
        }
    else {
        cout << "Something went wrong." <<endl;
        return 1; 
    }  

    // Testing one sample
    vector<double> v ;
    v = {0,-2, 4.43};
    stl_driver.add_sample(v);    
    v = {2.5, 0, 0};        
    stl_driver.add_sample(v);
    v = {5, 5, -1.3};        
    stl_driver.add_sample(v);
    
    vector<double> robs;
    
    robs = stl_driver.get_online_rob("phi_until2");
    cout << robs[0] << " " << robs[1] << " " << robs[2] << endl;
    
    return 0;

}

