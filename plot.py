#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Nov 21 19:45:23 2020
@author: Sacha
"""

import numpy as np
from matplotlib import pyplot as plt

 
def plot_data(X, Y):
  plt.plot(X, Y)
  
def plot_show(): 
    plt.show()
 
if __name__ == "__main__":
  counter = 0
  s = []
  while True:
    try:
      tmp = input().strip().split()
      data = np.array(tmp, dtype=np.double)
      s.append(data)
      counter += 1
      
    except EOFError:
      print("Input has terminated! Exiting")
      plot_data(s[0], s[1])
      plot_data(s[2], s[3])
      plot_show()
      exit()
    except ValueError:
      print("Invalid input, skipping.  Input was: %s"%tmp)
      continue