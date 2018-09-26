#ifndef I3_SHOVEL_RENDERING_ARTIST_H
#define I3_SHOVEL_RENDERING_ARTIST_H

// needs to be before any 'boost/python*' or else triggers https://bugs.python.org/issue10910
#include <icetray/I3Logging.h>

#include <boost/any.hpp>
#include <boost/python/object_fwd.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/seq.hpp>

#include <string>
#include <vector>
#include <utility>
#include <typeinfo>

#include <icetray/I3Frame.h>
#include <icetray/I3FrameObject.h>
#include <icetray/name_of.h>

#include "ArtistFwd.h"
#include "RangeSetting.h"
#include "ChoiceSetting.h"
#include "KeySetting.h"
#include "FileSetting.h"
#include "OMKeySet.h"

class Scene;
class SceneGroup;
class SceneState;
class TimeWindow;
class QStackedWidget;
class QWidget;

class Artist{
public:
    SET_LOGGER("Artist");

    class Setting{
        boost::any value_;
        Artist* owner_;

    public:
        Setting( Artist* owner ) : owner_(owner){}
        Setting( Artist* owner, boost::any value ) :
            value_(value), owner_(owner) {}

        // for POD types
        template <typename T>
        void setValue( const T& value )
        {
            value_ = boost::any(value);
            owner_->settings_valid_ = false;
        }

        // for types that derive from ComplexSetting
        template <typename T>
        void setValueOfComplex( typename T::Value value )
        {
            boost::any_cast<T&>( value_ ).setValue( value );
            owner_->settings_valid_ = false;
        }

        // returns the value for POD and the Complex for ComplexSettings
        template <typename T>
        const T& value() const
        {
            return boost::any_cast<const T&>(value_);
        }

        // returns the value for ComplexSettings
        template <typename T>
        typename T::Value valueFromComplex() const
        {
            return boost::any_cast<const T&>(value_);
        }

        template <typename T>
        bool is() const
        {
#if __cplusplus < 201103L
            return value_.type() == typeid(T);
#else
            return value_.type().hash_code() == typeid(T).hash_code();
#endif
        }

        bool empty() const {
            return value_.empty();
        }
    };

    typedef std::vector< std::string > typelist;

public:
    /* Artist keeps settings in the order of items inserted

       Attempts to use a boost::multi_index_container here to
       get order-preserving iteration and fast O(1) lookup
       failed miserably (you are invited to try, dear wanderer),
       that's why we have this home-cooked solution doing linear
       searches. The O(N) performance is okay, since the lists
       are short and the searches are not done very frequently.
    */
    class SettingsMap {
    public:
        struct Item { std::string name; Setting setting; };
    private:
        std::vector< Item > items_;
    public:
        typedef std::vector< Item >::const_iterator
            const_iterator;
        typedef std::vector< Item >::iterator
            iterator;
        const_iterator begin() const { return items_.begin(); }
        const_iterator end() const { return items_.end(); }
        iterator begin() { return items_.begin(); }
        iterator end() { return items_.end(); }

        const_iterator find( const std::string& name ) const
        { 
            const_iterator it = begin();
            for( ; it != end(); ++it )
                if( it->name == name )
                    break;
            return it;
        }

        iterator find( const std::string& name )
        { 
            iterator it = begin();
            for( ; it != end(); ++it )
                if( it->name == name )
                    break;
            return it;
        }

        const Setting& operator[]( const std::string& name )const{
            const_iterator iter = find( name );
            if( iter == end() )
                throw std::runtime_error( "No such setting '" + name + "'" );
            return iter->setting;
        }

        Setting& operator[]( const std::string& name ){
            iterator iter = find( name );
            if( iter == end() )
                throw std::runtime_error( "No such setting '" + name + "'" );
            return iter->setting;
        }

        void push_back( const std::string& name, const Setting& setting ){
            Item item = { name, setting };
            items_.push_back( item );
        }

