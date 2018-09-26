#ifndef HDF5_OPT_H_INCLUDED
#define HDF5_OPT_H_INCLUDED

#include "hdf5.h"

#if (H5_VERS_MAJOR >= 1)&&(H5_VERS_MINOR >= 8)
	#include "hdf5_hl.h"
#else
    #include "H5LT.h"
	#include "H5TA.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

herr_t I3H5TBmake_table( const char *table_title,
                       hid_t loc_id,
                       const char *dset_name,
                       hsize_t nfields,
                       hsize_t nrecords,
                       size_t type_size,
                       const char *field_names[],
                       const size_t *field_offset,
                       const hid_t *field_types,
                       hsize_t chunk_size,
                       void *fill_data,
                       int compress,
                       const void *buf );
                       
#ifdef __cplusplus
}
#endif

#endif  // HDF5_OPT_H_INCLUDED
