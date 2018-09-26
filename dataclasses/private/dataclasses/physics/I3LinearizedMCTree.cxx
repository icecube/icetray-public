/**
 * @file I3LinearizedMCTree.cxx
 * @date $Date$
 * @author jvansanten
 *
 * (c) 2012 Jakob van Santen <vansanten@wisc.edu>
 *          and the IceCube Collaboration
 * the IceCube Collaboration
 * $Id$
 *
 */

#include "dataclasses/physics/I3LinearizedMCTree.h"
#include "dataclasses/I3Constants.h"
#include "icetray/I3Units.h"

#include <boost/foreach.hpp>
#include <queue>

class I3Stochastic {
public:
  I3Stochastic() : time_(0), energy_(0) {};
  I3Stochastic(const I3Particle &parent, const I3Particle &stochastic);
  ~I3Stochastic() {}
  
  static bool IsCompressible(const I3Particle &parent, const I3Particle &stochastic);
  bool operator<(const I3Stochastic &other) const;
  
  I3Particle Reconstruct(const I3Particle &parent) const;
  
private:
  static void Propagate(I3Particle &p, double time);
  
  friend class icecube::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, unsigned version);
  
  float time_, energy_;
  uint64_t major_id_;
  int32_t minor_id_;
  I3Particle::ParticleType type_;
};

template <class Archive>
void
I3Stochastic::serialize(Archive &ar, unsigned version)
{
	ar & make_nvp("Time", time_);
	ar & make_nvp("Energy", energy_);
	ar & make_nvp("MajorID", major_id_);
	ar & make_nvp("MinorID", minor_id_);
	ar & make_nvp("Type", type_);
}

I3_SERIALIZABLE(I3Stochastic);

I3Stochastic::I3Stochastic(const I3Particle &parent, const I3Particle &p)
{
	time_ = p.GetTime()-parent.GetTime();
	energy_ = p.GetEnergy();
	major_id_ = p.GetMajorID();
	minor_id_ = p.GetMinorID();
	type_ = p.GetType();
}

bool I3Stochastic::operator<(const I3Stochastic &other) const
{
	return time_ < other.time_;
}

bool
I3Stochastic::IsCompressible(const I3Particle &parent, const I3Particle &p)
{
	if ((p.IsCascade() && p.GetLocationType() == I3Particle::InIce)
	    && (p.GetFitStatus() == I3Particle::NotSet) && (p.GetLength() == 0.*I3Units::m)
	    && (p.GetSpeed() == I3Constants::c) && (p.GetDir() == parent.GetDir())) {
		    I3Particle newp(p);
		    Propagate(newp, parent.GetTime() - p.GetTime());
		    double position_err = (newp.GetPos()-parent.GetPos()).Magnitude();
		    return (position_err < 0.5*I3Units::m);
	} else {
		return false;
	}
}

I3Particle
I3Stochastic::Reconstruct(const I3Particle &parent) const
{
	I3Particle p(major_id_, minor_id_);
	
	p.SetShape(I3Particle::Null);
	p.SetFitStatus(I3Particle::NotSet);
	p.SetLocationType(I3Particle::InIce);
	p.SetSpeed(I3Constants::c);
	p.SetLength(0.*I3Units::m);
	
	p.SetEnergy(energy_);
	p.SetType(type_);
	
	p.SetDir(parent.GetDir());
	p.SetPos(parent.GetPos());
	p.SetTime(parent.GetTime());
	Propagate(p, time_);
	
	return p;
}

/**
 * I3Particle::ShiftAlongTrack() without stupid assert()s.
 */ 
void
I3Stochastic::Propagate(I3Particle &p, double tick)
{
	I3Position pos = p.GetPos();
	const I3Direction &dir = p.GetDir();
	double distance = tick*p.GetSpeed();
		
	pos.SetX(pos.GetX() + distance*dir.GetX());
	pos.SetY(pos.GetY() + distance*dir.GetY());
	pos.SetZ(pos.GetZ() + distance*dir.GetZ());
	p.SetPos(pos);
	p.SetTime(p.GetTime() + tick);
}

