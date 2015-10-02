"""
Eric Nodwell, June 2011
enodwell@ucalgary.ca

This utility calculates parallel projections by doing an interpolated image
rotation of the volume, then summing the rows (which are now perpendicular
to the detector face).

Two real coordinate systems are used; both have their origins at the
center of rotation for simplicity:

X_V : real-space coordinates aligned with volume
X_P : real-space coordinates aligned with projection

Note that in X_P space, z is along the vertical axis of the detector, while
y is along the horizontal axis of the detector.

As we have chosen the centers of these coordinate systems to align, we have:

X_P = R X_V

where R is the rotation matrix:

R  =  [ 1      0            0       0  ]
      [ 0   cos(theta)  sin(theta)  0  ]
      [ 0  -sin(theta)  cos(theta)  0  ]
      [ 0      0            0       1  ]

Recall that in python, the fastest changing index is last, hence the order
of dimensions is z,y,x ; so this is a rotation around the z axis.

We are using here the 4x4 matrix convention to keep track of translations
as well as affine transformations.

Two voxel coordinate systems are used (indexed by integers of course):

V : voxel index of the volume (Vz, Vy, Vx)
P : voxel index of the rotated volume (note: not the same projection pixels)

These relate to the real-space coordinate systems as follows:

X_V = T_V S_V V

X_P = T_P S_P P

where T are translation transformations and S are scaling transformations:

T_V  =  [  1  0  0  origin_V_z ]
        [  0  1  0  origin_V_y ]
        [  0  0  1  origin_V_x ]
        [  0  0  0     1       ]

origin_V is the real-space location of the center of the 0,0,0 voxel.

S_V  =  [  voxel_size      0          0        0 ]
        [       0      voxel_size     0        0 ]
        [       0          0       voxel_size  0 ]
        [       0          0          0        1 ]

Similarly for T_P and S_P.

Putting all this together gives the transformation from volume indices
to indices of the scaled and rotated volume (aligned with the projections):

P = inv(S_P) inv(T_P) R T_V S_V V

And the inverse tranformation is

V = inv(S_V) inv(T_V) R T_P S_P P

"""

# Copyright (C) 2011 Eric Nodwell
# enodwell@ucalgary.ca
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


from __future__ import division
import sys
import numpy
from numpy.core import *
from numpy import matrix
import scipy
import scipy.ndimage
import vtk
from vtk.util.numpy_support import numpy_to_vtk, vtk_to_numpy


# -------------------------------------------------------------------------
#  Configuration

if len(sys.argv) != 2:
  print "Usage: python parallel_project.py example.conf"
  sys.exit(1)
config_file = sys.argv[1]

# Read in configuration file.
print "Reading configuration file " + config_file
# Here we are actually executing it as python code.
execfile(config_file)

print
print "Configuration"
print "-"*78
print "input_data_file: ", input_data_file
print "output_file_root: ", output_file_root
print "detector_horizontal_pixels: ", detector_horizontal_pixels
print "detector_vertical_pixels: ", detector_vertical_pixels
print "detector_pixel_size: ", detector_pixel_size
print "detector_vertical_center: ", detector_vertical_center
print "detector_horizontal_offset: ", detector_horizontal_offset
print "number_of_projections: ", number_of_projections
print "projection_at_180: ", projection_at_180
print "view_projections_from_behind: ", view_projections_from_behind
print "beam_power_decay_factor: ", beam_power_decay_factor
print "number_of_dark_field_measurements: ", number_of_dark_field_measurements
print "length of bad_pixel_list: ", len(bad_pixel_list)
print "-"*78


# ---------------------------------------------------------------------
# Read data

print
print "Reading", input_data_file
reader = vtk.vtkMetaImageReader()
reader.SetFileName(input_data_file)
reader.Update()
volume_vtk = reader.GetOutput()

# Convert all the following to z,y,x order
volume_dims = array(volume_vtk.GetDimensions())[::-1]
volume_origin = array(volume_vtk.GetOrigin())[::-1]
voxel_size = array(volume_vtk.GetSpacing())[::-1]
# Going to enforce cubic voxels.
assert(voxel_size[1] == voxel_size[0])
assert(voxel_size[2] == voxel_size[0])
voxel_size = voxel_size[0]
print "Volume dims: ", volume_dims
print "Volume origin: ", volume_origin
print "Voxel size: ", voxel_size

volume_data_vtk = volume_vtk.GetPointData().GetScalars()
volume = vtk_to_numpy(volume_data_vtk)
assert(volume.size == prod(volume_dims))
volume.resize(volume_dims)


# ---------------------------------------------------------------------
# Derived values
#
# including assembling transformation matrices

