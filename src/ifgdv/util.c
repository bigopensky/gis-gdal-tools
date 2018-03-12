// =====================================================================
// Serviceroutines for geographic stuff
// (c) - 2015 A. Weidauer  alex.weidauer@huckfinn.de
// All rights reserved to A. Weidauer
// =====================================================================

#include "ifgdv/error.h"
#include "ifgdv/alg.h"
#include "ifgdv/util.h"

static bool gistk_raster_driver_loaded = false;
static bool gistk_vector_driver_loaded = false;
static int  gistk_debug_mode = 0;

void gistk_init(bool use_raster, bool use_vector) {

    if ( use_raster )
    {
        GDALAllRegister();
        gistk_raster_driver_loaded = ( GDALGetDriverCount() > 0 );
        if ( ! gistk_raster_driver_loaded )
            gistk_error_fatal(GISTK_ERRC_GDAL_LOAD_DRVS,
                              GISTK_ERRS_GDAL_LOAD_DRVS, 0);
        if (gistk_debug_mode == 1)
          printf("# %d RASTER DRIVER REGISTERD %d\n",
                 GDALGetDriverCount(), gistk_raster_driver_loaded);
    }

    if ( use_vector )
    {
        OGRRegisterAll();
        gistk_vector_driver_loaded = ( OGRGetDriverCount() > 0 );
        if ( ! gistk_vector_driver_loaded )
            gistk_error_fatal(GISTK_ERRC_OGR_LOAD_DRVS,
                              GISTK_ERRS_OGR_LOAD_DRVS, 0);
    }
}

// ----------------------------------------------------------------
void gistk_check_raster_driver_base() {
    // printf("# DRIVER REGISTERD? %d\n", gistk_raster_driver_loaded);
    if ( ! gistk_raster_driver_loaded )
            gistk_error_fatal(GISTK_ERRC_GDAL_INIT_DRVS,
                              GISTK_ERRS_GDAL_INIT_DRVS, 0);
}

// ----------------------------------------------------------------
void gistk_check_raster_driver_init(int err_source,
                       const char * format,
                       gistk_raster_driver_t * result)
{
    if ( result == NULL )
        gistk_error_fatal(err_source,
                          GISTK_ERRS_OPEN_DRV_INIT,
                          format);
}

// ----------------------------------------------------------------
void gistk_open_raster_driver(const char * format,
                             bool can_read,
                             bool can_write,
                             bool can_copy,
                             gistk_raster_driver_t * result)
{

    // Assert that we've registered all drivers
    gistk_check_raster_driver_base();

    // Check the memory validity of the result object
    gistk_check_raster_driver_init(GISTK_ERRC_OPEN_DRV_INIT, format, result);

    result->driver = GDALGetDriverByName( format );
    if( result->driver == NULL )
            gistk_error_fatal( GISTK_ERRC_OPEN_DRV_VALID,
                               GISTK_ERRS_OPEN_DRV_VALID, format );

    // Get the metadata informations
    result-> info = GDALGetMetadata( result->driver, NULL );
    if( result->info == NULL )
            gistk_error_fatal( GISTK_ERRC_OPEN_DRV_INFO,
                               GISTK_ERRS_OPEN_DRV_INFO, format );

    // ---------------------------------------------------------------
    // DCAP seems to be incomplete
    // CSLPrint(result->info, NULL);
    // Check we can create a new source
    // if( ! CSLFetchBoolean( result->info , GDAL_DCAP_RASTER, FALSE ) )
    //        gistk_error_fatal( GISTK_ERRC_OPEN_DRV_ISRST,
    //                           GISTK_ERRS_OPEN_DRV_ISRST, format );


    // Check we can read the format
    // if ( can_read )
    // {
    //    if( ! CSLFetchBoolean( result->info , GDAL_DCAP_OPEN, FALSE ) )
    //        gistk_error_fatal( GISTK_ERRC_OPEN_DRV_ISREAD,
    //                           GISTK_ERRS_OPEN_DRV_ISREAD, format );
    //    else
    //        result->can_read = true;
    // }
    // ---------------------------------------------------------------

    // Check we can copy the format
    if ( can_copy )
    {
        if( ! CSLFetchBoolean( result->info , GDAL_DCAP_CREATECOPY, FALSE ) )
            gistk_error_fatal( GISTK_ERRC_OPEN_DRV_ISCOPY,
                               GISTK_ERRS_OPEN_DRV_ISCOPY, format );
        else
            result->can_copy = true;
    }

    // Check we can create the format
    if ( can_write )
    {
        if( ! CSLFetchBoolean( result->info , GDAL_DCAP_CREATE, FALSE ) )
            gistk_error_fatal( GISTK_ERRC_OPEN_DRV_ISWRITE,
                               GISTK_ERRS_OPEN_DRV_ISWRITE, format );
        else
            result->can_write = true;
    }
}

// ----------------------------------------------------------------
void gistk_check_raster_init(int err_source,
                       const char * filename,
                       gistk_raster_t * result)
{
    if ( result == NULL )
        gistk_error_fatal(err_source,
                          GISTK_ERRS_OPEN_RST_INIT,
                          filename);
}


