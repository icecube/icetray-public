#ifndef I3_SHOVEL_RENDERING_ARTIST_FWD_H
#define I3_SHOVEL_RENDERING_ARTIST_FWD_H

#include <icetray/I3PointerTypedefs.h>

#include <vector>
#include <string>

// forward declaration of Artist and ArtistPtr
class Artist;
I3_POINTER_TYPEDEFS( Artist );
typedef std::vector<std::string> ArtistKeyList;

// use there in all moc'd headers to avoid a conflict between boost and moc

#endif /* I3_SHOVEL_RENDERING_ARTIST_FWD_H */
