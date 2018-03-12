/* util.h --- Service routines
 */

#ifndef INCLUDED_UTIL_H
#define INCLUDED_UTIL_H 1

// IGEO Standard raster format GeoTIFF
#define IGEO_FMT_GTIFF "GTiff"

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

typedef struct {
  GDALDriverH driver;
  char** info;
  bool can_write;
  bool can_read;
  bool can_copy;
}  igeo_raster_driver_t;

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
} igeo_raster_t;

// ---------------------------------------
/**
 * Opens a a data driver for a given format
 * @param format a gdal forma key GTiff fex
 * @param can_read test always true
 * @param can_write test to write datasets with this driver
 * @param can_copy  test to copy datasets with this driver
 * @result a pointer to a valid a raster driver container
 */
void igeo_open_raster_driver(const char * format,
                       bool can_read,
                       bool can_write,
                       bool can_copy,
                       igeo_raster_driver_t * result);

// ---------------------------------------
/**
 * Opens a raster file by a given name
 * @param filename name of the file
 * @param readonly open the thie read only
 * @result a pointer to a valid a raster container
 */
void igeo_open_raster(const char * filename,
                  bool is_readonly,
                  igeo_raster_t * result);

// ---------------------------------------
/**
 * Closes the raster file
 * @param result is an pointer to an open raster file container.
 * @result the file is close and the structure is closed.
 */
void igeo_close_raster(igeo_raster_t * result);

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
void igeo_cut_raster(const igeo_raster_driver_t tool,
                const igeo_raster_t source,
                const char * filename,
                int win_min_x, int win_min_y,
                int win_max_x, int win_max_y,
                igeo_raster_t * result);

#endif /* INCLUDED_UTIL_H */
