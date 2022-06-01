#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include "tools.h"
#include "transducer.h"
#include "signal.h"
#include "stl_driver.h"

using namespace std;
using namespace RobonTL;



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
    string s ="signal x";
    bool parse_success = stl_driver.parse_file("spec.stl"); // parse_string is possible too, instead of file
    
    if (parse_success) {
        cout << "Formula parsed successfully." << endl;
            }
    else {
        cout << "Something went wrong." <<endl;
        return 1; 
    }  

    return 0;

}

