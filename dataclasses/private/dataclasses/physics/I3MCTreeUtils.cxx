#include <sstream>
#include "dataclasses/physics/I3MCTreeUtils.h"
#include "icetray/I3Frame.h"


void
I3MCTreeUtils::AddPrimary(I3MCTree& t, const I3Particle& p)
{
  t.insert_after(p);
  I3MCTree::iterator iter(t,p);
  i3_assert( iter != t.end() );
  iter->SetShape(I3Particle::Primary);
}

void
I3MCTreeUtils::AppendChild(I3MCTree& t, const I3ParticleID& parent, const I3Particle& child)
{
  if(child.GetID() == parent){
    log_fatal("Parent and child have the same ID.");
  }
  t.append_child(parent, child);
}


const std::vector<I3Particle>
I3MCTreeUtils::GetPrimaries(const I3MCTree& t)
{
  return t.get_heads();
}

const std::vector<const I3Particle*>
I3MCTreeUtils::GetPrimariesPtr(const I3MCTreeConstPtr t)
{
  std::vector<const I3Particle*> ret;
  I3MCTree::sibling_const_iterator iter(t->cbegin());
  for(;iter != t->cend_sibling();iter++)
    ret.push_back(&(*iter));
  return ret;
}

const std::vector<I3Particle*>
I3MCTreeUtils::GetPrimariesPtr(I3MCTreePtr t)
{
  std::vector<I3Particle*> ret;
  I3MCTree::sibling_iterator iter(t->begin());
  for(;iter != t->end_sibling();iter++)
    ret.push_back(&(*iter));
  return ret;
}

const std::vector<I3Particle>
I3MCTreeUtils::GetDaughters(const I3MCTree& t, const I3ParticleID& parent)
{
  return t.children(parent);
}

const std::vector<const I3Particle*>
I3MCTreeUtils::GetDaughtersPtr(const I3MCTreeConstPtr t, const I3ParticleID& parent)
{
  std::vector<const I3Particle*> ret;
  I3MCTree::const_iterator parent_iter(*t,parent);
  if (parent_iter == t->cend())
    return ret;
  parent_iter = t->first_child(parent_iter);
  if (parent_iter == t->cend())
    return ret;
  I3MCTree::sibling_const_iterator iter(parent_iter);
  for(;iter != t->cend_sibling();iter++)
    ret.push_back(&(*iter));
  return ret;
}

const std::vector<I3Particle*>
I3MCTreeUtils::GetDaughtersPtr(I3MCTreePtr t, const I3ParticleID& parent)
{
  std::vector<I3Particle*> ret;
  I3MCTree::iterator parent_iter(*t,parent);
  if (parent_iter == t->end())
    return ret;
  parent_iter = t->first_child(parent_iter);
  if (parent_iter == t->end())
    return ret;
  I3MCTree::sibling_iterator iter(parent_iter);
  for(;iter != t->end_sibling();iter++)
    ret.push_back(&(*iter));
  return ret;
}

const I3Particle
I3MCTreeUtils::GetParent(const I3MCTree& t, const I3ParticleID& child)
{
  I3MCTree::optional_value parent = t.parent(child);
  if (parent)
    return parent;
  else
    log_fatal("no parent found");
}

const I3Particle*
I3MCTreeUtils::GetParentPtr(const I3MCTreeConstPtr t, const I3ParticleID& child)
{
  const I3Particle* ret = NULL;
  I3MCTree::optional_value parent = t->parent(child);
  if (parent)
    ret = &(*I3MCTree::const_iterator(*t,*parent));
  return ret;
}

I3Particle*
I3MCTreeUtils::GetParentPtr(I3MCTreePtr t, const I3ParticleID& child)
{
  I3Particle* ret = NULL;
  I3MCTree::optional_value parent = t->parent(child);
  if (parent)
    ret = &(*I3MCTree::iterator(*t,*parent));
  return ret;
}

bool
I3MCTreeUtils::HasParent(const I3MCTree& t, const I3ParticleID& child)
{
  if (t.parent(child))
    return true;
  else
    return false;
}

bool
I3MCTreeUtils::Has(const I3MCTree& t, const I3ParticleID& particle)
{
  if (t.at(particle))
    return true;
  else
    return false;
}

const I3Particle
I3MCTreeUtils::GetParticle(const I3MCTree& t, const I3ParticleID& p)
{
  I3MCTree::optional_value ret = t.at(p);
  if (ret)
    return *ret;
  else
    log_fatal("particleID not found");
}

const I3Particle*
I3MCTreeUtils::GetParticlePtr(const I3MCTreeConstPtr t, const I3ParticleID& p)
{
  const I3Particle* ret = NULL;
  I3MCTree::const_iterator iter(*t,p);
  if (iter != t->cend())
    ret = &(*iter);
  return ret;
}

I3Particle*
I3MCTreeUtils::GetParticlePtr(I3MCTreePtr t, const I3ParticleID& p)
{
  I3Particle* ret = NULL;
  I3MCTree::iterator iter(*t,p);
  if (iter != t->end())
    ret = &(*iter);
  return ret;
}

const I3Particle
I3MCTreeUtils::GetPrimary(const I3MCTree& t, const I3ParticleID& p)
{
  I3MCTree::optional_value child,parent = t.at(p);
  if (!parent)
    log_fatal("particle not found");
  while(parent) {
    child = parent;
    parent = t.parent(*parent);
  }
  return *child;
}

const I3Particle*
I3MCTreeUtils::GetPrimaryPtr(const I3MCTreeConstPtr t, const I3ParticleID& p)
{
  const I3Particle* ret = NULL;
  I3MCTree::const_iterator child = t->cend(),parent(*t,p),end=t->cend();
  if (parent == end)
    return ret;
  while (parent != end) {
    child = parent;
    parent = t->parent(parent);
  }
  ret = &(*child);
  return ret;
}

I3Particle*
I3MCTreeUtils::GetPrimaryPtr(I3MCTreePtr t, const I3ParticleID& p)
{
  I3Particle* ret = NULL;
  I3MCTree::iterator child = t->end(),parent(*t,p),end=t->end();
  if (parent == end)
    return ret;
  while (parent != end) {
    child = parent;
    parent = t->parent(parent);
  }
  ret = &(*child);
  return ret;
}

const I3MCTreeConstPtr
I3MCTreeUtils::Get(const I3Frame &frame, const std::string& key)
{
  return frame.Get<I3MCTreeConstPtr>(key);
}

std::string
I3MCTreeUtils::Dump(const I3MCTree& t)
{
  std::stringstream s;
  I3Position pos;
  I3MCTree::const_iterator iter = t.cbegin(),end = t.cend();
  for (;iter != end;iter++) {
    for (unsigned int d=0;d<t.depth(*iter);d++) { s << "  "; }
    s << iter->GetMinorID() << " " << iter->GetTypeString() << " ";
    pos = iter->GetPos();
    s << "(" << pos.GetX()/I3Units::m << "m ,";
    s << pos.GetY()/I3Units::m << "m ,";
    s << pos.GetZ()/I3Units::m << "m) ";
    s << "(" << iter->GetZenith()/I3Units::degree << "deg ,";
    s << iter->GetAzimuth()/I3Units::degree << "deg) ";
    s << iter->GetTime()/I3Units::ns << "ns ";
    s << iter->GetEnergy()/I3Units::GeV << "GeV ";
    s << iter->GetLength()/I3Units::m << "m " << std::endl;
  }
  return s.str();
}