print
subdivide_factor = int(ceil(detector_pixel_size/voxel_size))
print "Using a subdivide factor of", subdivide_factor
rot_voxel_size = detector_pixel_size/subdivide_factor
# Note: The third (x) value is required so that the the rotated volume will
#       extend beyond the original volume for any rotation angle.
rot_volume_dims = array([detector_vertical_pixels*subdivide_factor,
                         detector_horizontal_pixels*subdivide_factor,
                         int(ceil(sqrt(2)*volume_dims[2]*voxel_size/rot_voxel_size))])
rot_volume_origin = array([detector_vertical_center,
                           detector_horizontal_offset,
                           0]) - 0.5*(rot_volume_dims-1)*rot_voxel_size
# Note: This is slightly tricky.  Without subdividing,
#   projection_origin = rot_volume_origin[:2] .  However, when subdiving,
#   since we measure from the center of the element, this changes, even
#   though the corners are still coincident.
projection_origin = array([detector_vertical_center,
                           detector_horizontal_offset]) \
                           - 0.5*(array([detector_vertical_pixels,
                                         detector_horizontal_pixels])-1)*detector_pixel_size
if view_projections_from_behind:
  projection_origin[1] = -(projection_origin[1] +
                           (detector_horizontal_pixels-1)*detector_pixel_size)

S_V_inv = matrix([[ 1/voxel_size, 0,            0,            0 ],
                  [ 0,            1/voxel_size, 0,            0 ],
                  [ 0,            0,            1/voxel_size, 0 ],
                  [ 0,            0,            0,            1 ]])

T_V_inv = matrix([[ 1, 0, 0, -volume_origin[0] ],
                  [ 0, 1, 0, -volume_origin[1] ],
                  [ 0, 0, 1, -volume_origin[2] ],
                  [ 0, 0, 0, 1                 ]])

S_P = matrix([[ rot_voxel_size, 0,                0,                0 ],
              [ 0,              rot_voxel_size,   0,                0 ],
              [ 0,              0,                rot_voxel_size,   0 ],
              [ 0,              0,                0,                1 ]])

T_P = matrix([[ 1, 0, 0, rot_volume_origin[0] ],
              [ 0, 1, 0, rot_volume_origin[1] ],
              [ 0, 0, 1, rot_volume_origin[2] ],
              [ 0, 0, 0, 1                    ]])


# ---------------------------------------------------------------------
# Bright field and dark field

dark_field = 50*ones((detector_vertical_pixels, detector_horizontal_pixels), int16)
bright_field = 4000*ones((detector_vertical_pixels, detector_horizontal_pixels), int16)
flat_field = bright_field - dark_field


# ---------------------------------------------------------------------
# Calculate projections

print

projections = zeros((number_of_projections,
                     detector_vertical_pixels,
                     detector_horizontal_pixels), int16)

for count in arange(number_of_projections):
  
  if projection_at_180:
    theta = pi*count/(number_of_projections-1)
  else:
    theta = pi*count/number_of_projections
  print "calculating projection %d at angle %.2f degrees" % (count, 180*theta/pi)

  R = matrix([[1, 0,           0,          0],
              [0, cos(theta),  sin(theta), 0],
              [0, -sin(theta), cos(theta), 0],
              [0, 0,           0,          1]])

  # matrix multiplication, as the objects are matrices, not arrays
  M = array(S_V_inv * T_V_inv * R * T_P * S_P)
  rot_volume = scipy.ndimage.interpolation.affine_transform(
                   volume,
                   M[:3,:3],   # this is the affine part of the 4x4 transformation matrix
                   offset=M[:3,3],   # this is the offset vector
                   output_shape=rot_volume_dims,
                   order=1)

  # Now sum the rotated and interpolated volume along the x axis
  projection_subdivided = sum(rot_volume, axis=2)
  assert(alltrue(projection_subdivided.shape == 
      subdivide_factor*array((detector_vertical_pixels, detector_horizontal_pixels))))

  if view_projections_from_behind:
    projection_subdivided = projection_subdivided[:,::-1]

  # If we subdivided the projection pixels, now we sum the sub-pixels.
  if (subdivide_factor > 1):
    projection = zeros((detector_vertical_pixels, detector_horizontal_pixels), float)
    for i in range(subdivide_factor):
      for j in range(subdivide_factor):
        projection += projection_subdivided[i::subdivide_factor,j::subdivide_factor]
    projection *= (1/subdivide_factor)**2
  else:
    projection = projection_subdivided
  
  projection = (beam_power_decay_factor**(count+number_of_dark_field_measurements) \
               *exp(-projection*rot_voxel_size)) \
               *flat_field \
               + dark_field

  for ij in bad_pixel_list:
      projection[ij[0],ij[1]] = 0
  
  projections[count] = projection


