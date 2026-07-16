#ifndef STL_DATA_H
#define STL_DATA_H

#include "transducer.h"

namespace STLRom
{
    /** A class for STL data */
    class STLData
    {
        public:
            trace_data data;

            STLData() = default;

            ~STLData() = default;
    };
}

#endif STL_DATA_H