// =====================================================================
// Serviceroutines for geographic stuff
// (c) - 2015 A. Weidauer  alex.weidauer@huckfinn.de
// All rights reserved to A. Weidauer
// =====================================================================

#include "ifgdv/alg.h"
#include "ifgdv/util.h"

// ----------------------------------------------------------------
void igeo_open_raster_driver(const char * format,
                             bool can_read,
                             bool can_write,
                             bool can_copy,
                             igeo_raster_driver_t * result)
{

    if ( result == NULL )
        error_exit(10,"Access to an uninitialized "
        "igeo_raster_driver handle for format %s is forbidden!" ,format);


    // Assure that we've registered all drivers
    if ( GDALGetDriverCount() < 1 ) GDALAllRegister();

    result->driver = GDALGetDriverByName( format );
    if( result->driver == NULL )
        error_exit(11,"Driver %s not avialable!" ,format);

    // Get the metadata informations
    result-> info = GDALGetMetadata( result->driver, NULL );

    // ---------------------------------------------------------------
    // DCAP seems to be incomplete
    // CSLPrint(result->info, NULL);
    // Check we can create a new source
    // if( ! CSLFetchBoolean( result->info , GDAL_DCAP_RASTER, FALSE ) )
    //    error_exit(12,"Format %s is not a raster format" ,format);

    // Check we can read the format
    // if ( can_read )
    // {
    //    if( ! CSLFetchBoolean( result->info , GDAL_DCAP_OPEN, FALSE ) )
    //        error_exit(13,"Format %s is not readable" ,format);
    //    else
    //        result->can_read = true;
    // }
    // ---------------------------------------------------------------

    // Check we can copy the format
    if ( can_copy )
    {
        if( ! CSLFetchBoolean( result->info , GDAL_DCAP_CREATECOPY, FALSE ) )
            error_exit(14,"Format %s is not copyable" ,format);
        else
            result->can_copy = true;
    }

    // Check we can create the format
    if ( can_write )
    {
        if( ! CSLFetchBoolean( result->info , GDAL_DCAP_CREATE, FALSE ) )
            error_exit(15,"Format %s is not writeable" ,format);
        else
            result->can_write = true;
    }
}

// ----------------------------------------------------------------
void igeo_open_raster(const char * filename, bool readonly, igeo_raster_t * result)
{
    if ( result == NULL )
        error_exit(20,"Access to an uninitialized "
        "igeo_raster handle for file %s is forbidden!" ,filename);

    if ( readonly )
       result->data = GDALOpen( filename , GA_ReadOnly );
    else
       result->data = GDALOpen( filename , GA_Update );

    if( result->data == NULL )
       error_exit(21, "File %s is not accessible!\n", filename );

    if( GDALGetGeoTransform( result->data, result->trfm ) != CE_None )
       error_exit(23, "Transformation not avialable in %s \n", filename);

    result->proj_info = GDALGetProjectionRef( result->data);

    if ( result->proj_info == NULL)
        error_exit(24, "Missing spatial reference system in %s!\n",filename);

    result->srs  = OSRNewSpatialReference(result->proj_info);

    if ( result->srs == NULL)
        error_exit(25, "Spatial reference system %s in %s is invalid!\n",
                   result->srs, filename);

    result->num_cols  = GDALGetRasterXSize( result->data );
    if ( result->num_cols < 1)
        error_exit(26, "Image width for raster %s is invalid!\n",
                    filename);

    result->num_rows = GDALGetRasterYSize( result->data );
    if ( result->num_rows < 1)
        error_exit(27, "Image height for raster %s is invalid!\n",
                   filename);

    result->num_bands  = GDALGetRasterCount( result->data );
    if ( result->num_bands < 1)
        error_exit(27, "Number of bands for raster %s is invalid!\n",
                    filename);

    result->is_open = true;

    result->readonly = readonly;
}

// --------------------------------------------------------------
void igeo_close_raster(igeo_raster_t * result)
{
    if ( result == NULL )
        error_exit(40,"Access to an uninitialized "
        "igeo_raster handle is forbidden!");

    if ( result->data != NULL) GDALClose( result -> data);

    if ( result->srs != NULL) CPLFree(result->srs);

    result->num_bands = 0;

    result->num_cols = 0;

    result->num_rows = 0;

    for (int i=0; i<6; i++) result->trfm[i] = 0.0;

    result->is_open = false;

}

