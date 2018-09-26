#ifndef I3_SHOVEL_KEY_SETTING_H
#define I3_SHOVEL_KEY_SETTING_H

#include "ComplexSetting.h"
#include <QString>
#include <QStringList>
#include <string>
#include <vector>
#include <algorithm>
#include <I3/name_of.h>
#include <icetray/I3Frame.h>
#ifndef Q_MOC_RUN
#include <boost/foreach.hpp>
#endif

/**
 * A setting for a key in an I3Frame that matches any of the stored
 * type names. Stored typne names should match what is returned by
 * I3::name_of(...).
 */
class KeySetting: public ComplexSetting<QString> {
    std::vector<std::string> type_name_;

public:
    typedef QString Value;

    // constructors for C++, we support up to five types
    template<typename T0,
             typename T1,
             typename T2,
             typename T3,
             typename T4>
    KeySetting( const QString& val = "" ) :
        ComplexSetting<QString>( val ), type_name_(5)
    {
        type_name_[0] = I3::name_of<T0>();
        type_name_[1] = I3::name_of<T1>();
        type_name_[2] = I3::name_of<T2>();
        type_name_[3] = I3::name_of<T3>();
        type_name_[4] = I3::name_of<T4>();
        std::sort(type_name_.begin(), type_name_.end());
    }

    template<typename T0,
             typename T1,
             typename T2,
             typename T3>
    KeySetting( const QString& val = "" ) :
        ComplexSetting<QString>( val ), type_name_(4)
    {
        type_name_[0] = I3::name_of<T0>();
        type_name_[1] = I3::name_of<T1>();
        type_name_[2] = I3::name_of<T2>();
        type_name_[3] = I3::name_of<T3>();
        std::sort(type_name_.begin(), type_name_.end());
    }

    template<typename T0,
             typename T1,
             typename T2>
    KeySetting( const QString& val = "" ) :
        ComplexSetting<QString>( val ), type_name_(3)
    {
        type_name_[0] = I3::name_of<T0>();
        type_name_[1] = I3::name_of<T1>();
        type_name_[2] = I3::name_of<T2>();
        std::sort(type_name_.begin(), type_name_.end());
    }

    template<typename T0,
             typename T1>
    KeySetting( const QString& val = "" ) :
        ComplexSetting<QString>( val ), type_name_(2)
    {
        type_name_[0] = I3::name_of<T0>();
        type_name_[1] = I3::name_of<T1>();
        std::sort(type_name_.begin(), type_name_.end());
    }

    template<typename T0>
    KeySetting( const QString& val = "" ) :
        ComplexSetting<QString>( val ), type_name_(1)
    {
        type_name_[0] = I3::name_of<T0>();
    }

    // constructors for Python (used in Types.cpp)
    KeySetting( const std::string& type_name, const QString& val = "" ) :
        ComplexSetting<QString>( val ), type_name_( 1, type_name )
    {}

    KeySetting( const std::vector<std::string>& type_names, const QString& val = "" ) :
        ComplexSetting<QString>( val ), type_name_( type_names )
    {
        std::sort(type_name_.begin(), type_name_.end());
    }

    // Default constructor is needed by QVariant
    KeySetting() :
        ComplexSetting<QString>( QString() ), type_name_()
    {}

    // get list of keys from frame that have matching types
    QStringList validKeys( I3FramePtr frame ) const {
        QStringList list;
        if( frame ){
            BOOST_FOREACH( const std::string& key, frame->keys() ){
                if( std::binary_search( type_name_.begin(),
                                        type_name_.end(),
                                        frame->type_name(key) ) )
                    list.push_back( QString::fromStdString(key) );
            }
        }
        return list;
    }

    // also behave like a std::string for C++ Setting users
    operator std::string() const { return QString(*this).toStdString(); }
};

#endif
