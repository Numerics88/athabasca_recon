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
import numpy
from numpy.core import *
import vtk
from vtk.util.numpy_support import numpy_to_vtk

volume_dims = array((128, 256, 256))     # z,y,x
voxel_size = 0.25  # x,y,z the same
# Note that the coordinates are at the center of voxels.
volume_origin = -0.5*(volume_dims-1)*voxel_size


vol = zeros(volume_dims, float32)

z,y,x = numpy.mgrid[0:volume_dims[0],0:volume_dims[1],0:volume_dims[2]]*voxel_size
z += volume_origin[0]
y += volume_origin[1]
x += volume_origin[2]

def add_sphere(center, radius, intensity):
    global vol
    r2 = (x-center[2])**2 + (y-center[1])**2 + (z-center[0])**2
    vol += intensity * (r2 < radius**2)

add_sphere((0,-16,-16), 12, 0.001)
add_sphere((0,-16,16), 10, 0.003)
add_sphere((0,16,0), 8, 0.004)
add_sphere((0,0,0), 4, 0.005)

integrated_attenuation = sum(vol)*voxel_size**3
print "Integrated Attenuation is ", integrated_attenuation

# For VTK need a flat data array
vol.resize((prod(volume_dims),))

data_vtk = numpy_to_vtk(vol)
image_vtk = vtk.vtkImageData()
image_vtk.SetDimensions(volume_dims[::-1])
image_vtk.SetSpacing((voxel_size,)*3)
image_vtk.SetOrigin(volume_origin[::-1])
image_vtk.GetPointData().SetScalars(data_vtk)

# writer = vtk.vtkXMLImageDataWriter()
# writer.SetFileName("spheres.vti")
# 
# writer = vtk.vtkMetaImageWriter()
# writer.SetFileName("spheres.mhd")

writer = vtk.vtkMetaImageWriter()
writer.SetFileName("synthetic_volume.mha")
writer.SetRAWFileName("synthetic_volume.raw")
writer.SetCompression(0)
writer.SetInput(image_vtk)
writer.Write()
