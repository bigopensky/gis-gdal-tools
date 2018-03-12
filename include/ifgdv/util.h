/* util.h --- Service routines
 */

#ifndef INCLUDED_UTIL_H
#define INCLUDED_UTIL_H 1

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <strings.h>
#include <gdal.h>
#include <ogr_srs_api.h>
#include <cpl_conv.h>
#include <cpl_string.h>

// GISTK Standard raster format GeoTIFF
#define GISTK_FMT_GTIFF "GTiff"

typedef struct {
  GDALDriverH driver;
  char** info;
  bool can_write;
  bool can_read;
  bool can_copy;
}  gistk_raster_driver_t;

typedef struct {
  GDALDatasetH data;
  OGRSpatialReferenceH srs;
  double trfm[6];
  const char * proj_info;
  int num_cols;
  int num_rows;
  int num_bands;
  bool is_open;
  bool readonly;
} gistk_raster_t;


/**
 * Initializes the gdal stuff
 * @param use_raster - Load GDALdriver 
 * @param use_vector - Load OGR driver
 * @result sets the global variables gistk_raster_loaded and
 *         gistk_raster_loaded to true or false
 * @error - exits with fatal error one of the driver register is'nt present
 */
void gistk_init(bool use_raster, bool use_vector);

void gistk_check_raster_driver_base();

// ---------------------------------------
/**
 * Opens a a data driver for a given format
 * @param format a gdal forma key GTiff fex
 * @param can_read test always true
 * @param can_write test to write datasets with this driver
 * @param can_copy  test to copy datasets with this driver
 * @result a pointer to a valid a raster driver container
 */
void gistk_open_raster_driver(const char * format,
                       bool can_read,
                       bool can_write,
                       bool can_copy,
                       gistk_raster_driver_t * result);

// ---------------------------------------
/**
 * Check the memory validity of a resulting
 * raster object
 * @param - err_source error code number for the source
 * @param - filename addressed in the runtime context
 * @param - result memory object to be checked
 * @error - exits with fatal if result is == NULL
 */
void gistk_check_raster_init(int err_source,
                       const char * filename,
                             gistk_raster_t * result);


// ---------------------------------------
/**
 * Opens a raster file by a given name
 * @param filename name of the file
 * @param readonly open the thie read only
 * @result a pointer to a valid a raster container
 */
void gistk_open_raster(const char * filename,
                  bool is_readonly,
                  gistk_raster_t * result);

// ---------------------------------------
/**
 * Closes the raster file
 * @param result is an pointer to an open raster file container.
 * @result the file is close and the structure is closed.
 */
void gistk_close_raster(gistk_raster_t * result);

// ---------------------------------------
/**
 * Cuts a window out of a existing rasterfile 
 * @param tool - driver container to create a new raster source
 * @param source - an open raster file container 
 * @param filename - for the new target object
 * @param win_min_x - left x coordinate [pixel] of the cut window
 * @param win_max_x - right x coordinate [pixel] of the cut window
 * @param win_min_y - lower x coordinate [pixel] of the cut window
 * @param win_max_y - rupper x coordinate [pixel] of the cut window
 * @param result -  a pointer to a valid a raster container
 */
void gistk_cut_raster(const gistk_raster_driver_t tool,
                const gistk_raster_t source,
                const char * filename,
                int win_min_x, int win_min_y,
                int win_max_x, int win_max_y,
                gistk_raster_t * result);

#endif /* INCLUDED_UTIL_H */
