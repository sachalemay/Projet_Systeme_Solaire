#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Oct 21 17:02:56 2020

@author: Sacha
"""

import json 

Solar_Syst_list = ["jupiter", "soleil", "mars", "pluton"]

final = []

for name in Solar_Syst_list: 
    with open ('{%s}.json' % name) as f: 
        celestarobj = json.load(f)
        
        mass_dict = celestarobj['mass']
        celestarobj['mass'] = float(mass_dict['massValue'])*10**(float(mass_dict['massExponent']))

        celestarobj.pop('englishName')
        celestarobj.pop('eccentricity') #eccentricté utile? 
        celestarobj.pop('vol')
        celestarobj.pop('density')
        celestarobj.pop('escape')
        celestarobj.pop('flattening')
        celestarobj.pop('dimension')
        celestarobj.pop('discoveredBy')
        celestarobj.pop('discoveryDate')
        celestarobj.pop('polarRadius')
        celestarobj.pop('alternativeName')
        celestarobj.pop('moons')  #l'affichage des lunes 
        final.append(celestarobj)

