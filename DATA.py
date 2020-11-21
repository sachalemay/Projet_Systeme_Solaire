#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Nov 18 16:33:43 2020

@author: Sacha
"""

import json 
import csv 
list = ["list_id","list_aphelion","list_perihelion","list_semimajorAxis", "list_mass","list_equaRadius", "list_sideralOrbit", "list_gravity"]
list_labels = ["id","aphelion","perihelion","semimajorAxis", "masse","equaRadius", "sideralOrbit", "gravity"]


with open ('bodies.json') as f:       
    celestarobj = json.load(f)
with open ('camarche.csv', "w", newline = "") as f:  
    writer_csv = csv.writer(f)    
    writer_csv.writerow(list_labels)
    dictionary_list = celestarobj["bodies"]
    for body in dictionary_list: 
        mass_dict = body['mass']
        body['masse'] = mass_dict['massValue']*10**mass_dict['massExponent'] 
        if body["isPlanet"] == True or body["id"] == "soleil": 
            list_body_data = [body[label] for label in list_labels]
            writer_csv.writerow(list_body_data)
        # if body["id"] == "vesta": 
        #     list_vesta_data = [body[label] for label in list_labels]
            
        
# with open ('ref_asteroids.csv', "w", newline = "") as file:  
#     writer_csv = csv.writer(file) 
#     writer_csv.writerow(list_labels)
#     writer_csv.writerow(list_vesta_data)
    