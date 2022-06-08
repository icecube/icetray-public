//
//   Copyright (c) 2004, 2005, 2006, 2007   Troy D. Straszheim  
//   
//   $Id$
//
//   This file is part of IceTray.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//   
//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//   ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//   SUCH DAMAGE.
//   
//   SPDX-License-Identifier: BSD-2-Clause
//   
//

#include <vector>
#include <algorithm>
#include <exception>

#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/I3Constants.h>
#include <icetray/python/copy_suite.hpp>
#include <icetray/python/operator_suite.hpp>
#include <icetray/python/boost_serializable_pickle_suite.hpp>
#include <icetray/python/get_class.hpp>
#include <icetray/python/stream_to_string.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/object.hpp>
#include <boost/python/slice.hpp>
#include <boost/python/extract.hpp>
#include <boost/function.hpp>

using namespace boost::python;

struct not_found_exception : std::exception
{
  std::string msg;
  not_found_exception(std::string m) : msg(m) { }
  ~not_found_exception() throw () { }
  char const* what() const  throw() { return msg.c_str(); }
};
void translate(const not_found_exception& e)
{
    // Use the Python 'C' API to set up an exception object
    PyErr_SetString(PyExc_IndexError, e.what());
}

I3Particle getParticle(const I3MCTree::optional_value& ptr, std::string err="")
{
  if (ptr)
    return *ptr;
  else
    throw not_found_exception(err);
}
I3Particle get_head(const I3MCTree& t)
{ return getParticle(t.get_head(),"no head in tree"); }
I3Particle parent(const I3MCTree& t,const I3ParticleID& p)
{ return getParticle(t.parent(p),"particle not found or no parent"); }
I3Particle previous_sibling(const I3MCTree& t,const I3ParticleID& p)
{ return getParticle(t.previous_sibling(p),"particle not found or no sibling"); }
I3Particle next_sibling(const I3MCTree& t,const I3ParticleID& p)
{ return getParticle(t.next_sibling(p),"particle not found or no sibling"); }
I3Particle first_child(const I3MCTree& t,const I3ParticleID& p)
{ return getParticle(t.first_child(p),"particle not found or no chilren"); }
I3Particle* at(I3MCTree& t,const I3ParticleID& p)
//{ return getParticle(t.at(p),"particle not found in tree"); }
{
  I3MCTree::iterator iter(t,p);
  if (iter == t.end())
    throw not_found_exception("particle not found in tree");
  else
    return &(*iter);
}
I3Particle* at2(I3MCTree& t,int num)
{
  I3Particle* ptr = NULL;
  if (num < 0) {
    if (unsigned(-num) > t.size())
      throw not_found_exception("particle index not found in tree");
    num = num+t.size();
  } else {
    if (unsigned(num) >= t.size())
      throw not_found_exception("particle index not found in tree");
  }
  I3MCTree::iterator iter = t.begin();
  for(int cnt=0;iter != t.end();cnt++) {
    if (cnt == num) {
      ptr = &(*iter);
      break;
    }
    iter++;
  }
  if (ptr != NULL)
    return ptr;
  else
    throw not_found_exception("particle index not found in tree");
}
std::vector<I3Particle> at3(I3MCTree& t,slice sl)
{
  std::vector<I3Particle> ret;
  int start = 0,stop=t.size(),step=1;
  if (sl.start() != object())
    start = extract<int>(sl.start());
  if (sl.stop() != object()) {
    stop = extract<int>(sl.stop());
    if (stop < 0)
        stop += t.size();
  }
  if (sl.step() != object())
    step = extract<int>(sl.step());
  I3MCTree::iterator iter = t.begin();
  for(int cnt=0;iter != t.end();cnt++) {
    if (cnt >= start && cnt < stop && cnt % step == 0)
      ret.push_back(*iter);
    if (cnt >= stop)
      break;
    iter++;
  }
  if (ret.size() > 0 && step < 1)
    std::reverse(ret.begin(),ret.end());
  return ret;
}

