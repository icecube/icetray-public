#ifndef I3_SHOVEL_ARTIST_STATIC_IMAGE_H
#define I3_SHOVEL_ARTIST_STATIC_IMAGE_H

#include "shovelart/Artist.h"

/**
 * Artist for the displaying a static image, like the IceCube logo.
 * Good example of ImageOverlay use in C++.
 */
class StaticImage : public GLArtist{
public:
	StaticImage();

    ARTIST_REQUIRED_TYPES();

    void create( I3FramePtr ptr, SceneGroup* group, const SceneState& state );
};

REGISTER_ARTIST(StaticImage);

#endif
