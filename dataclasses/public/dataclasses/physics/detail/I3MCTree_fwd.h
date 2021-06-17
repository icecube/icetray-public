#ifndef I3MCTREE_FWD_H
#define I3MCTREE_FWD_H

class I3Particle;
struct I3ParticleID;

namespace TreeBase {
  template<typename T> class TreeNode;
  template<typename T, typename Key=T, typename Hash=hash<Key> > class Tree;
}

/**
 * I3MCTree - This goes into the frame and everyone can see it
 */
typedef TreeBase::Tree<I3Particle,I3ParticleID> I3MCTree;

#endif