namespace outer {
  inline object pass_through(object const& o) { return o; }
  class sib_iter {
    public:
      sib_iter(const I3MCTree& t, const I3ParticleID& p)
        : i_(t,p), end_(t) { end_ = t.cend_sibling(); }
      I3Particle next()
      {
        if (i_ == end_) {
          PyErr_SetString(PyExc_StopIteration, "No more data.");
          boost::python::throw_error_already_set();
        }
        return *i_++;
      }
    private:
      I3MCTree::sibling_const_iterator i_;
      I3MCTree::sibling_const_iterator end_;
  };
}
outer::sib_iter sibling_iter(const I3MCTree& t, const I3ParticleID& p)
{ return outer::sib_iter(t,p); }

bool contains(const I3MCTree& t,const I3ParticleID& p)
{ return bool(t.at(p)); }
const std::vector<I3Particle> (I3MCTree::*children)(const I3ParticleID&) const  = &I3MCTree::children;
void (I3MCTree::*erase)(const I3ParticleID&) = &I3MCTree::erase;
void (I3MCTree::*erase_children)(const I3ParticleID&) = &I3MCTree::erase_children;
void (I3MCTree::*append_child_subtree)(const I3ParticleID&,const TreeBase::Tree<I3Particle,I3ParticleID>&,const I3ParticleID&) = &I3MCTree::append_child;
void (I3MCTree::*append_children)(const I3ParticleID&,const std::vector<I3Particle>&) = &I3MCTree::append_children;
void (I3MCTree::*insert_head)(const I3Particle&) = &I3MCTree::insert;
void (I3MCTree::*insert_after_head)(const I3Particle&) = &I3MCTree::insert_after;
void (I3MCTree::*insert)(const I3ParticleID&,const I3Particle&) = &I3MCTree::insert;
void (I3MCTree::*insert_after)(const I3ParticleID&,const I3Particle&) = &I3MCTree::insert_after;
void (I3MCTree::*insert_subtree)(const I3ParticleID&,const TreeBase::Tree<I3Particle,I3ParticleID>&,const I3ParticleID&) = &I3MCTree::insert_subtree;
void (I3MCTree::*insert_subtree_after)(const I3ParticleID&,const TreeBase::Tree<I3Particle,I3ParticleID>&,const I3ParticleID&) = &I3MCTree::insert_subtree_after;
void (I3MCTree::*replace)(const I3ParticleID&,const I3Particle&) = &I3MCTree::replace;
void (I3MCTree::*replace_subtree)(const I3ParticleID&,const TreeBase::Tree<I3Particle,I3ParticleID>&,const I3ParticleID&) = &I3MCTree::replace;
void (I3MCTree::*flatten)(const I3ParticleID&) = &I3MCTree::flatten;
void (I3MCTree::*reparent)(const I3ParticleID&,const I3ParticleID&) = &I3MCTree::reparent;
I3MCTree::size_type (I3MCTree::*depth)(const I3ParticleID&) const = &I3MCTree::depth;
I3MCTree::size_type (I3MCTree::*number_of_children)(const I3ParticleID&) const = &I3MCTree::number_of_children;
I3MCTree::size_type (I3MCTree::*number_of_siblings)(const I3ParticleID&) const = &I3MCTree::number_of_siblings;
bool (I3MCTree::*is_in_subtree)(const I3ParticleID&,const I3ParticleID&) const = &I3MCTree::is_in_subtree;
bool (I3MCTree::*subtree_in_tree)(const TreeBase::Tree<I3Particle,I3ParticleID>&,const I3ParticleID&) const = &I3MCTree::subtree_in_tree;

I3Particle get_best(const I3MCTree& tree, object func) {
  return getParticle(I3MCTreeUtils::GetBest(tree, func),"no particles in tree");
}
const std::vector<I3Particle> get_filter(const I3MCTree& tree, object func) {
  return I3MCTreeUtils::GetFilter(tree, func);
}
I3Particle get_best_filter(const I3MCTree& tree, object func1,object func2) {
  return getParticle(I3MCTreeUtils::GetBestFilter(tree, func1, func2),"no particles found");
}