# ---------------------------------------------------------------------
# Save projections

# For VTK need a flat data array
projections.resize((prod(projections.shape),))

data_vtk = numpy_to_vtk(projections)
assert(data_vtk.GetNumberOfTuples() == projections.size)
image_vtk = vtk.vtkImageData()
# Convert to x,y,z order
image_vtk.SetDimensions(detector_horizontal_pixels, detector_vertical_pixels, number_of_projections)
image_vtk.SetOrigin(projection_origin[1], projection_origin[0], 0)
image_vtk.SetSpacing((detector_pixel_size,)*3)
image_vtk.GetPointData().SetScalars(data_vtk)

writer = vtk.vtkMetaImageWriter()
writer.SetFileName(output_file_root + ".mha")
writer.SetRAWFileName(output_file_root + ".raw")
writer.SetCompression(0)
writer.SetInput(image_vtk)
writer.Write()

for ij in bad_pixel_list:
  dark_field[ij[0],ij[1]] = 0
  flat_field[ij[0],ij[1]] = 0


multi_dark_field = zeros((number_of_dark_field_measurements, dark_field.shape[0], dark_field.shape[1]), int16)
multi_dark_field[:] = dark_field
multi_dark_field.resize((prod(multi_dark_field.shape),))

data_vtk = numpy_to_vtk(multi_dark_field)
assert(data_vtk.GetNumberOfTuples() == multi_dark_field.size)
image_vtk = vtk.vtkImageData()
# Convert to x,y,z order
image_vtk.SetDimensions(detector_horizontal_pixels, detector_vertical_pixels, number_of_dark_field_measurements)
image_vtk.SetOrigin(projection_origin[1], projection_origin[0], 0)
image_vtk.SetSpacing((detector_pixel_size,)*3)
image_vtk.GetPointData().SetScalars(data_vtk)

writer = vtk.vtkMetaImageWriter()
writer.SetFileName(output_file_root + "-dark.mha")
writer.SetRAWFileName(output_file_root + "-dark.raw")
writer.SetCompression(0)
writer.SetInput(image_vtk)
writer.Write()


multi_bright_field = zeros((number_of_dark_field_measurements, bright_field.shape[0], bright_field.shape[1]), int16)
for i in range(number_of_dark_field_measurements):
    multi_bright_field[i] = flat_field*beam_power_decay_factor**i + dark_field
multi_bright_field.resize((prod(multi_bright_field.shape),))

data_vtk = numpy_to_vtk(multi_bright_field)
assert(data_vtk.GetNumberOfTuples() == multi_bright_field.size)
image_vtk = vtk.vtkImageData()
# Convert to x,y,z order
image_vtk.SetDimensions(detector_horizontal_pixels, detector_vertical_pixels, number_of_dark_field_measurements)
image_vtk.SetOrigin(projection_origin[1], projection_origin[0], 0)
image_vtk.SetSpacing((detector_pixel_size,)*3)
image_vtk.GetPointData().SetScalars(data_vtk)

writer = vtk.vtkMetaImageWriter()
writer.SetFileName(output_file_root + "-bright.mha")
writer.SetRAWFileName(output_file_root + "-bright.raw")
writer.SetCompression(0)
writer.SetInput(image_vtk)
writer.Write()

# Bright field after (really only useful for beam_power_decay_factor < 1)

if beam_power_decay_factor != 1:

  multi_bright_field = zeros((number_of_dark_field_measurements, bright_field.shape[0], bright_field.shape[1]), int16)
  for i in range(number_of_dark_field_measurements):
      factor = beam_power_decay_factor**(i + number_of_dark_field_measurements + number_of_projections)
      multi_bright_field[i] = flat_field*factor + dark_field
  multi_bright_field.resize((prod(multi_bright_field.shape),))

  data_vtk = numpy_to_vtk(multi_bright_field)
  assert(data_vtk.GetNumberOfTuples() == multi_bright_field.size)
  image_vtk = vtk.vtkImageData()
  # Convert to x,y,z order
  image_vtk.SetDimensions(detector_horizontal_pixels, detector_vertical_pixels, number_of_dark_field_measurements)
  image_vtk.SetOrigin(projection_origin[1], projection_origin[0], 0)
  image_vtk.SetSpacing((detector_pixel_size,)*3)
  image_vtk.GetPointData().SetScalars(data_vtk)

  writer = vtk.vtkMetaImageWriter()
  writer.SetFileName(output_file_root + "-bright_after.mha")
  writer.SetRAWFileName(output_file_root + "-bright_after.raw")
  writer.SetCompression(0)
  writer.SetInput(image_vtk)
  writer.Write()
