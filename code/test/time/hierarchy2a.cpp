//
//  Mach7: Pattern Matching Library for C++
//
//  Copyright 2011-2013, Texas A&M University.
//  Copyright 2014 Yuriy Solodkyy.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//
//      * Neither the names of Mach7 project nor the names of its contributors
//        may be used to endorse or promote products derived from this software
//        without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

///
/// \file
///
/// This file is a part of Mach7 library test suite.
///
/// \author Yuriy Solodkyy <yuriy.solodkyy@gmail.com>
///
/// \see https://parasol.tamu.edu/mach7/
/// \see https://github.com/solodon4/Mach7
/// \see https://github.com/solodon4/SELL
///

#include "testshape.hpp"
#include "testutils.hpp"
#include "match.hpp"

//------------------------------------------------------------------------------

#if !defined(NUMBER_OF_BASES)
  #define NUMBER_OF_BASES  5
#endif
#if NUMBER_OF_BASES > NUMBER_OF_DERIVED
  #error Visitor Forwarding only makes sense when number of base classes is smaller than the number of all derived classes.
#endif


//------------------------------------------------------------------------------

template <size_t N>
struct shape_kind;

#define FOR_EACH_MAX NUMBER_OF_BASES-1
#define FOR_EACH_N(N) template <> struct shape_kind<N> : OtherBase, Shape { typedef Shape base_type; shape_kind(size_t kind = N) : Shape(kind) { this->m_all_kinds = (const size_t*)mch::get_kinds<Shape>(mch::original2remapped<Shape>(mch::tag_type(kind))); } void accept(ShapeVisitor&) const; };
#include "loop_over_numbers.hpp"
#undef  FOR_EACH_N
#undef  FOR_EACH_MAX

//------------------------------------------------------------------------------

template <size_t N>
struct shape_kind : shape_kind<N % NUMBER_OF_BASES>
{
    typedef shape_kind<N % NUMBER_OF_BASES> base_type;
    shape_kind() : base_type(N) { this->m_all_kinds = (const size_t*)mch::get_kinds<Shape>(mch::original2remapped<Shape>(mch::tag_type(N))); }
    void accept(ShapeVisitor&) const;
};

//------------------------------------------------------------------------------

struct ShapeVisitor
{
    virtual void visit(const Shape&) {}
    #define FOR_EACH_MAX NUMBER_OF_DERIVED-1
    #define FOR_EACH_N(N) virtual void visit(const shape_kind<N>& s) { visit(static_cast<const shape_kind<N>::base_type&>(s)); }
    #include "loop_over_numbers.hpp"
    #undef  FOR_EACH_N
    #undef  FOR_EACH_MAX
};

//------------------------------------------------------------------------------

#define FOR_EACH_MAX NUMBER_OF_BASES-1
#define FOR_EACH_N(N) void shape_kind<N>::accept(ShapeVisitor& v) const { v.visit(*this); }
#include "loop_over_numbers.hpp"
#undef  FOR_EACH_N
#undef  FOR_EACH_MAX

//------------------------------------------------------------------------------

template <size_t N> void shape_kind<N>::accept(ShapeVisitor& v) const { v.visit(*this); }

SKV(Shape,0); // Declare the smallest kind value for Shape hierarchy

namespace mch ///< Mach7 library namespace
{
template <>         struct bindings<Shape>         { KS(Shape::m_kind); KV(Shape,NUMBER_OF_DERIVED+1); };

// NOTE: We need to explicitly instantiate all bindings as otherwise our kinds 
//       associations don't get instantiated and recorded. This happens because
//       now that bindings with specific N > base cases is never instantiated
// FIX:  Wonder though why it didn't instantiate them for base cases at least since
//       we instantiate them explicitly in Case statements
#define FOR_EACH_MAX NUMBER_OF_BASES-1
#define FOR_EACH_N(N) template <> struct bindings<shape_kind<N>> { KV(Shape,N);  BCS(shape_kind<N>,Shape); CM(0,shape_kind<N>::m_member0); CM(1,shape_kind<N>::m_member1); };
#include "loop_over_numbers.hpp"
#undef  FOR_EACH_N
#undef  FOR_EACH_MAX

// NOTE: We need to explicitly instantiate all bindings as otherwise our kinds 
//       associations don't get instantiated and recorded. This happens because
//       now that bindings with specific N > base cases is never instantiated
#define FOR_EACH_MAX NUMBER_OF_DERIVED-NUMBER_OF_BASES-1
#define FOR_EACH_N(N) template <> struct bindings<shape_kind<NUMBER_OF_BASES+N>> { KV(Shape,NUMBER_OF_BASES+N);  BCS(shape_kind<NUMBER_OF_BASES+N>,shape_kind<NUMBER_OF_BASES+N>::base_type,Shape); CM(0,shape_kind<NUMBER_OF_BASES+N>::m_member0); CM(1,shape_kind<NUMBER_OF_BASES+N>::m_member1); };
#include "loop_over_numbers.hpp"
#undef  FOR_EACH_N
#undef  FOR_EACH_MAX

} // of namespace mch

//------------------------------------------------------------------------------

#if 1
XTL_TIMED_FUNC_BEGIN
size_t do_match(const Shape& s, size_t)
{
    MatchF(s)
    {
        #define FOR_EACH_MAX  NUMBER_OF_BASES-1
        #define FOR_EACH_N(N) CaseF(shape_kind<N>) return N;
        #include "loop_over_numbers.hpp"
        #undef  FOR_EACH_N
        #undef  FOR_EACH_MAX
    }
    EndMatchF
    return invalid;
}
XTL_TIMED_FUNC_END
#else
#error No preprocessed version provided
#endif

//------------------------------------------------------------------------------

XTL_TIMED_FUNC_BEGIN
size_t do_visit(const Shape& s, size_t)
{
    struct Visitor : ShapeVisitor
    {
        #define FOR_EACH_MAX  NUMBER_OF_BASES-1
        #define FOR_EACH_N(N) virtual void visit(const shape_kind<N>&) { result = N; }
        #include "loop_over_numbers.hpp"
        #undef  FOR_EACH_N
        #undef  FOR_EACH_MAX
        size_t result;
    };

    Visitor v;
    v.result = invalid;
    s.accept(v);
    return v.result;
}
XTL_TIMED_FUNC_END

//------------------------------------------------------------------------------

Shape* make_shape(size_t i)
{
    switch (i % NUMBER_OF_DERIVED)
    {
        #define FOR_EACH_MAX  NUMBER_OF_DERIVED-1
        #define FOR_EACH_N(N) case N: return new shape_kind<N>;
        #include "loop_over_numbers.hpp"
        #undef  FOR_EACH_N
        #undef  FOR_EACH_MAX
    }
    return 0;
}

//------------------------------------------------------------------------------

#include "testvismat1.hpp"    // Utilities for timing tests

//------------------------------------------------------------------------------

int main()
{
    using namespace mch; // Mach7's library namespace

    verdict pp = test_repetitive();
    verdict ps = test_sequential();
    verdict pr = test_randomized();
    std::cout << "OVERALL: "
              << "Repetitive: " << pp << "; "
              << "Sequential: " << ps << "; "
              << "Random: "     << pr 
              << std::endl; 
}

//------------------------------------------------------------------------------
