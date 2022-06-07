/**
 *  $Id$
 *
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
 *  
 */
#ifndef ICETRAY_I3FACTORY_H_INCLUDED
#define ICETRAY_I3FACTORY_H_INCLUDED
// yesss we all looooove root
#ifndef __CINT__

#include <icetray/Singleton.h>
#include <icetray/I3TrayInfo.h>
#include <icetray/I3Module.h>
#include <icetray/I3Logging.h>

#include <boost/function.hpp>
#include <map>
#include <boost/preprocessor/stringize.hpp>
#include <boost/utility.hpp>

template <class Product, typename FactoryFn>
class I3Factory : boost::noncopyable
{
public:
  typedef FactoryFn factory_fn_t;

  struct product_info_t {
    factory_fn_t fn;
    std::string project;
  };

  typedef std::map<std::string,product_info_t> product_map_t;

  typedef typename product_map_t::const_iterator const_iterator;
  typedef typename product_map_t::iterator iterator;
  typedef typename product_map_t::const_reference const_reference;
  typedef typename product_map_t::reference reference;

  const_iterator begin() const { return product_map.begin(); }
  const_iterator end() const { return product_map.end(); }
  iterator begin() { return product_map.begin(); }
  iterator end() { return product_map.end(); }

  const_iterator find(const std::string& key) const { return product_map.find(key); }
  iterator find(const std::string& key) { return product_map.find(key); }

  void Register (std::string name,
                 std::string project,
                 factory_fn_t fn)
  {
    typename product_map_t::const_iterator iter = product_map.find(name);
    if (iter != product_map.end())
      {
        log_warn("Double-registering %s in I3Factory", name.c_str());
      }
    product_info_t& product = product_map[name];
    product.project = project;
    product.fn = fn;
  }

  FactoryFn Create (std::string name) const
  {
    typename product_map_t::const_iterator iter = product_map.find(name);
    if (iter == product_map.end())
      log_fatal("Module/service \"%s\" not registered with I3_MODULE() or I3_SERVICE_FACTORY()", name.c_str());
    factory_fn_t factory_fn = iter->second.fn;
    return factory_fn;
  }

  // this should technically be private (see friend declaration below)
  // but some gccs get confused.
  I3Factory() { }

private:

  product_map_t product_map;

  friend class I3::Singleton<I3Factory>;
};

typedef boost::function<boost::shared_ptr<I3Module>(const I3Context&)> I3Module_ffn_t;
typedef boost::function<boost::shared_ptr<I3ServiceFactory>(const I3Context&)> I3ServiceFactory_ffn_t;
typedef I3Factory<I3Module,I3Module_ffn_t> I3ModuleFactory;
typedef I3Factory<I3ServiceFactory, I3ServiceFactory_ffn_t> I3ServiceFactoryFactory;

template <class FactoryProductType, class ModuleType>
struct StandardCreate
{
  static
  boost::shared_ptr<FactoryProductType>
  Create (const I3Context& c)
  {
    boost::shared_ptr<FactoryProductType> module(new ModuleType(c));
    if (!module)
      log_fatal("failed to create");
    return module;
  }
};


template <class FactoryProductType,
          class ActualDerivedType,
          template <class, class> class Creator>
struct I3Registrator : boost::noncopyable
{
  I3Registrator& key_register(const char* productname,
                              const char* projectname) I3_USED
  {
    log_trace("key_register %s %s", productname, projectname);

    typedef I3Factory<FactoryProductType,
      boost::function<boost::shared_ptr<FactoryProductType>(const I3Context&)>
      > factory_t;

    I3::Singleton<factory_t>::get_mutable_instance()
      .Register(productname,
                projectname,
                Creator<FactoryProductType, ActualDerivedType>::Create);
    return *this;
  }

  // this should technically be private (see friend declaration below)
  // but some gccs get confused.
  I3Registrator() {};

private:

  friend class I3::Singleton<I3Registrator>;
};


#define I3_REGISTER(PRODUCT, TYPE, CREATOR)                             \
  namespace {                                                           \
      ::I3Registrator<PRODUCT, TYPE, CREATOR> const&                    \
      BOOST_PP_CAT(I3Registrator_, __LINE__)                            \
      = I3::Singleton<I3Registrator<PRODUCT, TYPE, CREATOR> >           \
      ::get_mutable_instance().key_register(BOOST_PP_STRINGIZE(TYPE),   \
                                            BOOST_PP_STRINGIZE(PROJECT)); \
  }



#define I3_MODULE(TYPE) I3_REGISTER(I3Module, TYPE, StandardCreate)

#define I3_SERVICE_FACTORY(TYPE) I3_REGISTER(I3ServiceFactory, TYPE, StandardCreate)

#endif
#endif