// ----------------------------------------------------------------
void gistk_open_raster(const char * filename, bool readonly, gistk_raster_t * result)
{

    // Assert that we've registered all drivers
    gistk_check_raster_driver_base();

    // Check the memory validity of the result object
    gistk_check_raster_init(GISTK_ERRC_OPEN_RST_INIT, filename, result);

    // Get the data source and check the results for
    // the read and write case.
    if ( readonly )
    {
       result->data = GDALOpen( filename , GA_ReadOnly );
       if( result->data == NULL )
           gistk_error_fatal(GISTK_ERRC_OPEN_RST_SRCR,
                             GISTK_ERRS_OPEN_RST_SRC,
                             filename, "readable");
    }
    else
    {
       result->data = GDALOpen( filename , GA_Update );
       if( result->data == NULL )
           gistk_error_fatal(GISTK_ERRC_OPEN_RST_SRCW,
                             GISTK_ERRS_OPEN_RST_SRC,
                             filename, "read- or writable");
    }

    // Get the affine transformation parameter and check the results
    if( GDALGetGeoTransform( result->data, result->trfm ) != CE_None )
        gistk_error_fatal(GISTK_ERRC_OPEN_RST_TRFM,
                          GISTK_ERRS_OPEN_RST_TRFM,
                          filename);

    // Get the string representation of the spatial reference
    // system and check the results
    result->proj_info = GDALGetProjectionRef( result->data);
    if ( result->proj_info == NULL)
        gistk_error_fatal(GISTK_ERRC_OPEN_RST_SRS,
                          GISTK_ERRS_OPEN_RST_SRS,
                          filename);


    result->srs  = OSRNewSpatialReference(result->proj_info);
    if ( result->srs == NULL)
        gistk_error_fatal(GISTK_ERRC_OPEN_RST_SRS,
                          GISTK_ERRS_OPEN_RST_SRS,
                          filename, result->proj_info);

    result->num_cols  = GDALGetRasterXSize( result->data );
    if ( result->num_cols < 1)
        gistk_error_fatal(GISTK_ERRC_OPEN_RST_WIDTH,
                          GISTK_ERRS_OPEN_RST_WIDTH,
                          filename);

    result->num_rows = GDALGetRasterYSize( result->data );
    if ( result->num_rows < 1)
        gistk_error_fatal(GISTK_ERRC_OPEN_RST_HEIGHT,
                          GISTK_ERRS_OPEN_RST_HEIGHT,
                          filename);

    result->num_bands  = GDALGetRasterCount( result->data );
    if ( result->num_bands < 1)
        gistk_error_fatal(GISTK_ERRC_OPEN_RST_NBAND,
                          GISTK_ERRS_OPEN_RST_NBAND,
                          filename);

    result->is_open = true;

    result->readonly = readonly;
}

// --------------------------------------------------------------
void gistk_close_raster(gistk_raster_t * result)
{
    // Check the memory validity of the result object
    if ( result == NULL )
        gistk_error_fatal(GISTK_ERRC_CLOSE_RST,
                          GISTK_ERRS_CLOSE_RST,
                          0);

    if ( result->data != NULL) GDALClose( result -> data);

    if ( result->srs != NULL) CPLFree(result->srs);

    result->num_bands = 0;

    result->num_cols = 0;

    result->num_rows = 0;

    for (int i=0; i<6; i++) result->trfm[i] = 0.0;

    result->is_open = false;

}

// -----------------------------------------------------------------------
void gistk_cut_raster(const gistk_raster_driver_t tool,
                const gistk_raster_t source, const char * filename,
                    int win_min_x, int win_min_y,
                    int win_max_x, int win_max_y,
                    gistk_raster_t * result) {

    // Check the memory validity of the result object
    gistk_check_raster_init(GISTK_ERRC_CUT_RST_INIT, filename, result);

    // sort the window parameter
    sort_int( &win_min_x, &win_max_x);
    sort_int( &win_min_y, &win_max_y);

    // Check the boundaries of the pixel
    if ( win_min_x < 0) win_min_x = 0;
    if ( win_max_x < 0) win_max_x = 0;
    if ( win_min_y < 0) win_min_y = 0;
    if ( win_max_y < 0) win_max_y = 0;
    if ( win_min_x > source.num_cols-1) win_min_x = source.num_cols-1;
    if ( win_max_x > source.num_cols-1) win_max_x = source.num_cols-1;
    if ( win_min_y > source.num_rows-1) win_min_y = source.num_rows-1;
    if ( win_max_y > source.num_rows-1) win_max_y = source.num_rows-1;

    // Calculate the width and heigh of the new image
    int width = win_max_x - win_min_x;
    int height = win_max_y - win_min_y;

    // Prevent mal formed images
    if ( width < 1 )
        gistk_error_fatal(GISTK_ERRC_CUT_RST_WIDTH,
                          GISTK_ERRS_CUT_RST_WIDTH ,
                          filename);

    if ( height < 1 )
        gistk_error_fatal(GISTK_ERRC_CUT_RST_HEIGHT,
                          GISTK_ERRS_CUT_RST_HEIGHT,
                          filename);

    // Get the sizes for the io buffer
    GDALDataType in_types [source.num_bands];
    int in_sizes [source.num_bands];
    for(int b=0 ; b < source.num_bands; b++) {
      GDALRasterBandH in_band = GDALGetRasterBand( source.data, b+1 );
      in_types[b] = GDALGetRasterDataType( in_band );

      int num_bits = GDALGetDataTypeSize( in_types[b] );
      // @TODO Use GDALGetDataTypeSizeBytes( in_types[b] );
      int num_bytes = num_bits / 8;
      // @TODO Bit packed stuff?
      // if ( num_bits % 8 > 0) num_bytes += 1;
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