        unsigned size() const { return items_.size(); }
    };

    const SettingsMap& settings()const{ return settings_; }

    // factory function for C++ artists, used by ArtistFactory
    template< typename T>
    static
    ArtistPtr make( const ArtistKeyList& keys ){
        ArtistPtr a(new T);
        a->keys_ = keys;
        log_trace_stream("Artist " << icetray::name_of<T>() << " constructed");
        return a;
    }

    // factory function for Python artists, used by ArtistFactory
    static
    ArtistPtr pymake( const boost::python::object& artist_type,
                      const ArtistKeyList& keys );

    virtual std::string description() = 0;
    virtual unsigned numRequiredKeys(){ return requiredTypes().size(); }
    const ArtistKeyList& keys() const { return keys_; }
    bool isActive() const { return active_; }

    // convenience function
    template < typename T >
    T setting( const std::string& name )const{
        return settings_[name].value<T>();
    }

private:
    // to be used by Scenario class, only Scenario is
    // allowed to change the state of an Artist
    SettingsMap settings_;
    ArtistKeyList keys_;
    typedef std::vector< I3FrameObjectConstPtr > sourcelist;
    sourcelist sources_;

    bool settings_valid_;
    bool active_;

protected:
    // ctor protected, artists should be created by make(...)
    Artist() : settings_valid_(false), active_(true) {}
    virtual ~Artist() { log_trace("Artist destroyed"); }

    virtual bool isValidKey( I3FramePtr ptr, int key_idx, const std::string& key );
    virtual typelist requiredTypes() = 0;

    // convenience function
    template <typename T>
    void addSetting( const std::string& name, const T& value )
    { addSetting( name, Setting(this, value) ); }

    void addSetting( const std::string& name, const Setting& s )
    { settings_.push_back( name, s ); }

    virtual void cleanUp() {} // some artists need to perform cleanup actions

    friend class Scenario;
};

#include "ArtistRegistry.h" // must be after declaration of Artist

/// Artist that draws on the GLWidget
class GLArtist: public Artist {
    friend class Scenario;

    // to be implemented by concrete GLWidgets
    virtual void create( I3FramePtr, SceneGroup*, const SceneState& ) = 0;    
};

/// Artist that manages a completely new QWidget
class WidgetArtist: public Artist {
    friend class Scenario;
    QWidget* widget_;

    // to be implemented by concrete WidgetArtists
    virtual QWidget* create() const = 0;
    virtual void updateFrame( I3FramePtr ) = 0;
    virtual void updateColors( const TimeWindow& ) {}
    virtual void updateTime( int ) {}

protected:
    template<typename Widget>
    Widget&
    widget(){ return dynamic_cast<Widget&>(*widget_); }

    WidgetArtist(): widget_(0) {}
    virtual ~WidgetArtist() {} // widget_ is deleted elsewhere
};

/**
 * Macro for C++ artist to specify their required types.
 * See examples within artists directory.
 */
#define ARTIST_SEQ_TYPES( r, u, T ) t.push_back( I3::name_of<T>() );

#define ARTIST_REQUIRED_TYPES( SEQ )                            \
    virtual std::string description();                          \
    virtual typelist requiredTypes(){                           \
        typelist t;                                             \
        BOOST_PP_SEQ_FOR_EACH( ARTIST_SEQ_TYPES, u, SEQ );      \
        return t;                                               \
    }

/**
 * Macro for C++ artist that registers it in the system.
 * It sets the description and generates a small struct which
 * calls the ArtistRegistry with this class at start up.
 * See examples within artists directory.
 */
#define REGISTER_ARTIST( T )                              \
    std::string T::description() { return #T; }           \
    struct RegisterArtist##T {                            \
        RegisterArtist##T() {                             \
            ArtistRegistry::global().registerArtist<T>(); \
        }                                                 \
    } registerArtist##T

#endif
