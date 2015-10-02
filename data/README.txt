Sample data files for athabasca recon
-------------------------------------

October 13, 2011

Data File Format
----------------

The data file format is ITK MetaImage.  Each data file consists in fact
of two files, the .raw file, and the .mhd file containing the meta
information, which can be read with a text editor.  For details refer to
the Athabasca Recon manual.


Synthetic Volume
----------------

The synthetic volume data set is synthetic_volume.mhd.  It consists of
4 spheres in space.  All the projection data sets were obtained from this
synthetic volume.  The dimensions are:

  ElementSpacing = 0.25 0.25 0.25
  DimSize = 256 256 128


Projection Data Set File Arrangement
------------------------------------

Each projection data file set, with name {NAME} has the following files:

{NAME}.mhd          : the projections themselves
{NAME}-bright.mhd   : the bright field
{NAME}-dark.mhd     : the dark field
parallel_project_{SHORTENED_NAME}.conf   : the configuration file used to
                                           generate the data sets.

This parallel_project_*.conf configuration files are to be used with the
script parallel_project.py, distributed with the Athabasca Recon source code.


List Of Projection Data Sets
----------------------------

projections :
  A basic projection data set, with:
    pixel size (0.8, 0.8)
    projections dimensions (96,64)
    129 projections (the last is 180 degrees from the first)
  This data set is centered.

projections_offset :
  Similar to the "projections" data set, but the rotation axis is shifted
  by 5 units from the center of the projections.

projections_decay :
  Similar to the "projections" data set, but the simulated beam power
  decreases by a factor of 0.99 from one projection to the next.


Quick Reconstruction Example For The Impatient
----------------------------------------------

I really recommend that you do read the manual, and at least go through
the tutorials.  However, if you want to get a quick reconstruction, you
can run this command:

  athabasca_recon recon.conf
