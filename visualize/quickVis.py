#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat May 30 18:55:44 2020

@author: ruben
"""
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import colors

plt.close("all")

fig, ax = plt.subplots(1,1)

sunCollector = np.loadtxt("./build/sunCollector.txt")
imageHandle = ax.imshow(sunCollector.transpose(), origin="lower",vmin = 0, vmax= 12, interpolation='none',cmap=plt.get_cmap('jet'))
fig.colorbar(imageHandle, ax=ax, fraction=0.07, aspect=50) 