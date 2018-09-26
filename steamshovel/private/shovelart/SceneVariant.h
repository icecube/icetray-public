#ifndef I3_SHOVEL_RENDERING_SCENE_VARIANT_H
#define I3_SHOVEL_RENDERING_SCENE_VARIANT_H

#include <limits>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <QColor>

#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/physics/I3Particle.h>

#include "graphicsmath.h"
#include "timewindow.h"

/** Base class of variant types
 *
 * A SceneVariant describes a property of a SceneObject, such as size or color.
 * Its value may be constant, or may change in response to the current visible time.
 */
template<typename T>
class SceneVariant{
public:
	typedef T T_type_;
	virtual ~SceneVariant(){}
	virtual T value( double vistime ) = 0;

};

typedef SceneVariant<double> VariantTime;
I3_POINTER_TYPEDEFS( VariantTime );

typedef SceneVariant<float> VariantFloat;
I3_POINTER_TYPEDEFS( VariantFloat );

typedef SceneVariant<vec3d> VariantVec3d;
I3_POINTER_TYPEDEFS( VariantVec3d );

typedef SceneVariant<QColor> VariantQColor;
I3_POINTER_TYPEDEFS( VariantQColor );

/** A constant-valued SceneVariant */
template<typename T>
class SceneConstant : public SceneVariant<T> {
private:
	T value_;

public:
	SceneConstant( const T& value ) : value_(value) {}
	T value( double ){ return value_; }
};

/** Abstract superclass of SceneVariants that are defined in terms of values at particular times */
template<typename T>
class SceneTimeFunction : public SceneVariant<T> {

protected:
	// using the ordered map as internal storage ensures iterations are time-ordered
	typedef std::map<double,T> timeMap;
	timeMap map_;

public:
	// static const double END = DBL_MAX;
	SceneTimeFunction( const T& value, double time = -std::numeric_limits<double>::max() ) :
		map_()
	{
		map_[time] = value;
	}

	void add( const T& value, double time ){
		map_[time] = value;
	}

	// T& operator()( double time ){ return map_[time]; }
	// const T& operator()( double time ) const{ return map_[time]; }
};

/** Step-function SceneVariant defined by values at particular times
 *
 * Variant representing a function that is piecewise constant.
 */
template<typename T>
class SceneStepFunction : public SceneTimeFunction<T> {

public:
	/** Construct with initial value */
	SceneStepFunction( const T& value, double time = -std::numeric_limits<double>::max() ) :
		SceneTimeFunction<T>( value, time )
	{}

	virtual T value( double vistime ){
		typename SceneTimeFunction<T>::timeMap::iterator j = this->map_.lower_bound(vistime);
		if ( j != this->map_.begin() )
			--j;
		return j->second;
	}

};

/** SceneVariant that interpolates between values at specified times
 *
 * Requires that its template type defines * and + operators
 */
template<typename T>
class SceneLinterpFunction : public SceneTimeFunction<T> {

public:
	SceneLinterpFunction( const T& value, double time = -std::numeric_limits<double>::max() ) :
		SceneTimeFunction<T>( value, time )
	{}

	inline T mix( double r, const T &vmin, const T &vmax )
	{
		return r * vmax + ( (1.0-r) * vmin );
	}

	virtual T value( double vistime ){
		typename SceneTimeFunction<T>::timeMap::iterator j = this->map_.lower_bound(vistime);
		if ( j == this->map_.end() )
			return (--j)->second;
		else if ( j == this->map_.begin() )
			return j->second;
		
		// okay, we are between two values and can interpolate
		typename SceneTimeFunction<T>::timeMap::iterator i(boost::prior(j));
		double tmin = (*i).first, tmax = (*j).first;
		const T& vmin = (*i).second, &vmax = (*j).second;
		double r = (vistime - tmin) / (tmax - tmin);
		return mix(r, vmin, vmax);
	}

};

/** Variant pointing to one of the members of a given TimeWindow
 *
 * The value that this variant takes on will vary as the underlying TimeWindow varies.
 */
class WindowTime : public VariantTime {

protected:
	const TimeWindow* window_;
	TimeWindow::TimePoint point_;

public:
	WindowTime( const TimeWindow* window, TimeWindow::TimePoint point ):
		window_(window), point_(point)
	{}

	virtual double value( double vistime ){
		return window_->getTime(point_);
	}
};

template<typename Pointer>
class ParticlePointT : public VariantVec3d {

protected:
	const Pointer particle_;
	WindowTime time_;
	const float offset_;

	vec3d loc_at_time( double t ){
	  float dist = particle_->GetSpeed() * (t - particle_->GetTime());
	  const float pLength = particle_->GetLength();
	  if (pLength > 0.0f)
	    dist = std::min(dist, pLength);

	  dist += offset_;
	  const I3Position& pos = particle_->GetPos();
	  const I3Direction& dir = particle_->GetDir();
	  return vec3d(pos.GetX() + dist * dir.GetX(),
	               pos.GetY() + dist * dir.GetY(),
	               pos.GetZ() + dist * dir.GetZ());
	}

public:
	ParticlePointT( Pointer p, WindowTime t ):
		particle_(p), time_(t), offset_(0.0f)
	{}

	ParticlePointT( Pointer p,
	                const TimeWindow* window,
	                TimeWindow::TimePoint point,
	                float offset = 0.0f ):
		particle_(p), time_(window, point), offset_(offset)
	{}

    virtual vec3d value( double vistime ){
      I3Particle::ParticleShape shape = particle_->GetShape();
      double t = time_.value(vistime);
      if( shape == I3Particle::StartingTrack ||
          shape == I3Particle::ContainedTrack){
        t = std::max( t, particle_->GetStartTime() );
      }

      if( shape == I3Particle::StoppingTrack ||
          shape == I3Particle::ContainedTrack ){
        t = std::min( t, particle_->GetStopTime() );
      }

      if( shape == I3Particle::Null )
        t = std::max( t, particle_->GetTime() );

      return loc_at_time( t );
    }
};

typedef ParticlePointT<I3ParticleConstPtr> ParticlePoint;
typedef ParticlePointT<const I3Particle*> BorrowingParticlePoint;
I3_POINTER_TYPEDEFS( ParticlePoint );

// specialize for QColor interpolation
template<>
inline QColor SceneLinterpFunction<QColor>::mix( double r, const QColor &vmin, const QColor &vmax )
{
	QColor ret = QColor();
	int h = r * vmax.hue() + ( (1.0f-r) * vmin.hue() );
	int s = r * vmax.saturation() + ( (1.0f-r) * vmin.saturation() );
	int v = r * vmax.value() + ( (1.0f-r) * vmin.value() );
	ret.setHsv( h, s, v );
	return ret;
}

#endif /* I3_SHOVEL_RENDERING_SCENE_VARIANT_H */
