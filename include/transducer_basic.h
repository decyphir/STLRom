#ifndef TRANSDUCER_H
#define TRANSDUCER_H

#include <string>
#include <memory>

namespace STLRom {

class Transducer {
private:
    std::string type;
    Transducer* childL;
    Transducer* childR;

public:
    // Constructor for leaf node
    Transducer(const std::string& type);

    // Constructor with left child
    Transducer(const std::string& type, Transducer* left);

    // Constructor with both children
    Transducer(const std::string& type, Transducer* left, 
               Transducer* right);

    // String representation
    std::string toString() const;

    // Clone function
    Transducer* clone() const;

    // Getters
    std::string getType() const;
    Transducer* getChildL() const;
    Transducer* getChildR() const;
};

}

#endif // TRANSDUCER_H