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

#ifdef BOOST_SERIALIZATION_DEFAULT_TYPE_INFO
#error Included too late
#endif

#define BOOST_SERIALIZATION_DEFAULT_TYPE_INFO
#include <boost/serialization/extended_type_info_typeid.hpp>
#undef BOOST_SERIALIZATION_DEFAULT_TYPE_INFO

template<class T> struct i3_export_key_setter;

template<class T>
class i3_extended_type_info :
    public boost::serialization::typeid_system::extended_type_info_typeid_0,
    public boost::serialization::singleton<i3_extended_type_info< T > >
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
	    boost::serialization::typeid_system::extended_type_info_typeid_0(guid_buffer),
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
		BOOST_STATIC_WARNING(boost::is_polymorphic< T >::value);
		return boost::serialization::typeid_system::extended_type_info_typeid_0::get_extended_type_info(typeid(t));
	}

	virtual void * construct(unsigned int count, ...) const{
		// count up the arguments
		std::va_list ap;
		va_start(ap, count);

		switch(count){
		case 0:
			return boost::serialization::factory<BOOST_DEDUCED_TYPENAME boost::remove_const< T >::type, 0>(ap);
		case 1:
			return boost::serialization::factory<BOOST_DEDUCED_TYPENAME boost::remove_const< T >::type, 1>(ap);
		case 2:
			return boost::serialization::factory<BOOST_DEDUCED_TYPENAME boost::remove_const< T >::type, 2>(ap);
		case 3:
			return boost::serialization::factory<BOOST_DEDUCED_TYPENAME boost::remove_const< T >::type, 3>(ap);
		case 4:
			return boost::serialization::factory<BOOST_DEDUCED_TYPENAME boost::remove_const< T >::type, 4>(ap);
		default:
			BOOST_ASSERT(false); // too many arguments
			// throw exception here?
			return NULL;
		}
	}

	virtual void destroy(void const * const p) const {
		boost::serialization::access::destroy(
			static_cast<T const *>(p)
		);
		//delete static_cast<T const * const>(p);
	}
};


#define BOOST_SERIALIZATION_DEFAULT_TYPE_INFO
namespace boost {
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
		boost::serialization::singleton<i3_extended_type_info<T> >::get_mutable_instance().set_key(guid);
	}
};

#endif

