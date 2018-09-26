/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3Tree.h
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */

#ifndef I3_TREE_H_INCLUDED
#define I3_TREE_H_INCLUDED

#include <icetray/serialization.h>
#include <stack>
#include <dataclasses/external/tree.hh>
#include <dataclasses/Utility.h>
#include <icetray/I3FrameObject.h>


/**
 * STL-like container class for n-ary trees.
 */
template <typename T>
struct I3Tree : public I3FrameObject , public tree<T>
{
  typedef typename tree<T>::iterator_base iterator_base;
  

  I3Tree(){}
  
  I3Tree(const T& value) : tree<T>(value){}
  
  I3Tree(const iterator_base& other) : tree<T>(other){}

  template<class Archive>
  void save(Archive& ar, const unsigned int version) const
  {
    typename tree<T>::pre_order_iterator iter;
    unsigned int count;

    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    
    iter = this->begin();
    count = this->empty() ? 0 : this->number_of_siblings(iter) + 1;
    ar & icecube::serialization::make_nvp("count", count);
    while(iter != this->end())
      {
        count = this->number_of_children(iter);
        ar & icecube::serialization::make_nvp("item", *iter);
        ar & icecube::serialization::make_nvp("count", count);
        ++iter;
      }
  }


  template<class Archive>
  void load(Archive& ar, const unsigned int version)
  {
    typename tree<T>::pre_order_iterator iter;
    std::stack<typename tree<T>::pre_order_iterator> iters;
    unsigned int count;
    std::stack<unsigned int> counts;
    T item;

    ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));

    ar & icecube::serialization::make_nvp("count", count);
    if(count)
      counts.push(count);

    while(!counts.empty())
      {
        ar & icecube::serialization::make_nvp("item", item);
        ar & icecube::serialization::make_nvp("count", count);
        if(iters.empty()) {
          iter = this->begin();
          if (!this->empty()) {
            unsigned siblings = this->number_of_siblings(iter);
            for (unsigned i=0; i <= siblings; i++)
              iter = this->next_sibling(iter);
          }
          iter = this->insert(iter, item);
        } else
          iter = this->append_child(iters.top(), item);
        counts.top()--;

        if(!counts.top())
          {
            counts.pop();
            if(!iters.empty())
              iters.pop();
          }

        if(count)
          {
            counts.push(count);
            iters.push(iter);
          }
      }
  }
  I3_SERIALIZATION_SPLIT_MEMBER()
};


#endif // I3_TREE_H_INCLUDED
