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


int read_sample(vector<double> &sample) {
    sample.clear();
    string line; 
    getline(cin, line);
    istringstream reader(line);
    while (!reader.eof()) {
        double val;
        reader >> val;
        sample.push_back(val);
    }
    return 0.;
}


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
    bool parse_success = stl_driver.parse_file("spec.stl"); // parse_string is possible too, instead of file
    
    if (parse_success) {
        cout << "Formula parsed successfully." << endl;
            }
    else {
        cout << "Something went wrong." <<endl;
        return 1; 
    }  

    // Get a monitor on formula phi. Any formula assigned with := is available
    STLMonitor phi = stl_driver.get_monitor("phi");     
    cout << *phi.formula << endl;  // displays formula of the monitor

    string signames = stl_driver.get_signals_names(); // get signal names involved in the formula
    cout << "Enter values for " << endl;
    cout << "time " << signames << endl;
    
    // Read data
    
    while (1) {
        vector<double> sample;    
        read_sample(sample); // reads data - first                
        phi.add_sample(sample);
        
        // Computes upper and lower robustness     	
        double rob_up = phi.get_upper_rob();
        double rob_low= phi.get_lower_rob();
        cout << "time: " << phi.current_time << " Interval robustness: [" << rob_low << ", " << rob_up << "]" << endl;        
    }
   

    return 0;

}

