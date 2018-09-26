#ifndef I3_SHOVEL_ARTIST_ICETOPVIEWER_H
#define I3_SHOVEL_ARTIST_ICETOPVIEWER_H

#include "shovelart/Artist.h"
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <boost/python/object_fwd.hpp>

class IceTopViewer: public WidgetArtist {
public:
    ARTIST_REQUIRED_TYPES( (I3Geometry)(I3FrameObject) );

    bool isValidKey( I3FramePtr ptr, int key_idx, const std::string& key );

    IceTopViewer();
    QWidget* create() const;
    void updateFrame( I3FramePtr );
    void updateColors( const TimeWindow& );
};

REGISTER_ARTIST(IceTopViewer);

#endif
