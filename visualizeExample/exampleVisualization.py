#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jun  1 14:29:26 2020

@author: ruben
"""

import xml.etree.ElementTree as ET
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import colors
import matplotlib
matplotlib.rcParams.update({'font.size': 6})
plt.close("all")

# import option file
tree = ET.parse("../input/options.xml")
options = tree.getroot()


""" make a zoned color map """ 
cmap = colors.ListedColormap(['darkblue', 'lawngreen', 'orange'])
bounds=[0,2,5,10]
norm = colors.BoundaryNorm(bounds, cmap.N)
""" use a continuous color map """ 
#cmap = plt.get_cmap('jet')


""" Setup everything for the figure """ 

fig, axs = plt.subplots(3,3, sharex='col', sharey='row',figsize=(4,8))
axs = axs.reshape(9)

""" Setup surface of interest (switch from m to cm) """
regionWidth =1
regionDepth = 1
stepsWidth = 1
stepsDepth = 1


""" Collect data and plot """
heightIncr = float(options.find("heightIncr").text)
inputFolder = options.find("outputPath").text
for step in range(0,9):
    height = heightIncr*(step+2) *100
    data = np.loadtxt(inputFolder + f"/growseason/height_{height:.0f}.txt").transpose()    
    imageHandle = axs[step].imshow(data, origin="lower",vmin = 0, vmax= 10, interpolation='none', extent=[0,130,0,385],cmap=cmap, norm=norm)
    """ These thicks are specifically done for the example and should probably 
    be disable or updated for any other geometry. """ 
    axs[step].set_xticks(5*np.arange(data.shape[1])+1, minor=True)
    axs[step].set_yticks(5*np.arange(data.shape[0])+1, minor=True)
    axs[step].set_xticks(5*np.arange(data.shape[1])[0::5], minor=False)
    axs[step].set_yticks(5*np.arange(data.shape[0])[0::5], minor=False)
    #ax[ct].grid(which="minor", color="w", linestyle='-', linewidth=0.5)
    axs[step].tick_params(which="minor", bottom=False, left=False)
    axs[step].grid(which='major', color='w', linestyle='-', linewidth=0.5)
    axs[step].set_title(f"height: {height:3.0f}cm")

""" Add colormap legend and save figure. """ 
fig.tight_layout()
fig.colorbar(imageHandle, ax=axs, fraction=0.07, aspect=50, cmap=cmap, norm=norm, spacing='proportional') 
fig.savefig("overview_grow_season_zoned.png", dpi=100)