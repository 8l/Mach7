///
/// \file
///
/// This file is a part of pattern matching testing suite.
///
/// \autor Yuriy Solodkyy <yuriy.solodkyy@gmail.com>
///
/// This file is a part of the XTL framework (http://parasol.tamu.edu/xtl/).
/// Copyright (C) 2005-2012 Texas A&M University.
/// All rights reserved.
///

#include "match.hpp"                // Support for Match statement
#include "patterns/constructor.hpp" // Support for constructor patterns
//#define XTL_TIMING_METHOD_2 1
#include "timing.hpp"

#include <iostream>

const size_t T = 10000000;

int main()
{
    int tests[]   = {2,3,5,7,11,13,17,19,23,29,31};
    const size_t N = XTL_ARR_SIZE(tests);
    int r = 0;
    size_t u = 0;

    mch::wildcard _;
    int v = 0;

    mch::time_stamp liStart1 = mch::get_time_stamp();

    for (size_t j = 0; j < T; ++j)
        for (size_t i = 0; i < N; ++i)
        {
            if (mch::C<int>(17)(tests[i])) r+=1;
            if (mch::C<int>( v)(tests[i])) r+=2;
            if (mch::C<int>( _)(tests[i])) r+=4;
            u += r;
            //std::cout << "r=" << r << " u=" << u << std::endl;
        }

    mch::time_stamp liFinish1 = mch::get_time_stamp();
    std::cout << "r=" << r << " u=" << u << " timing=" << mch::cycles((liFinish1-liStart1)/T) << " cycles/iteration" << std::endl;
}
