#ifndef I3_SHOVEL_ARTIST_DETECTOR_H
#define I3_SHOVEL_ARTIST_DETECTOR_H

#include <dataclasses/geometry/I3Geometry.h>

#include "shovelart/Artist.h"

/**
 * Artist to draw DOMs and strings, with optional DOM labels
 */
class Detector : public GLArtist{
public:
	Detector();

	ARTIST_REQUIRED_TYPES( (I3Geometry) );

	void create( I3FramePtr ptr, SceneGroup* group, const SceneState& state );
};

REGISTER_ARTIST(Detector);

#endif
