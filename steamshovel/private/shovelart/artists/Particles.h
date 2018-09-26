#ifndef I3_SHOVEL_ARTIST_PARTICLES_H
#define I3_SHOVEL_ARTIST_PARTICLES_H

#include "shovelart/Artist.h"

/**
 * Artist for I3Particles and collection of I3Particles.
 * Provides a semi-transparent Cherenkov cone and a
 * dynamic light front visualization.
 */
class Particles : public GLArtist{

    bool isValidKey( I3FramePtr ptr, int key_idx, const std::string& key );

    template <typename Container>
    void drawCollection( Container&, SceneGroup*, const SceneState&, bool, float, float );
    void drawParticle( const I3Particle&, SceneGroup*, const SceneState&, bool, float, float );

public:
    Particles();

    ARTIST_REQUIRED_TYPES( (I3FrameObject) );

    void create( I3FramePtr ptr, SceneGroup* group, const SceneState& state );
};

REGISTER_ARTIST(Particles);

#endif
