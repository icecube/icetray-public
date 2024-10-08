#ifndef ICETRAY_SINGLETON_HPP
#define ICETRAY_SINGLETON_HPP

//
//  Adapted from boost 1.36.0 by  Troy D. Straszheim  2008
//  Used in I3_MODULE / I3_SERVICE_FACTORY etc. registration patterns
//

/////////1/////////2///////// 3/////////4/////////5/////////6/////////7/////////8
//  singleton.hpp
//
// Copyright David Abrahams 2006. Original version
//
// Copyright Robert Ramey 2007.  Changes made to permit
// application throughout the serialization library.
//
// SPDX-License-Identifier: BSL-1.0
// Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// The intention here is to define a template which will convert
// any class into a singleton with the following features:
//
// a) initialized before first use.
// b) thread-safe for const access to the class
// c) non-locking
//
// In order to do this,
// a) Initialize dynamically when used.
// b) Require that all singletons be initialized before main
// is called or any entry point into the shared library is invoked.
// This guarantees no race condition for initialization.
// In debug mode, we assert that no non-const functions are called
// after main is invoked.
//

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cassert>
#include <boost/noncopyable.hpp>
#include <serialization/force_include.hpp>

namespace I3 {

  //////////////////////////////////////////////////////////////////////
  // Provides a dynamically-initialized (singleton) instance of T in a
  // way that avoids LNK1179 on vc6.  See http://tinyurl.com/ljdp8 or
  // http://lists.boost.org/Archives/boost/2006/05/105286.php for
  // details.
  //

  // singletons created by this code are guaranteed to be unique
  // within the executable or shared library which creates them.
  // This is sufficient and in fact ideal for the serialization library.
  // The singleton is created when the module is loaded and destroyed
  // when the module is unloaded.

  // This base class has two functions.

  // First it provides a module handle for each singleton indicating
  // the executable or shared library in which it was created. This
  // turns out to be necessary and sufficient to implement the tables
  // used by serialization library.

  // Second, it provides a mechanism to detect when a non-const function
  // is called after initialization.

  // make a singleton to lock/unlock all singletons for alteration.
  // The intent is that all singletons created/used by this code
  // are to be initialized before main is called. A test program
  // can lock all the singletons when main is entereed.  This any
  // attempt to retrieve a mutable instances while locked will
  // generate a assertion if compiled for debug.

  class SingletonModule  : public boost::noncopyable
  {
  private:
    static bool & get_lock(){
      static bool lock = false;
      return lock;
    }
  public:
    //    static const void * get_module_handle(){
    //        return static_cast<const void *>(get_module_handle);
    //    }
    static void lock(){
      get_lock() = true;
    }
    static void unlock(){
      get_lock() = false;
    }
    static bool is_locked() {
      return get_lock();
    }
  };

  namespace detail {

  } // detail


  template <class T>
  class Singleton : public SingletonModule
  {
    class Wrapper : public T
    {
    public:
      static bool m_is_destroyed;
      ~Wrapper(){
        m_is_destroyed = true;
      }
    };


  private:
    I3_DLLEXPORT static T & instance;
    // include this to provoke instantiation at pre-execution time
    static void use(T const &) {}
    I3_DLLEXPORT static T & get_instance() {
      static Wrapper t;
      // refer to instance, causing it to be instantiated (and
      // initialized at startup on working compilers)
      assert (! Wrapper::m_is_destroyed );
      use(instance);
      return t;
    }
  public:
    I3_DLLEXPORT static T & get_mutable_instance(){
      assert(! is_locked());
      return get_instance();
    }
    I3_DLLEXPORT static const T & get_const_instance(){
      return get_instance();
    }
    I3_DLLEXPORT static bool is_destroyed(){
        return Wrapper::m_is_destroyed;
    }
  };

  template<class T>
  I3_DLLEXPORT T & Singleton<T>::instance = Singleton<T>::get_instance();

  template<class T>
  I3_DLLEXPORT bool Singleton<T>::Wrapper::m_is_destroyed = false;

} // namespace I3

#endif // ICETRAY_SINGLETON_HPP
