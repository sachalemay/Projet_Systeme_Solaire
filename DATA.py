#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Oct 21 17:02:56 2020

@author: Sacha
"""

import json 
import csv 
   
list = ["list_id","list_aphelion","list_perihelion","list_semimajorAxis", "list_mass","list_equaRadius", "list_sideralOrbit", "list_gravity"]
list_labels = ["id","aphelion","perihelion","semimajorAxis", "mass","equaRadius", "sideralOrbit", "gravity"]

with open ('bodies.json') as f:       
    celestarobj = json.load(f)
    
with open ('bodies-2.csv', "a", newline = "") as f:  
    
    writer_csv = csv.writer(f)    
    writer_csv.writerow(list_labels)
    for body in celestarobj["bodies"]: 
        mass_dict = body['mass']
        body['mass'] = float(mass_dict['massValue'])*10**(float(mass_dict['massExponent'])) 
        body.pop("name")
        body.pop('aroundPlanet')
        body.pop('englishName')
        body.pop('eccentricity') #eccentricté utile? 
        body.pop('vol')
        body.pop('density')
        body.pop('escape')
        body.pop('flattening')
        body.pop('dimension')
        body.pop('discoveredBy')
        body.pop('discoveryDate')
        body.pop('polarRadius')
        body.pop('alternativeName')
        body.pop('moons')  #l'affichage des lunes 
        body.pop('rel')      
        body.pop('inclination') 
        body.pop('axialTilt')
        body.pop('sideralRotation')
    
    
        if body["isPlanet"] == True: 
            list_body_data = [body[label] for label in list_labels]
            writer_csv.writerow(list_body_data)
        
