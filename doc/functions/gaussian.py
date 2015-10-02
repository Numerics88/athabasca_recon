from __future__ import division
from numpy.core import *
import numpy
from scipy import fftpack

N = 64
sigma_x = 10

# The positive real-valued part of the function
x_half = arange(N+1)
g_half = exp(-0.5*(x_half/sigma_x)**2)

# Fancy flipping N/2 -> negative values, but with symmetry
g = zeros(2*N, float)
g[:N+1] = g_half
g[N+1:] = g[N-1:0:-1]

g_fft = fftpack.rfft(g)
# Extract real values, note that is in form [y(0),Re(y(1)),Im(y(1)),...,Re(y(n/2))] 
G_half = concatenate( (array((g_fft[0],)), g_fft[1::2]) )

# Mirror the results to negative values as well for plotting.
x_plot = arange(-N,N+1,1)

g_plot = zeros(2*N+1, float)
g_plot[N:] = g_half
g_plot[:N] = g_half[1::][::-1]

G_plot = zeros(2*N+1, float)
G_plot[N:] = G_half
G_plot[:N] = G_half[1::][::-1]

numpy.savetxt("gaussian_real_space", numpy.column_stack((x_plot,g_plot)))
numpy.savetxt("gaussian_freq_space", numpy.column_stack((x_plot,G_plot)))
