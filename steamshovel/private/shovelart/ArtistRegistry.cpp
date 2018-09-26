#include "Artist.h"
#include "ArtistRegistry.h"
#include "moc_ArtistRegistry.cpp"

#include <QString>
#include <QStringList>
#include <QSettings>

#include <boost/python.hpp>

#include "scripting/gil.h"
#include "scripting/pyerr.h"
#include <iostream>

namespace {

    /**
     * Create a new ArtistFactory for the python artist at the module_path.class_name
     * This function assumes the GIL is locked.
     * bp::error_already_set may arise for various reasons.
     */
    ArtistFactory makePyArtistFactory( const char* module_path,
                                       const char* class_name ){
        log_trace_stream("module path " << module_path << ", class " << class_name);

        using namespace boost::python;
        object artmod = import( module_path );
        object arttype = artmod.attr(class_name);
        ArtistPtr a = Artist::pymake( arttype, ArtistKeyList() );
        ArtistFactory factory(a);
        factory.make = boost::bind( Artist::pymake, arttype, _1 );
        return factory;
    }

} // namespace


ArtistRegistry& ArtistRegistry::global()
{
    // prevent destructor call of ArtistRegistry, which otherwise
    // corrupts shared memory management between C++ and Python 
    static ArtistRegistry* reg = new ArtistRegistry;
    return *reg;
}

void ArtistRegistry::registerPythonArtists()
{
    using namespace boost::python;
    scripting::ScopedGIL gil;
    log_debug( "Gathering python artists... " );
    try{
        object module = import("icecube.steamshovel.artists");
        object all = module.attr("__all__");
        for( unsigned i = 0, n = len(all); i < n; ++i ){
            object pyname = all[i];
            std::string name = extract<std::string>(pyname);
            log_debug("  Setting up python artist '%s'", name.c_str() );

            try{
                ArtistFactory fact =
                    makePyArtistFactory( ("icecube.steamshovel.artists."+name).c_str(),
                                         name.c_str() );
                factories_.insert( fact );
            }
            catch( error_already_set& e ){
                // Don't complain if the reason the python artist failed was an import error,
                // because that's probably due to it being in the wrong metaproject

                PyObject *err, *val, *traceback;
                PyErr_Fetch( &err, &val, &traceback );

                if( PyErr_GivenExceptionMatches(err, PyExc_ImportError) ){
                    std::string message = "(null msg)";
                    extract<std::string> get_str(val);
                    if (get_str.check())
                        message = get_str();
                    log_debug_stream( "PyArtist " << name << " deactivated, missing imports: " << message );
                    Py_XDECREF( err );
                    Py_XDECREF( val );
                    Py_XDECREF( traceback );
                }
                else{
                    // If it was indeed some other error, then whine
                    PyErr_Restore( err, val, traceback ); // decrements our references
                    log_warn_stream( "PyArtist " << name <<
                                     " cannot be used for the following reason:\n"
                                     << e );
                }
            }
        }
    }
    catch( error_already_set& e ){
        log_error_stream( "Unexpected problem setting up python artists, "
                          "exception trace follows:\n" << e );
    }
}

void ArtistRegistry::registerExternalArtist( const char* path,
                                             const char* classname )
{
    scripting::ScopedGIL gil;
    try{
        ArtistFactory fact = makePyArtistFactory( path, classname );
        fact.external_load_path = std::make_pair( std::string(path),
                                                  std::string(classname) );
        ArtistFactoryList::iterator pos = factories_.find( fact );
        if( pos == factories_.end() ){
            factories_.insert( fact );
        }
        else{
            // *pos is an existing Artist with name and key arity equal to artist
            bool old_artist_is_builtin = 
                (pos->external_load_path.first.length() == 0 &&
                 pos->external_load_path.second.length() == 0);
            bool paths_differ = (pos->external_load_path != fact.external_load_path);
            if( old_artist_is_builtin ){
                log_error( "Cannot register new artist: it would replace built-in "
                           "artist '%s' with %d keys. Please rename your artist.",
                           fact.description.c_str(), fact.keyCount );
            }
            else if( paths_differ ){
                log_warn( "Newly registered artist '%s' is replacing "
                          "identically-named artist %s from module %s",
                          fact.description.c_str(),
                          pos->external_load_path.first.c_str(),
                          pos->external_load_path.second.c_str() );
                factories_.erase( pos );
                factories_.insert( fact );
            }
            else{
                log_warn( "Re-registering an artist has no effect. "
                          "Use reload() on its python module if you "
                          "want to re-read it from disk" );
            }
        }
        Q_EMIT artistRegistered( );
    }
    catch( boost::python::error_already_set& e ){
        log_warn_stream( "Externally loaded PyArtist" << path << "." << classname
                         << " cannot be used because:\n" << e << "\n"
                         << "This artist type will not appear in this or future steamshovel sessions." );
    }
}

void ArtistRegistry::saveExternallyLoadedArtists(){
    log_debug( "Saving externally loaded python artists" );
    QStringList save;
    for( ArtistFactoryList::iterator i = factories_.begin();
         i != factories_.end(); ++i )
    {
        const std::pair< std::string, std::string >& loadpath = i->external_load_path;
        if( loadpath.first == "" || loadpath.second == "" ) continue;
        log_trace_stream( "    Remembering " << loadpath.first << "." << loadpath.second );
        QString path = QString::fromStdString( loadpath.first );
        QString classname = QString::fromStdString( loadpath.second );
        save.append( path ); save.append(classname);
    }
    QSettings settings;
    settings.setValue( "ArtistRegistry/externals", save );
}

void ArtistRegistry::restoreExternallyLoadedArtists(){
    log_debug( "Restoring externally loaded python artists" );
    QSettings settings;
    QVariant dat = settings.value( "ArtistRegistry/externals" );
    QStringList items = dat.toStringList();
    for( QStringList::iterator i = items.begin(); i != items.end(); i+=2 ){
        std::string path = i->toStdString();
        std::string classname = (i+1)->toStdString();
        log_trace_stream( "Restoring " << path << "." << classname );
        registerExternalArtist( path.c_str(), classname.c_str() );
    }
}

const ArtistFactory& ArtistRegistry::getFactory( const std::string& desc ){
    ArtistFactoryList::iterator factory;
    for( factory = factories_.begin(); factory != factories_.end(); ++factory ){
        if( factory->description == desc ){
            break;
        }
    }
    if( factory == factories_.end() ){
        throw std::runtime_error( "No such factory name: '" + desc  + "'");
    }
    return *factory;
}

void ArtistRegistry::setExtras( bool on ){
    extras_.on = on;
    Q_EMIT artistRegistered();
}

bool ArtistRegistry::Extras::operator()( const ArtistFactory& f )const{
    if( on )
        return true;
    return f.description != "Pong";
}
