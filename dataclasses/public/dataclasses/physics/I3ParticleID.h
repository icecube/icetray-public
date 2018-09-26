#ifndef I3ParticleID_H_INCLUDED
#define I3ParticleID_H_INCLUDED

#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/I3Map.h>

#ifndef __CINT__
#include <icetray/serialization.h>
#endif

/**
 * @brief A single object for holding the ID numbers of an I3Particle
 *
 * In icetray, the "ID" of a particle object is a pair of (integer) numbers that
 * serve as its unique identifier.  No two particles, even two particles in
 * different data sets, can have the same identifier.  This is important because
 * simulation is not done in one single process, but instead in stages, where
 * particle generation and propagation can happen on different machines.  So if
 * we simply assigned '0' to the first particle created in a generator process,
 * its ID would clash with the first particle created by the propagator, both of
 * which will be added to the same I3MCTree.
 */

struct I3ParticleID{
  /**
   * This is a hash of the time (of I3Particle object creation),
   * process ID, and hostname.  So two particles created at the same
   * time on the same machine, but in different processes will have
   * different major IDs.
   */
  uint64_t majorID;

  /** This is meant to provide the bone-headed (and commonly used)
   * method of particle ID creation.  The first particle created is
   * '0' and the next '1', and so on.
   */
  int32_t minorID;

  // assume noise
  I3ParticleID() : majorID(0), minorID(0){};
  I3ParticleID(uint64_t major, int32_t minor) : 
    majorID(major), minorID(minor){};

  std::ostream& Print(std::ostream&) const;
  
  /**
   * Need to tell gcc to ignore the "maybe unitialized warning"
   * in this case.  A default constructor was added that initializes
   * these values and none of these methods are static, so this
   * is a false positive.
   *
   * clang seems to be smart enough to know these aren't used 
   * uninitialized, so we only turn them off for gcc, since
   * this just causes more clang warnings.
   *
   * These directives we need were added in gcc 4.6
   * https://gcc.gnu.org/gcc-4.6/changes.html
   */  
#if defined(__GNUC__) && GCC_VERSION > 40600
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
  bool operator==(const I3ParticleID& other) const {
    return(majorID == other.majorID && minorID == other.minorID);
  }

  bool operator!=(const I3ParticleID& other) const {
    return(majorID != other.majorID || minorID != other.minorID);
  }

  bool operator<(const I3ParticleID& other) const {
    if (majorID!=other.majorID)
      return(majorID<other.majorID);
    return(minorID<other.minorID);
  }
  // a pop without a push restores the command line options.
  // https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
#if defined(__GNUC__) && GCC_VERSION > 40600
#pragma GCC diagnostic pop
#endif

private:
#ifndef __CINT__
  friend class icecube::serialization::access;

  template <class Archive> void serialize(Archive & ar, const unsigned version)
  {
    ar & make_nvp("majorID",majorID);
    ar & make_nvp("minorID",minorID);
  }
#endif
};

I3_POINTER_TYPEDEFS(I3ParticleID);
typedef I3Map<I3ParticleID, double> I3MapI3ParticleIDDouble;
I3_POINTER_TYPEDEFS(I3MapI3ParticleIDDouble);

std::ostream& operator<<(std::ostream &, const I3ParticleID &);

#ifndef __CINT__
#include <I3/hash_map.h>

#ifdef USING_GCC_EXT_HASH_MAP

namespace __gnu_cxx{
  template<>
    struct hash<I3ParticleID>{
#if defined(__GNUC__) && GCC_VERSION > 40600
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
    size_t operator()(const I3ParticleID& id) const{
      return hash<int>()(id.majorID + id.minorID);
    }
#if defined(__GNUC__) && GCC_VERSION > 40600
#pragma GCC diagnostic pop
#endif
  };
}

#else //using std::unordered_map

template<>
struct i3hash<I3ParticleID>{
  size_t operator()(const I3ParticleID& id) const{
    return ::hash<int>()(id.majorID + id.minorID);
  }
};

#endif
#endif

#endif
