#ifndef I3_SHOVEL_ARTIST_TIME_LABEL_H
#define I3_SHOVEL_ARTIST_TIME_LABEL_H

#include "shovelart/Artist.h"

/**
 * Textual label artist displaying the current visible time.
 * Settings for font and font size
 */
class TimeLabel: public GLArtist {
public:
	TimeLabel();

    ARTIST_REQUIRED_TYPES();

	void create( I3FramePtr ptr, SceneGroup* group, const SceneState& state );
};

REGISTER_ARTIST(TimeLabel);

#endif
