#ifndef I3_SHOVEL_ARTIST_REGISTRY_H
#define I3_SHOVEL_ARTIST_REGISTRY_H

#include "ArtistFwd.h"
#include "Artist.h"
#include <boost/function.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <QMetaType>
#include <QObject>
#include <string>
#include <set>
#include <icetray/I3Logging.h>

struct ArtistFactory{
    ArtistPtr prototype;
    std::string description;
    unsigned keyCount;

    ArtistFactory(ArtistPtr artist):
        prototype( artist ),
        description( artist->description() ),
        keyCount( artist->numRequiredKeys() )
    {}

    boost::function< ArtistPtr( const std::vector<std::string>& ) > make;

    /** If the user loaded this python artist "externally" (i.e. it is not a default
     *  steamshovel artist), then this member contains the module path and class name
     *  of the artist.  Otherwise this member is empty.
     */
    std::pair< std::string, std::string > external_load_path;

    /// test identity based on description and number of keys
    bool operator==(const ArtistFactory &other) const {
        return (description==other.description && keyCount==other.keyCount);
    }

    /// do lexical sort on description, ignore case
    bool operator<(const ArtistFactory& other) const {
        const std::string& sa = description;
        const std::string& sb = other.description;
        for( int i = 0, n = std::min( sa.size(), sb.size() ); i < n; ++i ){
            char ac = tolower(sa[i]);
            char bc = tolower(sb[i]);
            if( ac > bc )
                return false;
            if( ac < bc )
                return true;
        }
        // if we arrive here, all chars up to n are equal
        if (sa.size() < sb.size())
            return true;
        return false;
    }
};

/**
 * registry class for managing Artist types on behalf of Scenario objects;
 */
class ArtistRegistry : public QObject {

    Q_OBJECT;
    SET_LOGGER("steamshovel");

    // keep a sorted collection of factories
    typedef std::multiset<ArtistFactory> ArtistFactoryList;
    ArtistFactoryList factories_;

    struct Extras {
        bool on;
        bool operator()( const ArtistFactory& f )const;
    } extras_;

    ArtistRegistry() { extras_.on = false; }
    ArtistRegistry( const ArtistRegistry& );
    ArtistRegistry& operator=( const ArtistRegistry& );

public:
    static ArtistRegistry& global();


    typedef boost::filter_iterator<
        Extras,
        ArtistFactoryList::const_iterator
    > const_iterator;

    const_iterator begin() { return const_iterator( extras_,
                                                    factories_.begin(),
                                                    factories_.end() ); }
    const_iterator end() { return const_iterator( extras_,
                                                  factories_.end(),
                                                  factories_.end() ); }
    unsigned size() const { return factories_.size() - 1 * !extras_.on; };

    const ArtistFactory& getFactory( const std::string& desc );

    // add the factory for the templated artist T; used for C++ artists
    template <typename T>
    void registerArtist(){
        ArtistPtr a = Artist::make<T>( ArtistKeyList() );
        ArtistFactory f(a);
        f.make = boost::function< ArtistPtr( const std::vector<std::string>& ) > ( &Artist::make<T> );
        factories_.insert(f);
    }

    /**
     * Fill the input list with all available python artists in
     * steamshovel.artists, as specified within its __init__.py.
     * May only be called after the Python interpreter has been
     * initialized.
     */
    void registerPythonArtists();

    // Add to the registry the python artist type at path.classname
    void registerExternalArtist( const char* path,
                                 const char* classname );

    void saveExternallyLoadedArtists();
    void restoreExternallyLoadedArtists();

public Q_SLOTS:
    void setExtras( bool on );

Q_SIGNALS:
    void artistRegistered( );
};

Q_DECLARE_METATYPE( ArtistPtr );
Q_DECLARE_METATYPE( const ArtistFactory* );

#endif
