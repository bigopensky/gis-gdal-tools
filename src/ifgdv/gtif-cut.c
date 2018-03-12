// =====================================================================
// Cut von subsamples (small images) from a geotiff
// (c) - 2013 A. Weidauer  alex.weidauer@huckfinn.de
// All rights reserved to A. Weidauer
// =====================================================================
// gtif-cut.c is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// gtif-cut.c is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with gtif-cut.c.  If not, see <http://www.gnu.org/licenses/>.
// =====================================================================
#include "ifgdv/alg.h"
#include "ifgdv/util.h"

// -------------------------------------------------------------------
int main(int argc, char **argv)
{

  char *ERROR_PRM = "Invalid numeric value %s: %s\n!";

  // Check the minimum of cmd params
  if (argc<7) {
    error_exit(10,
	"Missing parameter at least 8\n"
        "Usage: %s IN OUT EXT WSZ HSZ ID1 X1 Y1 ID2 X2 Y2 ...!\n"
        "Example: %s  dem.v2.3d.tif zz tif 128 423 1 399000 6038000 2 380000 6100000\n",
         argv[0], argv[0]);
  }

  // Read infile pattern from cli
  char *ifile = argv[1];

  // Read outfile pattern from cli
  char *ofile = argv[2];

  // var for the resulting filename
  char cfile[1024];

  // Read file extension from cli
  char *ext   = argv[3];

  // Set default window size and try to read from cli
  int wsize = 64;
  int hsize = 64;
  if (! sscanf(argv[4],"%d",&wsize) ) {
    error_exit(1000+3,ERROR_PRM,"WSZ",argv[4]);
  }

  if (! sscanf(argv[5],"%d",&hsize) ) {
    error_exit(1000+3,ERROR_PRM,"HSZ",argv[4]);
  }

  // init vectors for id's and positions
  int_vector_t id;
  int_vector_init(&id, 10);
  dbl_vector_t pos_x;
  dbl_vector_init(&pos_x, 10);
  dbl_vector_t pos_y;
  dbl_vector_init(&pos_y, 10);

  // Read center positions of the window from cli
  int a = 6; double dbl; int pk;
  while( a < argc-2 ) {

    // parse id coordinate
    if (! sscanf(argv[a],"%d",&pk) ) {
      error_exit(1000+a,ERROR_PRM,"ID", argv[a]);
    }
    int_vector_add(&id,pk);

    // parse x coordinate
    if (! sscanf(argv[a+1],"%lf",&dbl) ) {
      error_exit(1000+a+1,ERROR_PRM,"X", argv[a+1]);
    }
    dbl_vector_add(&pos_x,dbl);

    // parse y coordinate
    if (! sscanf(argv[a+2],"%lf",&dbl) ) {
      error_exit(1000+a+2,ERROR_PRM,"Y", argv[a+2]);
    }
    dbl_vector_add(&pos_y,dbl);

    a+=3;
  }

  // Get the GTiff driver an assure raste, read and write capabilities
  igeo_raster_driver_t gtiff;
  igeo_open_raster_driver( IGEO_FMT_GTIFF, true, true, false, &gtiff );

  // open geotiff and handle error
  igeo_raster_t src_raster;
  printf("# IN FILE:       %s\n", ifile);
  igeo_open_raster( ifile, true, &src_raster);

  printf("# OUT FILE:      %s\n", ofile);
  printf("# EXTENSION:     %s\n", ext);
  printf("# NUM TUPLE:     %d\n", pos_x.length);
  printf("# WINDOW WIDTH:  %d\n",wsize);
  printf("# WINDOW HEIGHT: %d\n",hsize);

  // Create snippets
  for (int c=0; c < pos_x.length; c++ ) {

    // Transform cut position (world) to image positions
    long icol = -1; long irow = -1;
    trfm_geo_pix(src_raster.trfm, pos_x.data[c], pos_y.data[c], &icol , &irow);

    // Create filename from patter id and extention
    sprintf(cfile,"%s.%d.%s",ofile, id.data[c], ext);

    // Test if the window is inside the image and
    // skip the stuff if outside
    if (icol-wsize/2<=0 ||
	irow-hsize/2<=0 ||
	icol+wsize/2>=src_raster.num_cols ||
	irow+hsize/2>=src_raster.num_rows)
    {
      printf ("IGN %d %s %d %d\n",id.data[c], cfile, icol, irow);
      continue;
    }

    // cut the sub image
    printf ("ADD %d %s %d %d\n",id.data[c], cfile, icol, irow);

    int ioffs_col = icol-wsize/2;
    int ioffs_row = irow-hsize/2;

    igeo_raster_t new_raster;
    igeo_cut_raster(gtiff,
                    src_raster,
                    cfile,
                    ioffs_col, ioffs_col+wsize,
                    ioffs_row, ioffs_row+hsize,
                    &new_raster);

    igeo_close_raster(&new_raster);

  } // EOF positions

  // Close source image
  igeo_close_raster(&src_raster);

  return 0;
}

// --- EOF -----------------------------------------------------------
