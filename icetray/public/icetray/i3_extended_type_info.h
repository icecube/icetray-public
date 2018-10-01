/**
 *  $Id$
 *  
 * (C) Copyright 2002 Robert Ramey - http://www.rrsd.com .
 * Use, modification and distribution is subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 *  See http://www.boost.org for updates, documentation, and revision history.
 *  
 */
#ifndef ICETRAY_EXTENDED_TYPE_INFO_H_INCLUDED
#define ICETRAY_EXTENDED_TYPE_INFO_H_INCLUDED

#ifdef I3_SERIALIZATION_DEFAULT_TYPE_INFO
#error Included too late
#endif

#include <cstring>
#include <cassert>

#define I3_SERIALIZATION_DEFAULT_TYPE_INFO
#include <serialization/extended_type_info_typeid.hpp>
#undef I3_SERIALIZATION_DEFAULT_TYPE_INFO

template<class T> struct i3_export_key_setter;

template<class T>
class i3_extended_type_info :
    public icecube::serialization::typeid_system::extended_type_info_typeid_0,
    public icecube::serialization::singleton<i3_extended_type_info< T > >
{
private:
	friend struct i3_export_key_setter<T>;

	char guid_buffer[255];
	bool registered;

	void set_key(const char *guid) {
		strncpy(guid_buffer, guid, 254);
		guid_buffer[254] = 0;
		assert(!registered);
		key_register();
		registered = true;
	}
	
public:
	i3_extended_type_info() :
	    icecube::serialization::typeid_system::extended_type_info_typeid_0(guid_buffer),
	    registered(false)
	{
		type_register(typeid(T));
	}

	~i3_extended_type_info() {
		if (registered)
			key_unregister();
		type_unregister();
	}

	const extended_type_info *
	get_derived_extended_type_info(const T & t) const {
		// note: this implementation - based on usage of typeid (rtti)
		// only does something if the class has at least one virtual
		// function.
#ifdef BOOST_STATIC_ASSERT_MSG
		BOOST_STATIC_ASSERT_MSG(boost::is_polymorphic< T >::value, "Serializable types should be polymorphic");
#else
		BOOST_STATIC_ASSERT(boost::is_polymorphic< T >::value && "Serializable types should be polymorphic");
#endif
		return icecube::serialization::typeid_system::extended_type_info_typeid_0::get_extended_type_info(typeid(t));
	}

	virtual void * construct(unsigned int count, ...) const{
		// count up the arguments
		std::va_list ap;
		va_start(ap, count);

		switch(count){
		case 0:
			return icecube::serialization::factory<BOOST_DEDUCED_TYPENAME boost::remove_const< T >::type, 0>(ap);
		case 1:
			return icecube::serialization::factory<BOOST_DEDUCED_TYPENAME boost::remove_const< T >::type, 1>(ap);
		case 2:
			return icecube::serialization::factory<BOOST_DEDUCED_TYPENAME boost::remove_const< T >::type, 2>(ap);
		case 3:
			return icecube::serialization::factory<BOOST_DEDUCED_TYPENAME boost::remove_const< T >::type, 3>(ap);
		case 4:
			return icecube::serialization::factory<BOOST_DEDUCED_TYPENAME boost::remove_const< T >::type, 4>(ap);
		default:
			BOOST_ASSERT(false); // too many arguments
			// throw exception here?
			return NULL;
		}
	}

	virtual void destroy(void const * const p) const {
		icecube::serialization::access::destroy(
			static_cast<T const *>(p)
		);
		//delete static_cast<T const * const>(p);
	}
};


#define I3_SERIALIZATION_DEFAULT_TYPE_INFO
namespace icecube {
namespace serialization {
template<class T>
struct extended_type_info_impl {
typedef ::i3_extended_type_info< T > type;
};
} // namespace serialization
} // namespace boost

template<class T>
struct i3_export_key_setter {
	const char *guid;
	i3_export_key_setter(const char *name) : guid(name) {
		icecube::serialization::singleton<i3_extended_type_info<T> >::get_mutable_instance().set_key(guid);
	}
};

#endif
