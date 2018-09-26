/**
 * copyright  (C) 2013
 * the icecube collaboration
 * @file I3MCTreeUtils.h
 */

#ifndef I3MCTREEUTILS_H_INCLUDED
#define I3MCTREEUTILS_H_INCLUDED

#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3ParticleID.h"
#include "dataclasses/physics/I3MCTree.h"

namespace I3MCTreeUtils
{
  /**
   * Adds a primary (top-level node) to the tree.
   * The primary has no parent.
   * A tree can have multiple primaries.
   * Sets the shape to Primary
   */
  void AddPrimary(I3MCTree&, const I3Particle&);
  
  /**
   * Appends a child to the parent particle
   */
  void AppendChild(I3MCTree&, const I3ParticleID&, const I3Particle&);
  
  /**
   * Gets a list of primaries from the tree
   */
  const std::vector<I3Particle> GetPrimaries(const I3MCTree&);
  const std::vector<const I3Particle*> GetPrimariesPtr(I3MCTreeConstPtr);
  const std::vector<I3Particle*> GetPrimariesPtr(I3MCTreePtr);
  
  /**
   * Gets a list of daughters of the parent particle.
   */
  const std::vector<I3Particle> GetDaughters(const I3MCTree&, const I3ParticleID&);
  const std::vector<const I3Particle*> GetDaughtersPtr(const I3MCTreeConstPtr, const I3ParticleID&);
  const std::vector<I3Particle*> GetDaughtersPtr(I3MCTreePtr, const I3ParticleID&);
  
  /**
   *Gets the parent of a particleID. log_fatal or NULL if parent does not exist
   */
  const I3Particle GetParent(const I3MCTree&, const I3ParticleID&);
  const I3Particle* GetParentPtr(const I3MCTreeConstPtr, const I3ParticleID&);
  I3Particle* GetParentPtr(I3MCTreePtr, const I3ParticleID&);
  
  /**
   * Returns true if the particle is in the tree
   */
  bool Has(const I3MCTree&, const I3ParticleID&);

  /**
   * Returns true if the particle has a parent
   */
  bool HasParent(const I3MCTree&, const I3ParticleID&);
  
  /**
   * This gets the particle with a particleID.
   * log_fatal or NULL if particle does not exist.
   */
  const I3Particle GetParticle(const I3MCTree&, const I3ParticleID&);
  const I3Particle* GetParticlePtr(const I3MCTreeConstPtr, const I3ParticleID&);
  I3Particle* GetParticlePtr(I3MCTreePtr, const I3ParticleID&);
  
  /**
   * This gets the primary that created the particleID.
   * log_fatal or NULL if particle does not exist.
   */
  const I3Particle GetPrimary(const I3MCTree&, const I3ParticleID&);
  const I3Particle* GetPrimaryPtr(const I3MCTreeConstPtr, const I3ParticleID&);
  I3Particle* GetPrimaryPtr(I3MCTreePtr, const I3ParticleID&);
  
  /**
   * Gets an I3MCTree from the frame with the specificed key.
   */
  const I3MCTreeConstPtr Get(const I3Frame& , const std::string&);
  
  
  /**
   * Dumps an I3MCTree to a std::string
   */
  std::string Dump(const I3MCTree&);
  
  /**
   * Get the "best match" particle in the tree using a comparison Function.
   * 
   * \param t    I3MCTree
   * \param func Callable which takes two I3Particles, compares them, and returns
   *             true if the first is better, false if the second is better
   */
  template<typename Function>
  const typename I3MCTree::optional_value
  GetBest(const I3MCTree& t, Function func)
  {
    typename I3MCTree::fast_const_iterator iter(t), end=t.cend_fast();
    typename I3MCTree::optional_value ret;
    if (iter == end)
      return ret;
    ret = *iter++;
    for(;iter != end;iter++) {
      if (!func(*ret,*iter))
        ret = *iter;
    }
    return ret;
  }
  template<typename Function>
  const typename I3MCTree::fast_const_iterator
  GetBestPtr(const I3MCTreeConstPtr t, Function func)
  {
    assert((bool)t);
    typename I3MCTree::fast_const_iterator iter(*t), end=t->cend_fast();
    typename I3MCTree::fast_const_iterator ret = iter;
    if (iter == end)
      return ret;
    for(iter++;iter != end;iter++) {
      if (func(*iter,*ret))
        ret = iter;
    }
    return ret;
  }
  
  /**
   * Get all matching particles in the tree using a filter Function.
   * 
   * \param t    I3MCTree
   * \param func Callable which takes an I3Particle and returns true/false
   */
  template<typename Function>
  const std::vector<I3Particle>
  GetFilter(const I3MCTree& t, Function func)
  {
    std::vector<I3Particle> ret;
    typename I3MCTree::fast_const_iterator iter(t), end=t.cend_fast();
    for(;iter != end;iter++) {
      if (func(*iter))
        ret.push_back(*iter);
    }
    return ret;
  }
  template<typename Function>
  const std::vector<typename I3MCTree::fast_const_iterator>
  GetFilterPtr(const I3MCTreeConstPtr t, Function func)
  {
    std::vector<typename I3MCTree::fast_const_iterator> ret;
    typename I3MCTree::fast_const_iterator iter(*t), end=t->cend_fast();
    for(;iter != end;iter++) {
      if (func(*iter))
        ret.push_back(iter);
    }
    return ret;
  }
  
  /**
   * Get best matching particle in the tree using a filter Function
   * and a comparison Function.
   * 
   * \param t I3MCTree
   * \param f Callable which takes an I3Particle and returns true/false
   * \param c Callable which takes two I3Particles, compares them, and returns
   *          true if the first is better, false if the second is better
   */
  template<typename FilterFunction,typename CmpFunction>
  const typename I3MCTree::optional_value
  GetBestFilter(const I3MCTree& t, FilterFunction f, CmpFunction c)
  {
    typename I3MCTree::optional_value ret;
    typename I3MCTree::fast_const_iterator iter(t), end=t.cend_fast();
    for(;iter != end;iter++) {
      if (f(*iter) && (!ret || !c(*ret,*iter)))
        ret = *iter;
    }
    return ret;
  }
  template<typename FilterFunction,typename CmpFunction>
  const typename I3MCTree::fast_const_iterator
  GetBestFilterPtr(const I3MCTreeConstPtr t, FilterFunction f, CmpFunction c)
  {
    typename I3MCTree::fast_const_iterator iter(*t), end=t->cend_fast();
    typename I3MCTree::fast_const_iterator ret = end;
    for(;iter != end;iter++) {
      if (f(*iter) && (ret == end || !c(*ret,*iter)))
        ret = iter;
    }
    return ret;
  }

}

#endif 