// -----------------------------------------------------------------------
void igeo_cut_raster(const igeo_raster_driver_t tool,
                const igeo_raster_t source, const char * filename,
                    int win_min_x, int win_min_y,
                    int win_max_x, int win_max_y,
                    igeo_raster_t * result) {

    // Check the outgoing memory
    if ( result == NULL )
        error_exit(30,"Access to an uninitialized "
        "igeo_raster handle for file %s is forbidden!" ,filename);

    // sort the window parameter
    sort_int( &win_min_x, &win_max_x);
    sort_int( &win_min_y, &win_max_y);

    // Check the boundaries of the pixel
    if ( win_min_x < 0) win_min_x = 0;
    if ( win_max_x < 0) win_max_x = 0;
    if ( win_min_y < 0) win_min_y = 0;
    if ( win_max_y < 0) win_max_y = 0;
    if ( win_min_x > source.num_cols-1)  win_min_x = source.num_cols-1;
    if ( win_max_x > source.num_cols-1)  win_max_x = source.num_cols-1;
    if ( win_min_y > source.num_rows-1) win_min_y = source.num_rows-1;
    if ( win_max_y > source.num_rows-1) win_max_y = source.num_rows-1;

    // Calculate the width and heigh of the new image
    int width = win_max_x - win_min_x;
    int height = win_max_y - win_min_y;

    // Prevent mal formed images
    if ( width < 1 )
        error_exit(31,"Cut window width is 0 for file %s!" ,filename);

    if ( height < 1 )
        error_exit(31,"Cut window height is 0 for file %s!" ,filename);

    // Get the sizes for the io buffer
    GDALDataType in_types [source.num_bands];
    int in_sizes [source.num_bands];
    for(int b=0 ; b < source.num_bands; b++) {
      GDALRasterBandH in_band = GDALGetRasterBand( source.data, b+1 );
      in_types[b] = GDALGetRasterDataType( in_band );
      int num_bits = GDALGetDataTypeSize( in_types[b] );
      int num_bytes = num_bits / 8;
      if ( num_bits % 8 > 0) num_bytes += 1;
      in_sizes[b] = num_bytes;
    }

    // Create a new raster file
    result->data = GDALCreate( tool.driver, filename,
                               width,  height,
                               source.num_bands,
                               in_types[0], NULL);

    // Create th new transformation
    double goffx = 0; double goffy = 0;
    trfm_pix_geo(source.trfm, win_min_x, win_min_y, &goffx, &goffy);

    result->trfm[0] = goffx;
    result->trfm[1] = source.trfm[1];
    result->trfm[2] = source.trfm[2];
    result->trfm[3] = goffy;
    result->trfm[4] = source.trfm[4];
    result->trfm[5] = source.trfm[5];

    // Set transformation an coordinate system
    GDALSetGeoTransform(result->data, result->trfm);
    GDALSetProjection(result->data, source.proj_info);

    // TRansfer the image data
    for (int b=0; b < source.num_bands; b++) {

      GDALRasterBandH in_band = GDALGetRasterBand( source.data, b+1 );

      // alloc io buffer for the copy @todo static
      void *io_buffer = CPLMalloc(in_sizes[b] * width * height);

      // copy pixels
      GDALRasterIO( in_band, GF_Read,
		    win_min_x, win_min_y, width, height,
		    io_buffer, width, height, in_types[b], 0, 0 );

      // address pixel to band handle of and copy the io buffer
      GDALRasterBandH out_band = GDALGetRasterBand(result->data, b+1);
      GDALRasterIO( out_band, GF_Write, 0, 0, width, height,
		    io_buffer, width, height, in_types[b], 0, 0 );
      // free io buffer
      CPLFree(io_buffer);
    }

    // Set the remaining parts for the raster
    result->proj_info = GDALGetProjectionRef(result->data);
    result->srs  = OSRNewSpatialReference(result->proj_info);
    result->num_bands = source.num_bands;
    result->num_cols = width;
    result->num_rows = height;
    result->readonly = false;
    result->is_open  = true;
}


// =====================================================================
// EOF
// =====================================================================
