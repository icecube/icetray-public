
#include <icetray/serialization.h>
#include <dataclasses/I3Matrix.h>

std::ostream& I3Matrix::Print(std::ostream& oss) const{
	oss << "I3Matrix:\n";
	for(size_t i=0; i<size1(); i++){
		oss << (i>0 ? " [" : "[[");
		for(size_t j=0; j<size2(); j++){
			oss << (*this)(i,j);
			if(j<size2()-1)
				oss << ", ";
		}
		oss << (i<size1()-1 ? "]\n" : "]]");
	}
	return oss;
}

std::ostream& operator<<(std::ostream& os, const I3Matrix& m){
	return(m.Print(os));
}

template <typename Archive>
void
I3Matrix::serialize(Archive &ar, unsigned int version)
{	
	ar & icecube::serialization::make_nvp("I3FrameObject", icecube::serialization::base_object<I3FrameObject>(*this));
	ar & icecube::serialization::make_nvp("Matrix", icecube::serialization::base_object<ublas_matrix_shim>(*this));
}

I3_SERIALIZABLE(ublas_storage_shim);
I3_SERIALIZABLE(ublas_matrix_shim);
I3_SERIALIZABLE(I3Matrix);
