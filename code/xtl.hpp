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
/// A minimal version of the XTL library definitions required to support subtyping.
///
/// \author Yuriy Solodkyy <yuriy.solodkyy@gmail.com>
///
/// \see https://parasol.tamu.edu/xtl/
/// \see https://parasol.tamu.edu/mach7/
/// \see https://github.com/solodon4/Mach7
/// \see https://github.com/solodon4/SELL
///

#pragma once

#include <type_traits>

namespace xtl
{
    template <class T, class U, class C = void> struct is_subtype : std::is_same<T,U> {};

    template <class T, class S>
    struct is_subtype<S*,T*> : is_subtype<S,T> {};

    // Derived class is a subtype of a base class
    template <class D, class B>
    struct is_subtype <D, B, typename std::enable_if<std::is_base_of<B,D>::value>::type> : std::true_type {};

    //==============================================================================
    // target class definition
    //==============================================================================

    /// Helper qualifier to overcome problems with overload resolution when only
    /// several argument types are deduced and others are explicitly specified.
    /// This class will be used to wrap explicitly given arguments. Overloaded
    /// subtype_cast_impl have to use this qualifier to match target type T that was
    /// specified by the user in subtype_cast<T>(val).
    template <class T>
    struct target
    {
        /// Type that actually has to be returned by a subtype_cast. Most of the
        /// time it will be the same as target type T, however in some cases it may
        /// be different. An example of such is the result type of casting array
        /// array types, which is T* instead of T[N].By default, result type matches T.
        typedef T type;
    };

    //==============================================================================
    // Forward declarations
    //==============================================================================

    template <class T, class S>
    typename std::enable_if<is_subtype<S, T>::value, typename target<T>::type>::type
    subtype_cast(const S& s);

    //==============================================================================
    // subtype_cast_impl definition
    //==============================================================================

    /// Helper function to overcome problems with overload resolution when only
    /// several arguments types are deduced and others are explicitly specified.
    /// This function has all of the types in deducible context, so that explicitly
    /// given arguments will be passed through dummy null-pointers.
    /// Default implementation just tries to cast one type to another.
    template <class T, class S>
    T subtype_cast_impl(target<T> t, const S& s)
    {
        return (T)s;
    }

    //==============================================================================
    // subtype_cast definition
    //==============================================================================

    /// Routine to cast sub-type T to its super-type S.
    /// Extra level of indirection with impl was made to overcome the problem that
    /// when S is not among the arguments generic and more specific specialization
    /// are the same for compiler and it reports ambiguity.
    template <class T, class S>
    typename std::enable_if<is_subtype<S, T>::value, typename target<T>::type>::type
    subtype_cast(const S& s)
    {
        // We create a dedicated variable with result because some classes may provide
        // copy constructors that don't bind to temporaries (e.g. auto_ptr<T>).
        typename target<T>::type result(subtype_cast_impl(target<T>(), s));
        return result;
    }

    //==============================================================================
    // subtype_dynamic_cast definition
    //==============================================================================

    template <class D, class B>
    typename std::enable_if<std::is_base_of<B, D>::value, D*>::type
    subtype_dynamic_cast_impl(target<D*>, B* p)
    {
        return dynamic_cast<D*>(p);
    }

    template <class S, class T>
    //typename std::enable_if<is_subtype<S, T>::value, S*>::type
    S subtype_dynamic_cast(T* t)
    {
        typename target<S>::type result = subtype_dynamic_cast_impl(target<S>(), t);
        return result;
    }

} // of namespace xtl
