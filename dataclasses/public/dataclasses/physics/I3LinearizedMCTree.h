/**
 * @file I3LinearizedMCTree.h
 * @date $Date$
 * @author jvansanten
 *
 * (c) 2012 Jakob van Santen <vansanten@wisc.edu>
 *          and the IceCube Collaboration
 * the IceCube Collaboration
 * $Id$
 *
 */

#ifndef DATACLASSES_I3LINEARIZEDMCTREE_H_INCLUDED
#define DATACLASSES_I3LINEARIZEDMCTREE_H_INCLUDED

#include "dataclasses/physics/I3MCTree.h"

static const unsigned i3linearizedmctree_version_ = 1;

/**
 * @brief A compact I3MCTree
 * 
 * I3LinearizedMCTree is an alternate format for I3MCTree, optimized for
 * efficient on-disk storage in the case where the tree is dominated by
 * a large number of terminal nodes containing I3Particles that are
 * co-linear with their parents. In these cases the daughter particles
 * can be completely described by a time, energy, type, and minor/major ids,
 * as well as the parameters of the parent particle.
 *
 * Entries in the I3MCTree returned by I3LinearizedMCTree::operator I3MCTree
 * are guaranteed to be identical to the entries in the original tree, with
 * the following exceptions:
 * - Times and energies will only be identical to within single precision
 * - The positions of daughter particles may deviate by up to 50 cm.
 */
class I3LinearizedMCTree : public I3MCTree {
public:
	I3LinearizedMCTree() {};
	explicit I3LinearizedMCTree(const I3MCTree &tree) : I3MCTree(tree) {};
			
private:
	typedef I3MCTree::pre_order_iterator pre_iterator;
	typedef I3MCTree::post_order_iterator post_iterator;
	typedef I3MCTree::sibling_iterator sibling_iterator;
		
	friend class icecube::serialization::access;
	template <class Archive>
	void load(Archive &ar, unsigned version);
	template <class Archive>
	void save(Archive &ar, unsigned version) const;
	I3_SERIALIZATION_SPLIT_MEMBER();
	
};

I3_POINTER_TYPEDEFS(I3LinearizedMCTree);
I3_CLASS_VERSION(I3LinearizedMCTree,i3linearizedmctree_version_);

#endif /* DATACLASSES_I3LINEARIZEDMCTREE_H_INCLUDED */
