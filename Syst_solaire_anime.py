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


class Celestar_object():
  def __init__ (self,name, x_list, y_list,planetRadius,color):
    self.name = name
    self.xlist = x_list
    self.ylist = y_list
    self.planetRadius = planetRadius
    self.color = color
    
    [x0,y0] = Celestar_object.getPosition(self)
    
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
    [xn,yn] = Celestar_object.getPosition(self)
    [xn1,yn1,xn2,yn2] = xn-self.planetRadius,yn-self.planetRadius,xn+self.planetRadius,yn+self.planetRadius
    canvas.coords(self.thisPlanet, xn1,yn1,xn2,yn2)
    
  def resize(self, size_mult):
    self.planetRadius = self.planetRadius * size_mult
        
def resize_global(size_mult): 
    global object_list
    for object in object_list:
        if object.name != "asteroide": 
            object.resize(size_mult)
    sun.resize(size_mult)
    
def change_bornes(t): 
    global x_max
    global y_max
    x,y = asteroide.getPosition()
    if x>WIDTH or y>HEIGHT or x<0 or y<0 : 
        x_max = x_max * 2
        y_max = y_max * 2
        resize_global(1/2)
    if x_max > 950000000 and y_max >950000000 and ((1/4)*WIDTH<x<(3/4)*WIDTH) and ((1/4)*HEIGHT<y<(3/4)*HEIGHT):
        x_max = x_max * 1/2
        y_max = y_max * 1/2
        resize_global(2)
        
if __name__ == "__main__":
  s = []
  c = 1
  while c == True:
    try:
      tmp = input().strip().split()
      data = np.array(tmp, dtype=np.double)
      s.append(data)
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

kuiper_distance = 14900000000

###                    Creation of main canvas  + title + label          ###

tk = Tk()
canvas = Canvas(tk, width = WIDTH, height = HEIGHT, bg = "#000210") #212121 
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


def KuiperBelt(asteroid_number):
  astLocations = []
  astRadius = 0.5
  for i in range(asteroid_number):
    rand_distance = randint(10000000,15000000)
    rand_teta = randint(0,1) * 2* math.pi
    astLocations.append([center[0]+ rand_distance * math.cos(rand_teta)* WIDTH/x_max,center[1] + rand_distance* math.sin(rand_teta) * HEIGHT/y_max])
  for i in range(asteroid_number):
    drawAst = canvas.create_rectangle(astLocations[i][0]-astRadius,astLocations[i][1]-astRadius,
      astLocations[i][0]+astRadius,astLocations[i][1]+astRadius,fill = "brown", outline = "")

KuiperBelt(800)


ceres = Celestar_object("ceres", s[0], s[1], 4, "#7A7373")
eris = Celestar_object("eris", s[2], s[3], 4, "#898989")
uranus = Celestar_object("uranus", s[4], s[5], 10, "#4FD0E7")
pluton = Celestar_object("pluton", s[6], s[7], 5, "#9ca6b7")
neptune = Celestar_object("neptune",s[8],s[9],10, "#73ACAC")
haumea = Celestar_object("haumea",s[10],s[11],4, "#F5F5F5")
makemake = Celestar_object("makemake",s[12],s[13],4, "#D07649")
jupiter = Celestar_object("jupiter",s[14], s[15], 15, "#DE654B")
mars = Celestar_object("mars",s[16], s[17], 7.5, "red") #FF2D00
mercury = Celestar_object("mercury", s[18],s[19],5, "#B99289")
saturne = Celestar_object("saturne",s[20], s[21], 15, "#C0B89E")
sun     = Celestar_object("sun", [0],[0],30, "#FCD440")
earth = Celestar_object("earth",s[24], s[25], 10.4, "#0859AD")
venus = Celestar_object("venus",s[26],s[27],10.4, "#FF5733")
asteroide = Celestar_object("asteroide",s[28], s[29], 2, "white")
asteroide_reference = Celestar_object("asteroide_ref",s[30], s[31], 4, "brown")
interval_time = s[32][0]
NbPoints = int(s[33][0])
tableau_distance_soleil = s[34]
collision_with_planet = s[35]

object_list = [ceres,eris, uranus, pluton, neptune, haumea, makemake, jupiter, mars, mercury, saturne, earth, venus, asteroide, asteroide_reference ] # sans soleil 

print(NbPoints)

while t<NbPoints-2:
  t += 1
  present_time = t * interval_time
  present_time_txt = "%.0f" % present_time
  dist_sun = "%10.4E" % s[34][t]
  txt = "Time = " + present_time_txt + " days" + "                      Asteroid's distance from Sun  = " +  dist_sun + " kilometers..."
  montexte.set(txt)
  #change_bornes(t)
  
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