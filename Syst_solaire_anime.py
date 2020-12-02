#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Nov 25 11:45:07 2020

@author: Sacha
"""
import numpy as np
from tkinter import *
from random import randint
import time
import math


class Planet():
  def __init__ (self,name, x_list, y_list,planetRadius,color):
    self.name = name
    self.xlist = x_list
    self.ylist = y_list
    self.planetRadius = planetRadius
    self.color = color
    
    [x0,y0] = Planet.getPosition(self)
    
    self.thisPlanet = canvas.create_oval(x0-self.planetRadius,y0-self.planetRadius,
      x0+self.planetRadius,y0+self.planetRadius,fill = self.color, outline = "")
    
  def getPosition(self):
    #x = center[0]+self.radiusToSun*math.cos(self.speedFactor*self.angle)
    #y = center[1]+self.radiusToSun*math.sin(self.speedFactor*self.angle)
    global t
    x = center[0] + (self.xlist[t])*WIDTH/x_max
    y = center[1] + (self.ylist[t])*HEIGHT/y_max
    return [x,y]

  def updateCoordinates(self):
    global t
    [xn,yn] = Planet.getPosition(self)
    [xn1,yn1,xn2,yn2] = xn-self.planetRadius,yn-self.planetRadius,xn+self.planetRadius,yn+self.planetRadius
    canvas.coords(self.thisPlanet, xn1,yn1,xn2,yn2)


def change_bornes(t): 
    global x_max
    global y_max
    if asteroide.xlist[t]>x_max or asteroide.ylist[t]>y_max : 
        x_max = x_max * 10
        y_max = y_max * 10 

        
if __name__ == "__main__":
  counter = 0
  s = []
  c = 1
  while c == True:
    try:
      tmp = input().strip().split()
      data = np.array(tmp, dtype=np.double)
      s.append(data)
      counter += 1
    except EOFError:
      print("Input has terminated!")
      c = 0
    except ValueError:
      print("Invalid input, skipping.  Input was: %s"%tmp)
      continue

t = 0
WIDTH  = 1200
HEIGHT = 800
center = [WIDTH/2,HEIGHT/2]
x_max = 950000000
y_max = 950000000

tk = Tk()
canvas = Canvas(tk, width = WIDTH, height = HEIGHT, bg = "#212121")
T = Text(tk, height=2, width=30)
montexte = StringVar()
label = Label(tk, textvariable = montexte)
label.pack()

tk.title("Solar System")
canvas.pack()

def stars(numberOfStars):
  starLocations = []
  starRadius = 0.5
  for i in range(numberOfStars):
    starLocations.append([randint(0,WIDTH),randint(0,HEIGHT)])
  for i in range(numberOfStars):
    drawStar = canvas.create_rectangle(starLocations[i][0]-starRadius,starLocations[i][1]-starRadius,
      starLocations[i][0]+starRadius,starLocations[i][1]+starRadius,fill = "white", outline = "")
stars(300)


ceres = Planet("ceres", s[0], s[1], 5, "#7A7373")
eris = Planet("eris", s[2], s[3], 5, "#898989")
uranus = Planet("uranus", s[4], s[5], 5, "#4FD0E7")
pluton = Planet("pluton", s[6], s[7], 20, "#9ca6b7")
neptune = Planet("neptune",s[8],s[9],5, "#73ACAC")
haumea = Planet("haumea",s[10],s[11],5, "#F5F5F5")
makemake = Planet("makemake",s[12],s[13],5, "#D07649")
jupiter = Planet("jupiter",s[14], s[15], 5, "#DE654B")
mars = Planet("mars",s[16], s[17], 2, "red") #FF2D00
mercury = Planet("mercury", s[18],s[19],2, "#B99289")
saturne = Planet("saturne",s[20], s[21], 5, "#C0B89E")
sun     = Planet("sun", [0],[0],17, "#FCD440")
earth = Planet("earth",s[24], s[25], 5, "#0859AD")
venus = Planet("venus",s[26],s[27],5, "#FF5733")
asteroide = Planet("asteroide",s[28], s[29], 2, "white")
asteroide_reference = Planet("asteroide_ref",s[30], s[31], 5, "brown")
interval_time = s[32][0]
NbPoints = int(s[33][0])
print(NbPoints)
montexte.set("Time = 0 days")

while t<NbPoints-2:
  t += 1
  present_time = t * interval_time
  present_time_txt = "%.1f" % present_time
  txt = "Time = " + present_time_txt + " days"
  montexte.set(txt)
  change_bornes(t)
  
  
  ceres.updateCoordinates()
  eris.updateCoordinates()
  uranus.updateCoordinates()
  pluton.updateCoordinates()
  neptune.updateCoordinates()
  haumea.updateCoordinates()
  makemake.updateCoordinates()
  jupiter.updateCoordinates()
  mars.updateCoordinates()
  mercury.updateCoordinates()
  saturne.updateCoordinates()
  earth.updateCoordinates()
  venus.updateCoordinates()
  jupiter.updateCoordinates()
  asteroide.updateCoordinates()
  asteroide_reference.updateCoordinates()
  
  #time.sleep(0.009)
  time.sleep(0.000000000001)
  tk.update()

canvas.mainloop()