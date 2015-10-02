from __future__ import division
from numpy.core import *
import numpy
from scipy import fftpack
from math import *

N = 64
sigma_x = 10
f1 = 0.2
f2 = 0.5

def tapered_cosine_window(f):
  f_N = abs(f)/(N+1)
  if f_N <= f1:
    return 1.0
  if f_N >= f2:
    return 0.0
  return 0.5+0.5*cos(pi*(f_N-f1)/(f2-f1))

tapered_cosine_window_ufunc = numpy.frompyfunc(tapered_cosine_window, 1, 1)

# The positive real-valued part of the function
f_half = arange(N+1, dtype=float)
G_half = tapered_cosine_window_ufunc(f_half)

# Convert to form [y(0),Re(y(1)),Im(y(1)),...,Re(y(n/2))] 
G_complex = zeros(2*N, float)
G_complex[0] = G_half[0]
G_complex[1::2] = G_half[1:]

print G_half
print "******"
print G_complex

g = fftpack.irfft(G_complex)

print "******"
print g

# Mirror the results to negative values as well for plotting.
G_plot = zeros(2*N+1, float)
G_plot[N:] = G_half
G_plot[:N] = G_half[1::][::-1]

# Shift g so center is in middle
g_plot = zeros(2*N+1, float)
g_plot[N:] = g[:N+1]
g_plot[:N] = g[N:]

x_plot = arange(-N,N+1,1)

print "******"
print g_plot

numpy.savetxt("tapered_cosine_window_freq_space", numpy.column_stack((x_plot,G_plot)))
numpy.savetxt("tapered_cosine_window_real_space", numpy.column_stack((x_plot,g_plot)))
