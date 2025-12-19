#include "transducer.h"

using namespace STLRom;

// Leaf constructor
Transducer::Transducer(const std::string& type): 
    type(type), 
    childL(nullptr),
    childR(nullptr) 
    
{

}

// Constructor with left child only
Transducer::Transducer(const std::string& type, Transducer *left): 
    type(type),
    childL(left),
    childR(nullptr)
    
{

}

// Constructor with both children
Transducer::Transducer(const std::string& type, Transducer *left, Transducer *right):
    type(type),
    childL(left),
    childR(right)
    
{

}

// String representation: (childL, type, childR)
std::string Transducer::toString() const {
    std::string leftStr  = childL ? childL->toString() : "null";
    std::string rightStr = childR ? childR->toString() : "null";

    return "(" + leftStr + ", " + type + ", " + rightStr + ")";
}

// Getters
std::string Transducer::getType() const {
    return type;
}

Transducer *Transducer::getChildL() const {
    return childL;
}

Transducer *Transducer::getChildR() const {
    return childR;
}


// Clone function
Transducer* Transducer::clone() const {
        return new Transducer(
            type,
            childL  ? childL->clone()  : nullptr,
            childR ? childR->clone() : nullptr
        );
}