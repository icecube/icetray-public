#include <stdlib.h>
#include <string.h>
#include "hdf5_opt.h"

/*-------------------------------------------------------------------------
 * Function: H5TBmake_table
 *
 * Purpose: Make a table
 *
 * Return: Success: 0, Failure: -1
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *             Quincey Koziol
 *
 * Date: January 17, 2001
 *
 * Comments:
 *
 * Modifications:
 *
 * (Jakob van Santen <vansanten@wisc.edu>) 
 * - Pass compression level directly, rather than defaulting to 6 (which calls deflate_slow)
 * - Turn on shuffle filter by default when compression is enabled
 *-------------------------------------------------------------------------
 */

#define TABLE_CLASS         "TABLE"
#define TABLE_VERSION       "3.0"

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
                       const void *buf )
{
    
    hid_t   did;
    hid_t   sid;
    hid_t   mem_type_id;
    hid_t   plist_id;
    hsize_t dims[1];
    hsize_t dims_chunk[1];
    hsize_t maxdims[1] = { H5S_UNLIMITED };
    char    attr_name[255];
    char    *member_name;
    hid_t   attr_id;
    char    aux[255];
    hsize_t i;
    unsigned char *tmp_buf;
    
    dims[0]       = nrecords;
    dims_chunk[0] = chunk_size;
    
    /* create the memory data type. */
    if ((mem_type_id = H5Tcreate (H5T_COMPOUND, type_size )) < 0)
        return -1;
    
    /* insert fields. */
    for ( i = 0; i < nfields; i++)
    {
        if(H5Tinsert(mem_type_id, field_names[i], field_offset[i], field_types[i] ) < 0)
            return -1;
    }
    
    /* create a simple data space with unlimited size */
    if ((sid = H5Screate_simple( 1, dims, maxdims )) < 0)
        return -1;
    
    /* modify dataset creation properties, i.e. enable chunking  */
    plist_id = H5Pcreate(H5P_DATASET_CREATE);
    if (H5Pset_chunk(plist_id, 1, dims_chunk) < 0)
        return -1;
    
    /* set the fill value using a struct as the data type. */
    if (fill_data)
    {
        if(H5Pset_fill_value(plist_id, mem_type_id, fill_data) < 0)
            return -1;
    }
    
    /*
    dataset creation property list is modified to use
    GZIP compression with the compression effort set to the value passed by the user
    with the shuffle filter
    */
    if (compress)
    {
        if ( H5Pset_shuffle( plist_id ) < 0 )
             return -1;
        if ( H5Pset_deflate( plist_id, compress ) < 0)
            return -1;
    }
    
    /* create the dataset. */
    if ((did = H5Dcreate(loc_id, dset_name, mem_type_id, sid, plist_id)) < 0)
        goto out;
    
    /* only write if there is something to write */
    if (buf)
    {
        /* Write data to the dataset. */
        if (H5Dwrite( did, mem_type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf ) < 0)
            goto out;
    }
    
    /* terminate access to the data space. */
    if (H5Sclose(sid) < 0)
        goto out;
    
    /* end access to the dataset */
    if (H5Dclose(did) < 0)
        goto out;
    
    /* end access to the property list */
    if (H5Pclose(plist_id) < 0)
        goto out;
    
    /*-------------------------------------------------------------------------
    * set the conforming table attributes
    *-------------------------------------------------------------------------
    */
    
    /* attach the CLASS attribute */
    if (H5LTset_attribute_string( loc_id, dset_name, "CLASS", TABLE_CLASS ) < 0)
        goto out;
    
    /* attach the VERSION attribute */
    if (H5LTset_attribute_string( loc_id, dset_name, "VERSION", TABLE_VERSION ) < 0)
        goto out;
    
    /* attach the TITLE attribute */
    if (H5LTset_attribute_string( loc_id, dset_name, "TITLE", table_title ) < 0)
        goto out;
    
    /* attach the FIELD_ name attribute */
    for ( i = 0; i < nfields; i++)
    {        
        /* get the member name */
        member_name = H5Tget_member_name( mem_type_id,(unsigned) i );
        
        strcpy( attr_name, "FIELD_" );
        sprintf( aux, "%d", (int)i );
        strcat( attr_name, aux );
        sprintf( aux, "%s", "_NAME" );
        strcat( attr_name, aux );
        
        /* attach the attribute */
        if (H5LTset_attribute_string( loc_id, dset_name, attr_name, member_name ) < 0)
            goto out;
        
        free( member_name );
        
    }
    
    /* attach the FIELD_ fill value attribute */
    if (fill_data )
    {
        
        tmp_buf = fill_data;
        
        /* open the dataset. */
        if ((did = H5Dopen(loc_id, dset_name)) < 0)
            return -1;
        
        if (( sid = H5Screate(H5S_SCALAR)) < 0)
            goto out;
        
        for ( i = 0; i < nfields; i++)
        {
            
            /* get the member name */
            member_name = H5Tget_member_name(mem_type_id, (unsigned)i);
            
            strcpy(attr_name, "FIELD_");
            sprintf(aux, "%d", (int)i);
            strcat(attr_name, aux);
            sprintf(aux, "%s", "_FILL");
            strcat(attr_name, aux);
            
            if ((attr_id = H5Acreate(did, attr_name, field_types[i], sid, H5P_DEFAULT)) < 0)
                goto out;
            
            if (H5Awrite(attr_id, field_types[i], tmp_buf+field_offset[i]) < 0)
                goto out;
            
            if (H5Aclose(attr_id) < 0)
                goto out;
            
            free(member_name);
        }
        
        /* terminate access to the data space. */
        if (H5Sclose(sid) < 0)
            goto out;
        
        /* end access to the dataset */
        if (H5Dclose(did) < 0)
            goto out;
    }
    
    /* release the datatype. */
    if (H5Tclose( mem_type_id ) < 0)
        return -1;
    
    return 0;
    
    /* error zone */
out:
    H5E_BEGIN_TRY {
        H5Dclose(did);
        H5Sclose(sid);
        H5Pclose(plist_id);
        H5Tclose(mem_type_id);
    } H5E_END_TRY;
    return -1;
    
}
