#ifndef DATACLASSES_I3MATRIX_H_INCLUDED
#define DATACLASSES_I3MATRIX_H_INCLUDED

#include <icetray/I3FrameObject.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <serialization/serialization.hpp>
#include <serialization/array.hpp>
#include <serialization/collection_size_type.hpp>
#include <serialization/nvp.hpp>

class ublas_storage_shim :
	public boost::numeric::ublas::unbounded_array<double, std::allocator<double> > {
public:
	typedef boost::numeric::ublas::unbounded_array<double, std::allocator<double> > base;
	typedef base::allocator_type allocator_type;
	typedef base::size_type size_type;
	typedef base::difference_type difference_type;
	typedef base::value_type value_type;
	typedef base::const_reference const_reference;
	typedef base::reference reference;
	typedef base::const_pointer const_pointer;
	typedef base::pointer pointer;
	typedef base::const_iterator const_iterator;
	typedef base::iterator iterator;
	
	explicit ublas_storage_shim(const allocator_type& a=allocator_type()):base(a){}
	explicit ublas_storage_shim(size_type size, const allocator_type& a=allocator_type()):
	base(size,a){}
	ublas_storage_shim(size_type size, const value_type& init, const allocator_type& a=allocator_type()):
	base(size,init,a){}
	ublas_storage_shim(const base& b):base(b){}
private:
	friend class icecube::serialization::access;
	
	// Serialization
	// Copied from the base class in order to use icecube::serialization instead of boost
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /*version*/)
	{
		icecube::serialization::collection_size_type s(size());
		ar & icecube::serialization::make_nvp("size",s);
		if ( Archive::is_loading::value ) {
			resize(s);
		}
		ar & icecube::serialization::make_array(&this->operator[](0), s);
	}
};

class ublas_matrix_shim :
	public boost::numeric::ublas::matrix<double,boost::numeric::ublas::row_major,ublas_storage_shim> {
private:
	friend class icecube::serialization::access;
	// Serialization
	// Copied from the base class in order to use icecube::serialization instead of boost
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */){
		
		// we need to copy to a collection_size_type to get a portable
		// and efficient serialization
		icecube::serialization::collection_size_type s1 (size1());
		icecube::serialization::collection_size_type s2 (size2());
		
		// serialize the sizes
		ar & icecube::serialization::make_nvp("size1",s1)
		& icecube::serialization::make_nvp("size2",s2);
		
		// copy the values back if loading
		if (Archive::is_loading::value)
			resize(s1,s2,false);
		ar & icecube::serialization::make_nvp("data",data());
	}
protected:
	typedef boost::numeric::ublas::matrix<double,boost::numeric::ublas::row_major,ublas_storage_shim> base;
	ublas_matrix_shim(){}
	ublas_matrix_shim(const base& m) : base(m) {}
	ublas_matrix_shim(size_t size1, size_t size2) : base(size1, size2) {}
	ublas_matrix_shim(size_t size1, size_t size2, const base::value_type &init) : base(size1, size2, init) {}
	ublas_matrix_shim(size_t size1, size_t size2, const base::array_type &data) : base(size1, size2, data) {}
};

class I3Matrix : public I3FrameObject,
public ublas_matrix_shim {
//public boost::numeric::ublas::matrix<double> {
public:
	typedef ublas_matrix_shim::base base;
	//typedef boost::numeric::ublas::matrix<double> base;
	typedef base::size_type size_type;
	typedef base::difference_type difference_type;
	typedef base::value_type value_type;
	typedef base::const_reference const_reference;
	typedef base::reference reference;
	typedef base::array_type array_type;
	typedef base::const_closure_type const_closure_type;
	typedef base::closure_type closure_type;
	typedef base::vector_temporary_type vector_temporary_type;
	typedef base::matrix_temporary_type matrix_temporary_type;
	typedef base::storage_category storage_category;
	typedef base::orientation_category orientation_category;
	
	I3Matrix() {}
	I3Matrix(const base &m) : ublas_matrix_shim(m) {}
	I3Matrix(size_t size1, size_t size2) : ublas_matrix_shim(size1, size2) {}
	I3Matrix(size_t size1, size_t size2, const value_type &init) : ublas_matrix_shim(size1, size2, init) {}
	I3Matrix(size_t size1, size_t size2, const array_type &data) : ublas_matrix_shim(size1, size2, data) {}

	std::ostream& Print(std::ostream&) const override;
private:
	friend class icecube::serialization::access;
	template <typename Archive>
	void serialize(Archive &, unsigned);
};

std::ostream& operator<<(std::ostream&, const I3Matrix&);

I3_POINTER_TYPEDEFS(I3Matrix);
// I3_CLASS_VERSION(I3Matrix, 0);

#endif // DATACLASSES_I3MATRIX_H_INCLUDED