bool mctree_bool(const I3MCTree& tree){
  return !(tree.empty());
}

void register_I3MCTree()
{
  {
    register_exception_translator<not_found_exception>(&translate);
    
    scope outer = 
      class_<I3MCTree, bases<I3FrameObject>, I3MCTreePtr>("I3MCTree",
        "A tree of I3Particles used for simulation.")
        // Extra Constructors
        .def(init<const I3Particle&>())
        
        // I3MCTreeUtils
        .def("get_daughters", &I3MCTreeUtils::GetDaughters, "Get all daughters/children of an I3ParticleID")
        .def("has_parent", &I3MCTreeUtils::HasParent, "Does the I3ParticleID have a parent?")
        .def("has", &I3MCTreeUtils::Has, "Does the I3ParticleID exist in the tree?")
        .def("add_primary", &I3MCTreeUtils::AddPrimary, "Add an I3Particle as a primary (at root-level)")
        .def("get_primary", &I3MCTreeUtils::GetPrimary, "Get the primary that created the I3ParticleID")
        .def("get_primaries", &I3MCTreeUtils::GetPrimaries, "Get a list of all primaries")
        .add_property("primaries", &I3MCTreeUtils::GetPrimaries)
        .def("append_child", &I3MCTreeUtils::AppendChild, "Add a child to an I3ParticleID")
        .def("get_particle", &I3MCTreeUtils::GetParticle, "Get the I3Particle represented by the I3ParticleID")
        .def("dump", &I3MCTreeUtils::Dump, "Return tree as a string")
        .def("get_best", get_best, "Get the best matching I3Particle")
        .def("get_filter", &get_filter, "Get the I3Particles passing the filter")
        .def("get_best_filter", &get_best_filter, "Get the best matching I3Particle passing the filter")
        
        // Base Class Methods
        .def("get_head", &get_head, "Get the left-most primary (the root or head of the tree)")
        .def("get_heads", &I3MCTree::get_heads, "Get a list of all primaries (the roots or heads of the tree)")
        .def("at", &at, return_internal_reference<>(), "Get the I3Particle represented by the I3ParticleID")
        .def("parent", &parent, "Get the parent of the I3ParticleID")
        .def("previous_sibling", &previous_sibling, "Get the previous sibling of the I3ParticleID")
        .def("next_sibling", &next_sibling, "Get the next sibling of the I3ParticleID")
        .def("children", children, "Get the children of the I3ParticleID")
        .def("first_child", &first_child, "Get the first (left-most) child of the I3ParticleID")
        .def("clear", &I3MCTree::clear, "Clear everything from the tree")
        .def("erase", erase, "Erase the I3ParticleID and all children")
        .def("erase_children", erase_children, "Erase only the children of the I3ParticleID (keeping the I3ParticleID itself)")
        .def("append_child", append_child_subtree)
        .def("append_children", append_children, "Add multiple children to an I3ParticleID")
        .def("insert", insert_head, "Add an I3Particle at the root level, before other I3Particles")
        .def("insert_after", insert_after_head, "Add an I3Particle at the root level, after other I3Particles")
        .def("insert", insert, "Add an I3Particle before the sibling I3ParticleID")
        .def("insert_after", insert_after, "Add an I3Particle after the sibling I3ParticleID")
        .def("insert_subtree", insert_subtree, "Add a subtree of I3Particles before the sibling I3ParticleID")
        .def("insert_subtree_after", insert_subtree_after, "Add a subtree of I3Particles after the sibling I3ParticleID")
        .def("replace", replace, "Replace an I3ParticleID with another I3Particle")
        .def("replace", replace_subtree, "Replace an I3ParticleID and all children with another subtree of I3Particles")
        .def("flatten", flatten, "Move the children of the I3Particle to be siblings after it")
        .def("reparent", reparent, "Move all children to an I3ParticleID, from another I3ParticleID in the tree")
        .def("merge",&I3MCTree::merge, "Merge two trees, modifying the first tree")
        .def("size", &I3MCTree::size, "Get the number of I3Particles in the tree")
        .def("empty", &I3MCTree::empty, "Is the tree empty?")
        .def("swap", &I3MCTree::swap, "Swap the contents of another I3MCTree with this one.")
        .def("depth", depth, "Get the depth from the I3ParticleID to the primary")
        .def("number_of_children", number_of_children, "Get the number of children an I3ParticleID has")
        .def("number_of_siblings", number_of_siblings, "Get the number of siblings an I3ParticleID has")
        .def("is_in_subtree", is_in_subtree, "Is an I3ParticleID in a subtree?")
        .def("subtree_in_tree", subtree_in_tree, "Is any part of a subtree in the tree?")
        
        // Iterators
        .def("pre_order_iter", boost::python::range<return_value_policy<copy_const_reference> >
          (
            (I3MCTree::pre_order_const_iterator(I3MCTree::*)() const) &I3MCTree::cbegin, 
            (I3MCTree::pre_order_const_iterator(I3MCTree::*)() const) &I3MCTree::cend
          ),
          "Pre order iterator for I3MCTree. This is the default iterator."
        )
        .def("post_order_iter", boost::python::range<return_value_policy<copy_const_reference> >
          (
            (I3MCTree::post_order_const_iterator(I3MCTree::*)() const) &I3MCTree::cbegin_post, 
            (I3MCTree::post_order_const_iterator(I3MCTree::*)() const) &I3MCTree::cend_post
          ),
          "Post order iterator for I3MCTree"
        )
        .def("sibling_iter",sibling_iter, "Sibling iterator for I3MCTree. Takes an I3ParticleID, provides all next_siblings of it.")
        .def("fast_iter", boost::python::range<return_value_policy<copy_const_reference> >
          (
            (I3MCTree::fast_const_iterator(I3MCTree::*)() const) &I3MCTree::cbegin_fast, 
            (I3MCTree::fast_const_iterator(I3MCTree::*)() const) &I3MCTree::cend_fast
          ),
          "Fast iterator for I3MCTree. Fast but unordered traversal."
        )
        .def("leaf_iter", boost::python::range<return_value_policy<copy_const_reference> >
          (
            (I3MCTree::leaf_const_iterator(I3MCTree::*)() const) &I3MCTree::cbegin_leaf, 
            (I3MCTree::leaf_const_iterator(I3MCTree::*)() const) &I3MCTree::cend_leaf
          ),
          "Leaf iterator for I3MCTree. Fast but unordered leaf traversal."
        )
        
        // Python Special Methods
        .def("__str__", &stream_to_string<I3MCTree>)
        .def("__nonzero__", &mctree_bool)
        .def("__bool__", &mctree_bool)      
        .def("__len__", &I3MCTree::size)
        .def("__iter__", boost::python::range<return_value_policy<copy_const_reference> >
          (
            (I3MCTree::const_iterator(I3MCTree::*)() const) &I3MCTree::cbegin, 
            (I3MCTree::const_iterator(I3MCTree::*)() const) &I3MCTree::cend
          )
        )
        .def("__contains__", &contains)
        .def("__getitem__", &at, return_internal_reference<>())
        .def("__getitem__", &at2, return_internal_reference<>())
        .def("__getitem__", &at3)
        .def("__delitem__", erase)
        
        // dataclass suites
        .def(copy_suite<I3MCTree>())
        .def(operator_suite<I3MCTree>())
        .def_pickle(boost_serializable_pickle_suite<I3MCTree>())
        .def("__value_type__", get_class<I3MCTree::value_type>)
        .staticmethod("__value_type__")
      ;
      class_<outer::sib_iter>("_Sibling_Iter_",
        "Sibling iterator object for I3MCTree. DO NOT CALL DIRECTLY. Instead use the I3MCTree.sibling_iter method",
        no_init)
        .def("next",&outer::sib_iter::next)
        .def("__next__",&outer::sib_iter::next)
        .def("__iter__",&outer::pass_through)
      ;
  }
  
  register_pointer_conversions<I3MCTree>();
}

