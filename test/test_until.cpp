#include <stdio.h>
#include "stl_driver.h"
#include <string>
#include <vector>

using namespace std;
using namespace STLRom;

int main(int argc, char** argv) {

    /*  Read formula(s). 

        Formulas can be defined in a file or a string. At minima there needs to be: 
        - a signal(s) declaration  
        - one formula assignement, e.g.:

        signal x,y
        phi := alw_[0, 5.] (x[t]>0 and y[t]<2)

    */

    // STLDriver is the class implementing the parser.            
    
    STLDriver stl_driver = STLDriver();	  
    string s="spec.stl";
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
    
    vector<double> robs, robs2;
    
    robs = stl_driver.get_online_rob("phi_until2");
    cout << robs[0] << " " << robs[1] << " " << robs[2] << endl;
    
    return 0;

}