template <class Archive>
void
I3LinearizedMCTree::save(Archive &ar, unsigned version) const
{
	typedef std::pair<I3ParticleID, unsigned> leaf_t;
	std::map<I3ParticleID, std::vector<I3Stochastic> > stripped;
	
	I3MCTree tree(*this);
	for ( auto it = tree.cbegin_post(); it != tree.cend_post(); ) {
		I3MCTree::const_iterator parent = tree.parent(it);
		// Strip out nodes that
		//    a) have a parent, and
		//    b) are leaf nodes, and
		//    c) are stochastic energy losses, and
		//    d) are not the parents of already-stripped nodes
		if (parent != tree.cend() && tree.first_child(it) == tree.cend_post() &&
		    I3Stochastic::IsCompressible(*parent, *it)
		    && (stripped.find(*it) == stripped.end())) {
			stripped[parent->GetID()].emplace_back(*parent, *it);
			it = tree.erase(it);
		} else {
			it++;
		}
	}

	// Now, flatten the list of stripped nodes, noting the number of children
	// to attach to each parent
	std::vector<I3Stochastic> stochastics;
	std::vector<leaf_t> leaves;
	for(auto &pair : stripped) {
		leaves.emplace_back(pair.first, pair.second.size());
		std::copy(pair.second.begin(), pair.second.end(),
		    std::back_inserter(stochastics));
	}
	
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	ar & make_nvp("I3MCTree", tree);
	ar & make_nvp("Leaves", leaves);
	ar & make_nvp("Stochastics", stochastics);
}

template <class Archive>
void
I3LinearizedMCTree::load(Archive &ar, unsigned version)
{
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	ar & make_nvp("I3MCTree", base_object<I3MCTree>(*this));
	if (version == 0) {
		typedef std::pair<unsigned, unsigned> range_t;
		std::vector<I3Stochastic> stochastics;
		std::vector<range_t> ranges;
		ar & make_nvp("Ranges", ranges);
		ar & make_nvp("Stochastics", stochastics);
	
		// Find the points in the *stripped* tree where
		// we should attach the stripped leaves.
		std::queue<I3ParticleID> parents;
		{
			pre_iterator it = this->begin();
			unsigned offset = 0;
			BOOST_FOREACH(const range_t &span, ranges) {
			        i3_assert(span.first >= offset);
				std::advance(it, span.first-offset);
				offset = span.first;
				parents.push(it->GetID());
			}
		}
	
		std::vector<I3Stochastic>::const_iterator leaf = stochastics.begin();
		BOOST_FOREACH(const range_t &span, ranges) {
			pre_iterator parent = this->find(parents.front());
			i3_assert(parent != this->end());

			// Insert new leaves in time order relative to existing siblings
			sibling_iterator splice = this->children(parent);
			for (unsigned i = 0; i < span.second; i++, leaf++) {
				I3Particle reco = leaf->Reconstruct(*parent);
				while (splice != this->end_sibling() && reco.GetTime() >= splice->GetTime())
					splice++;
				if (splice == this->end_sibling())
					splice = this->append_child(parent, reco);
				else
					splice = this->insert(splice, reco);
			}
		
			parents.pop();
		}
	} else if (version == 1) {
		typedef std::pair<I3ParticleID, unsigned> leaf_t;
		
		std::vector<I3Stochastic> stochastics;
		std::vector<leaf_t> leaves;
		ar & make_nvp("Leaves", leaves);
		ar & make_nvp("Stochastics", stochastics);
		
		std::vector<I3Stochastic>::const_iterator leaf = stochastics.begin();
		for (const auto &span : leaves) {
			pre_iterator parent = this->find(span.first);
			
			// Insert new leaves in time order relative to existing siblings
			sibling_iterator splice = this->children(parent);
			for (unsigned i = 0; i < span.second; i++, leaf++) {
				I3Particle reco = leaf->Reconstruct(*parent);
				while (splice != this->end_sibling() && reco.GetTime() >= splice->GetTime())
					splice++;
				if (splice == this->end_sibling())
					splice = this->append_child(parent, reco);
				else
					splice = this->insert(splice, reco);
			}
		}
		i3_assert(leaf == stochastics.end());
		
	} else {
		log_fatal_stream("version " << version << " is from the future!");
	}
}

// Don't bother with compact serialization for XML archives
template<>
void I3LinearizedMCTree::save(icecube::archive::xml_oarchive& ar, unsigned version) const
{
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	ar & make_nvp("I3MCTree", base_object<I3MCTree>(*this));
}

template<>
void I3LinearizedMCTree::load(icecube::archive::xml_iarchive& ar, unsigned version)
{
	ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
	ar & make_nvp("I3MCTree", base_object<I3MCTree>(*this));
}

I3_SERIALIZABLE(I3LinearizedMCTree);

