#ifndef ICETRAY_PORTABLE_BINARY_IARCHIVE_HPP
#define ICETRAY_PORTABLE_BINARY_IARCHIVE_HPP

#include <streambuf>
#include <archive/basic_archive.hpp>
#include <archive/shared_ptr_helper.hpp>
#include <serialization/shared_ptr_helper.hpp>
#include <archive/detail/register_archive.hpp>
#include <archive/detail/common_iarchive.hpp>
#include <archive/detail/common_oarchive.hpp>
#include <archive/detail/iserializer.hpp>
#include <archive/detail/oserializer.hpp>
#include <serialization/is_bitwise_serializable.hpp>
#include <serialization/array.hpp>

#include <sys/types.h>
#include <cstring>

namespace icecube { namespace archive {

namespace portable {
	template <typename T>
	inline void swap(T& t) {
		#if BYTE_ORDER == BIG_ENDIAN
		T orig = t;
		uint8_t *bytes = reinterpret_cast<uint8_t *>(&t);
		uint8_t *origbytes = reinterpret_cast<uint8_t *>(&orig);
		for (size_t i = 0; i < sizeof(T); i++)
			bytes[i] = origbytes[(sizeof(T)-1) - i];
		#endif
	}
}

class portable_binary_oarchive;
class portable_binary_iarchive;


class portable_binary_oarchive :
    public icecube::archive::detail::common_oarchive<portable_binary_oarchive>,
    public icecube::archive::detail::shared_ptr_helper
{
	public:
		portable_binary_oarchive(std::ostream &stream,
		    unsigned int flags = icecube::archive::no_header) :
		    os (*stream.rdbuf()) {}

		template<class T>
		void save_override(const T &t, I3_PFTO int version) {
			this->detail::common_oarchive<portable_binary_oarchive>
			    ::save_override(t, static_cast<int>(version));
		}

		// binary files don't include the optional information
		void save_override(const class_id_optional_type & /* t */, int) {}

		// bool is a full byte
		void save_override(const bool& t, I3_PFTO int) {
			const uint8_t x = t;
			(*this) << x;
		}

		// the following have been overridden to provide specific sizes
		// for these pseudo-primitive types

		#define sv_override(a, b) \
			void save_override(const a& t, int) { \
				const b x = t; \
				(*this) << x; \
			}
		// make these as small as possible, even if they lose precision
		// (we have so many that file sizes increase a lot otherwise)
		void save_override(const version_type& t, I3_PFTO int);
		sv_override(class_id_type, int16_t)
		sv_override(class_id_reference_type, int16_t)
		sv_override(object_id_type, uint32_t)
		sv_override(object_reference_type, uint32_t)
		void save_override(const serialization::collection_size_type& t, I3_PFTO int);
		sv_override(tracking_type, char)
		#undef sv_override

		void save_override(const std::string& s, I3_PFTO int);
        #ifndef BOOST_NO_STD_WSTRING
		void save_override(const std::wstring& s, I3_PFTO int);
        #endif
		void save_override(const class_name_type& t, I3_PFTO int);

		struct use_array_optimization {
			template <class T> struct apply :
			    public icecube::serialization::
			    is_bitwise_serializable<T> {};
		};

		template <class T>
		void save_array(serialization::array<T> const &a, unsigned int v)
		{
			#if BYTE_ORDER == BIG_ENDIAN
			for (unsigned i = 0; i < a.count(); i++)
				save_override(a.address()[i], v);
			#else
			save_binary(a.address(), a.count()*sizeof(T));
			#endif
		}

		void save_array(serialization::array<char> const &a,
		    unsigned int) {
			save_binary(a.address(), a.count());
		}

		void save_binary(const void *address, size_t count) {
			std::streamsize s = os.sputn((char *)(address), count);
			if (s != (std::streamsize)count)
				icecube::serialization::throw_exception(
				    archive_exception(
				    archive_exception::output_stream_error
				    ));
		}

	private:
		friend class save_access;
		template<class T> void save(const T &t) {
			T st = t;
			portable::swap(st);
			save_binary(&st, sizeof(T));
		}

		std::streambuf &os;
};

class portable_binary_iarchive :
    public icecube::archive::detail::common_iarchive<portable_binary_iarchive>,
    public icecube::archive::detail::shared_ptr_helper
{
	public:
		portable_binary_iarchive(std::istream &stream,
		    unsigned int flags = icecube::archive::no_header) :
		    is (*stream.rdbuf()) {}

		template<class T>
		void load_override(T &t, I3_PFTO int version) {
			this->detail::common_iarchive<portable_binary_iarchive>
			    ::load_override(t, static_cast<int>(version));
		}

		// binary files don't include the optional information
		void load_override(class_id_optional_type & /* t */, int) {}

		// bool is a full byte
		void load_override(bool& t, I3_PFTO int) {
			uint8_t x = 0;
			(*this) >> x;
			t = (bool)x;
		}

		// the following have been overridden to provide specific sizes
		// for these pseudo-primitive types
		#define ld_override(a, b, c, d) \
			void load_override(a& t, int) { \
				d x = 0; (*this) >> x; \
				t = a((b)(c)(x)); \
			}

		// make these as small as possible, even if they lose precision
		// (we have so many that file sizes increase a lot otherwise)
		void load_override(version_type &t, int);
		ld_override(class_id_type, size_t, size_t,
		    int16_t)
		ld_override(class_id_reference_type, class_id_type, size_t,
		    int16_t)
		ld_override(object_id_type, unsigned int, unsigned int,
		    uint32_t)
		ld_override(object_reference_type, object_id_type, unsigned int,
		    uint32_t)
		#undef ld_override
		void load_override(serialization::collection_size_type &t, int);
		void load_override(tracking_type &t, int) {
			char x = 0; (*this) >> x;
			t = (x != 0);
		}
		void load_override(std::string& s, I3_PFTO int);
		#ifndef BOOST_NO_STD_WSTRING
		void load_override(std::wstring& s, I3_PFTO int);
		#endif
		void load_override(class_name_type& t, I3_PFTO int) {
			std::string cn;
			cn.reserve(I3_SERIALIZATION_MAX_KEY_SIZE);
			load_override(cn, 0);
			if (cn.size() > (I3_SERIALIZATION_MAX_KEY_SIZE - 1))
				boost::throw_exception(archive_exception(
				    archive_exception::invalid_class_name));
			memcpy(t, cn.data(), cn.size());
			t.t[cn.size()] = '\0';
		}

		struct use_array_optimization {
			template <class T> struct apply :
			    public icecube::serialization::
			    is_bitwise_serializable<T> {};
		};

		template <class T>
		void load_array(serialization::array<T> &a, unsigned int v) {
			#if BYTE_ORDER == BIG_ENDIAN
			for (unsigned i = 0; i < a.count(); i++)
				load_override(a.address()[i], v);
			#else
			load_binary(a.address(), a.count()*sizeof(T));
			#endif
		}

		void load_array(serialization::array<char> &a, unsigned int) {
			load_binary(a.address(), a.count());
		}

		void load_binary(void *address, size_t count) {
			std::streamsize s = is.sgetn((char *)(address), count);
			if (s != (std::streamsize)count)
				icecube::serialization::throw_exception(
				    archive_exception(
				    archive_exception::input_stream_error
				    ));
		}
	private:
		friend class load_access;
		template<class T> void load(T &t) {
			load_binary(&t, sizeof(T));
			portable::swap(t);
		}

		std::streambuf &is;
};

}} // end namespace

I3_SERIALIZATION_REGISTER_ARCHIVE(icecube::archive::portable_binary_iarchive)
I3_SERIALIZATION_USE_ARRAY_OPTIMIZATION(icecube::archive::portable_binary_iarchive)
I3_SERIALIZATION_REGISTER_ARCHIVE(icecube::archive::portable_binary_oarchive)
I3_SERIALIZATION_USE_ARRAY_OPTIMIZATION(icecube::archive::portable_binary_oarchive)

#endif // ICETRAY_PORTABLE_BINARY_IARCHIVE_HPP

