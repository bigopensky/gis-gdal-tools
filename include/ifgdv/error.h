/* error.h --- error routines
 */

#ifndef INCLUDED_ERROR_H
#define INCLUDED_ERROR_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define GISTK_ERRS_INVALID_NUMERIC "Invalid numeric value %s: %s\n!"

// --------------------------------------------------------------
#define GISTK_ERRC_DRV_LOAD_BASE 10100

#define GISTK_ERRC_GDAL_LOAD_DRVS GISTK_ERRC_DRV_LOAD_BASE+1
#define GISTK_ERRS_GDAL_LOAD_DRVS "GDAL raster driver are not available!"

#define GISTK_ERRC_GDAL_INIT_DRVS GISTK_ERRC_DRV_LOAD_BASE+2
#define GISTK_ERRS_GDAL_INIT_DRVS "GDAL raster driver are not registered!"

#define GISTK_ERRC_OGR_LOAD_DRVS GISTK_ERRC_DRV_LOAD_BASE+3
#define GISTK_ERRS_OGR_LOAD_DRVS "OGR vector driver are not available!"

#define GISTK_ERRC_OGR_INIT_DRVS GISTK_ERRC_DRV_LOAD_BASE+4
#define GISTK_ERRS_OGR_INIT_DRVS "OGR vector driver are not registered!"

// --------------------------------------------------------------
#define GISTK_ERRC_OPEN_DRV_BASE  10200

#define GISTK_ERRC_OPEN_DRV_INIT  GISTK_ERRC_OPEN_DRV_BASE+1
#define GISTK_ERRS_OPEN_DRV_INIT  "Access to an uninitialized gistk "\
                                  "raster driver handle for format %s denied!"

#define GISTK_ERRC_OPEN_DRV_VALID  GISTK_ERRC_OPEN_DRV_BASE+2
#define GISTK_ERRS_OPEN_DRV_VALID  "Driver %s not avialable!"

#define GISTK_ERRC_OPEN_DRV_INFO   GISTK_ERRC_OPEN_DRV_BASE+3
#define GISTK_ERRS_OPEN_DRV_INFO   "Metadata for driver %s not avialable!"

#define GISTK_ERRC_OPEN_DRV_ISRST  GISTK_ERRC_OPEN_DRV_BASE+4
#define GISTK_ERRS_OPEN_DRV_ISRST  "Driver %s addresses no type raster type!"

#define GISTK_ERRC_OPEN_DRV_ISREAD  GISTK_ERRC_OPEN_DRV_BASE+5
#define GISTK_ERRS_OPEN_DRV_ISREAD  "Format %s is not readable!"

#define GISTK_ERRC_OPEN_DRV_ISCOPY  GISTK_ERRC_OPEN_DRV_BASE+6
#define GISTK_ERRS_OPEN_DRV_ISCOPY  "Format %s is not copyable!"

#define GISTK_ERRC_OPEN_DRV_ISWRITE  GISTK_ERRC_OPEN_DRV_BASE+7
#define GISTK_ERRS_OPEN_DRV_ISWRITE  "Format %s is not writable!"

// --------------------------------------------------------------
#define GISTK_ERRC_OPEN_RST_BASE  10300

#define GISTK_ERRC_OPEN_RST_INIT GISTK_ERRC_OPEN_RST_BASE+1
#define GISTK_ERRS_OPEN_RST_INIT "Access to an uninitialized gistk "\
                                  "raster handle for format %s denied!"

#define GISTK_ERRC_OPEN_RST_SRCR  GISTK_ERRC_OPEN_RST_BASE+2
#define GISTK_ERRC_OPEN_RST_SRCW  GISTK_ERRC_OPEN_RST_BASE+3
#define GISTK_ERRS_OPEN_RST_SRC   "File %s is not %s!"

#define GISTK_ERRC_OPEN_RST_TRFM GISTK_ERRC_OPEN_RST_BASE+4
#define GISTK_ERRS_OPEN_RST_TRFM "Affine transformation not present"\
                                 " in in file %s !"

#define GISTK_ERRC_OPEN_RST_SRS  GISTK_ERRC_OPEN_RST_BASE+5
#define GISTK_ERRS_OPEN_RST_SRS  "Missing spatial reference system "\
                                 " in in file %s !"

#define GISTK_ERRC_OPEN_RST_CSRS   GISTK_ERRC_OPEN_RST_BASE+6
#define GISTK_ERRS_OPEN_RST_CSRS   "Cannot open spatial reference system "\
                                   " in in file %s !\n SRS is:\n%s"

#define GISTK_ERRC_OPEN_RST_WIDTH   GISTK_ERRC_OPEN_RST_BASE+7
#define GISTK_ERRS_OPEN_RST_WIDTH   "Image width for raster file %s is invalid!"

#define GISTK_ERRC_OPEN_RST_HEIGHT GISTK_ERRC_OPEN_RST_BASE+8
#define GISTK_ERRS_OPEN_RST_HEIGHT "Image height for raster file %s is invalid!"

#define GISTK_ERRC_OPEN_RST_NBAND GISTK_ERRC_OPEN_RST_BASE+9
#define GISTK_ERRS_OPEN_RST_NBAND "Image heightNumber of bands for "\
                                  "raster file %s is invalid!"

#define GISTK_ERRC_CLOSE_RST GISTK_ERRC_OPEN_RST_BASE+20
#define GISTK_ERRS_CLOSE_RST "Access to close an uninitialized gistk "\
                             "raster handle is denied!"

// --------------------------------------------------------------
#define GISTK_ERRC_CUT_RST_BASE  10400

#define GISTK_ERRC_CUT_RST_INIT  GISTK_ERRC_CUT_RST_BASE+1

#define GISTK_ERRC_CUT_RST_WIDTH GISTK_ERRC_CUT_RST_BASE+2
#define GISTK_ERRS_CUT_RST_WIDTH "Cut window width is 0 for file %s!"

#define GISTK_ERRC_CUT_RST_HEIGHT GISTK_ERRC_CUT_RST_BASE+3
#define GISTK_ERRS_CUT_RST_HEIGHT "Cut window hight is 0 for file %s!"

                                
// =================================================================
/**
 * central error exit point
 * @param code    exit code
 * @param message message template sprintf format
 * @param ...     parameter
 */
void gistk_error_fatal(int code, const char *message, ...);


#endif /* INCLUDED_ERROR_H */

