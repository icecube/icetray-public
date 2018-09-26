#ifndef I3_SHOVEL_RANGE_SETTING_H
#define I3_SHOVEL_RANGE_SETTING_H

#include "ComplexSetting.h"
#include <cmath>
#include <icetray/I3Logging.h>

class RangeSettingEditor;

/**
 * A value range, with a min, max, current value, and step size.
 * Artists can define these to put sane bounds
 * on their floats and to get a nice slider.
 */
class RangeSetting: public ComplexSetting<float> {
	friend class RangeSettingEditor;

	// for editor: define range and get/set value via index
	float vmin_, vmax_;
	int nmax_;

public:
	typedef float Value;

	/**
	 * @param vmin Minimum value
	 * @param vmax Maximum value
	 * @param steps Steps between min and max
	 * @param val initial value
	 */
	RangeSetting( float vmin, float vmax, int steps, float val ) :
		ComplexSetting<float>( std::max(std::min(val, vmax), vmin) ),
		vmin_( vmin ),
		vmax_( vmax ),
		nmax_( steps )
	{}

	// help C++ to resolve ambiguities
	RangeSetting( float vmin, float vmax, int steps, double val ) :
		ComplexSetting<float>( std::max(std::min(float(val), vmax), vmin) ),
		vmin_( vmin ),
		vmax_( vmax ),
		nmax_( steps )
	{}

	// this constructor is deprecated and should be phased out by the end of 2015
	RangeSetting( float vmin, float vmax, int steps, int init ) :
		ComplexSetting<float>( vmin + (vmax - vmin) * float(init) / steps ),
		vmin_( vmin ),
		vmax_( vmax ),
		nmax_( steps )
	{
		log_warn_stream( "Warning: Found old ctor RangeSetting("
		                 << vmin << ", " << vmax << ", " << steps << ", " << init
		                 << ") replace with RangeSetting("
		                 << vmin << ", " << vmax << ", " << steps << ", "
		                 << float(*this) << ")" );
	}

	// Default constructor is needed by QVariant
	RangeSetting() :
		ComplexSetting<float>(0),
		vmin_(0),
		vmax_(1),
		nmax_(1)
	{}
};

#endif /* I3_SHOVEL_RANGE_SETTING_H */
