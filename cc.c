#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "image.h"


struct donneePlanet {
	char planetName[100];			//"name"
	double demiGrandAxe; 		//ou "aphelion"
	double demiPetitAxe; 		//ou "perihelion"
	double masse; 					// "massValue" + "massExponent"
	double radius;					//"equaRadius"
	double fullOrbitTime;		//"sideralOrbit"
	double distanceSoleil;
	double x_cartesien;
	double previous_x_cartesien;
	double y_cartesien;
	double previous_y_cartesien;
	double distanceAsteroide;
	double dephasage;
	double Vrai_demiGrandAxe;
	double * tableau_coordonnees_X;
	double * tableau_coordonnees_Y;
	
};
struct meteorite {
	double initialspeed_x;
	double initialspeed_y;
	double distanceSoleil;
	double distancePlanet;
	double masse;
	double radius;
	double x_cartesien;
	double previous_x_cartesien;
	double y_cartesien;
	double previous_y_cartesien;
	double vitesse_tot;
	_Bool collision;
	char * collisionWith;
	int number_of_pl;
	double * tableau_coordonnees_X;
	double * tableau_coordonnees_Y;
	double * tableau_distance_soleil;
	double * tableau_vitesses; 
};
struct forceCaract{
	double direction_x;
	double direction_y;
	double intensitee;
};
double random_radian() {
	//Cette fonction donne un nombre aleatoire entre 0 et 2 pi
	double radian =((double)rand()) / ((double)RAND_MAX)*2*M_PI;
	return radian; 
}
void aphel_dephasage(struct donneePlanet * planet){
	//Cette fonction place le nombre aleatoire en radian dans le dephasage d'une planet pour changer sa position initiale
	random_radian();
	planet->dephasage = random_radian(); 	
}
void global_dephasage (struct donneePlanet * planets, int lenght){
	//Cette fonction permet de changer le dephasage pour toutes les planetes en appelant la fonction precedente en boucle
	for (int i=1; i<lenght; i++){
		aphel_dephasage(&planets[i]);
	}
	
}
void avancementParametrisation ( double time, struct donneePlanet * planet){
	//Cette fonction permet de faire bouger une planete sur son ellipse en fonction du temps
	planet->previous_x_cartesien = planet->x_cartesien;
	planet->previous_y_cartesien = planet->y_cartesien;
	planet->distanceSoleil = planet->demiPetitAxe * planet->demiGrandAxe / sqrt( pow(planet->demiPetitAxe*cos(time*2*M_PI/planet->fullOrbitTime+planet->dephasage),2) + pow(planet->demiGrandAxe*sin(time*2*M_PI/planet->fullOrbitTime+planet->dephasage),2));
	planet->x_cartesien= planet->demiGrandAxe * cos(time*2*M_PI / planet->fullOrbitTime + planet->dephasage);
	planet->y_cartesien= planet->demiPetitAxe * sin(time*2*M_PI / planet->fullOrbitTime + planet->dephasage);
}
void avancementParametrisation_Asteroid ( double time, struct donneePlanet * Asteroide_reference){
	//Cette fonction fait bouger la position de l'asteroide de reference sur son ellipse en fonction de temps
	Asteroide_reference->previous_x_cartesien = Asteroide_reference->x_cartesien;
	Asteroide_reference->previous_y_cartesien = Asteroide_reference->y_cartesien;
	Asteroide_reference->distanceSoleil = Asteroide_reference->demiPetitAxe * Asteroide_reference->demiGrandAxe / sqrt( pow(Asteroide_reference->demiPetitAxe*cos(time*2*M_PI/Asteroide_reference->fullOrbitTime+Asteroide_reference->dephasage),2) + pow(Asteroide_reference->demiGrandAxe*sin(time*2*M_PI/Asteroide_reference->fullOrbitTime+Asteroide_reference->dephasage),2));
	Asteroide_reference->x_cartesien= Asteroide_reference->demiGrandAxe * cos(time*2*M_PI / Asteroide_reference->fullOrbitTime + Asteroide_reference->dephasage);
	Asteroide_reference->y_cartesien= Asteroide_reference->demiPetitAxe * sin(time*2*M_PI / Asteroide_reference->fullOrbitTime + Asteroide_reference->dephasage);
}
void GlobalPlanetAvancement (double time, struct donneePlanet * planets, int lenght){
	//Cette fonction permet de faire bouger la position de toutes les planetes en fontion de temps 
	for (int i=1; i<lenght; i++){
		avancementParametrisation(time,&planets[i]);
	}
}
void gravitationalForce (double gravitationalConstant, struct donneePlanet * planet, struct meteorite * meteor, struct forceCaract * force){
	//Cette fonction permet de calculer l'intensitée et la direction de la force qu'exerce un astre sur le meteor au temps t
	meteor->distancePlanet = sqrt(pow(planet->x_cartesien - meteor->x_cartesien,2)+pow(planet->y_cartesien - meteor->y_cartesien,2));
	planet->distanceAsteroide = meteor->distancePlanet;
	double intensitee = gravitationalConstant * planet->masse * meteor->masse / pow(meteor->distancePlanet,2);
	force->intensitee+=intensitee;
	if ( planet->y_cartesien == meteor->y_cartesien ) {
		if ( planet->x_cartesien > meteor->x_cartesien) force->direction_x += intensitee;
		else force-> direction_x -= intensitee;
	}
	else{
		double angle = atan ( (planet->x_cartesien - meteor->x_cartesien) / (planet->y_cartesien - meteor->y_cartesien) );
		if (planet->y_cartesien < meteor -> y_cartesien) angle = -(M_PI-angle);
		force->direction_y += intensitee*cos(angle);
		force->direction_x += intensitee*sin(angle);
	}
}
struct forceCaract AdditionGravitationalForce (double gravitationalConstant, struct donneePlanet * planets,int lenght, struct meteorite * meteor){
	//Cette fonction cacule la somme de toutes les forces que subit l'asteroide au temps t
	struct forceCaract forceOnMeteor = {0,0,0};
	for (int i=1; i<lenght; i++){
		gravitationalForce(gravitationalConstant,&planets[i],meteor,&forceOnMeteor);
	}
	return forceOnMeteor;
}
void applicationForceMeteor ( double time, struct meteorite * meteor, struct forceCaract * forceOnMeteor){
	//Cette fontion permet de calculer les nouveaux parametres de l'asteroide après application de la force
	double timeInSec= time *3600 *24;
	meteor->previous_x_cartesien = meteor->x_cartesien;
	meteor->previous_y_cartesien = meteor->y_cartesien;
	meteor->x_cartesien= forceOnMeteor->direction_x * pow(timeInSec,2) / (2*meteor->masse) + meteor->initialspeed_x * timeInSec + meteor->x_cartesien;
	meteor->y_cartesien= forceOnMeteor->direction_y * pow(timeInSec,2) / (2*meteor->masse) + meteor->initialspeed_y * timeInSec + meteor->y_cartesien;
	meteor->initialspeed_x= forceOnMeteor->direction_x * timeInSec / meteor->masse + meteor->initialspeed_x;
	meteor->initialspeed_y= forceOnMeteor->direction_y * timeInSec / meteor->masse + meteor->initialspeed_y;
	meteor->distanceSoleil = sqrt(pow(meteor->x_cartesien,2)+pow(meteor->y_cartesien,2)); 
	meteor->vitesse_tot = sqrt(pow(meteor->initialspeed_x,2) + pow(meteor->initialspeed_y,2));

}
void conditionCollision (double timeInterval, struct donneePlanet * planets, int length, struct meteorite * meteor){
	//Cette fonction explique en detail comment on peut trouver une collision entre l'asteroide et un astre du systeme solaire
	for (int i=1; i<length; i++){
		
		if (planets[i].radius + meteor->radius >= planets[i].distanceAsteroide){
			meteor->collision = 1;
			meteor->collisionWith = planets[i].planetName;
			meteor->number_of_pl = i;
		}
			
			if (strcmp(planets[i].planetName,"soleil") == 0) {
					double coeff_directeur_as = (meteor->y_cartesien - meteor->previous_y_cartesien) / (meteor->x_cartesien - meteor->previous_x_cartesien);
					double b_directeur_as = meteor->y_cartesien - coeff_directeur_as * meteor->x_cartesien;
					double coeff_directeur_normal = -1 / coeff_directeur_as;
					double x_found = (b_directeur_as) / (coeff_directeur_as - coeff_directeur_normal);
					double y_found = coeff_directeur_as * x_found + b_directeur_as;
					if (pow(x_found,2)+pow(y_found,2)<=pow(planets[i].radius,2) && ((x_found < meteor->x_cartesien && x_found > meteor->previous_x_cartesien) || (x_found > meteor->x_cartesien && x_found < meteor->previous_x_cartesien))){
						meteor->collision = 1;
						meteor->collisionWith = planets[i].planetName;
						meteor->number_of_pl = i;
					}
				}
				
			else{
					//initialisation droite de la planete
					double coeff_directeur_pl = (planets[i].y_cartesien - planets[i].previous_y_cartesien) / (planets[i].x_cartesien - planets[i].previous_x_cartesien);
					double b_directeur_pl = planets[i].y_cartesien - coeff_directeur_pl * planets[i].x_cartesien;
					//initialisation droite de l'asteroide
					double coeff_directeur_as = (meteor->y_cartesien - meteor->previous_y_cartesien) / (meteor->x_cartesien - meteor->previous_x_cartesien);
					double b_directeur_as = meteor->y_cartesien - coeff_directeur_as * meteor->x_cartesien;
					//point d'intersection des 2 droites
					double x_found = (b_directeur_pl - b_directeur_as) / (coeff_directeur_as - coeff_directeur_pl);
					double y_found = coeff_directeur_as * x_found + b_directeur_as;
					//Il faut que le point soit dans l'interval du déplacement
					if ( (x_found < meteor->x_cartesien && x_found > meteor->previous_x_cartesien) || (x_found > meteor->x_cartesien && x_found < meteor->previous_x_cartesien) ){
							//distance du point par rapport à l'asteroide
							double dist_found_as = sqrt( pow( x_found - meteor->previous_x_cartesien,2) + pow(y_found - meteor->previous_y_cartesien,2) );
							double dist_found_pl = sqrt( pow( x_found - planets[i].previous_x_cartesien,2) + pow(y_found - meteor->previous_y_cartesien,2) );
							//distance que parcours la planete/ l'asteroide entre 2 points et leurs vitesses
							double dist_tot_as = sqrt( pow(meteor->x_cartesien - meteor->previous_x_cartesien,2) + pow(meteor->y_cartesien - meteor->previous_y_cartesien,2) );
							double dist_tot_pl = sqrt(pow(planets[i].x_cartesien - planets[i].previous_x_cartesien,2) + pow(planets[i].y_cartesien - planets[i].previous_y_cartesien,2));
							double speed_as = dist_tot_as / timeInterval;
							double speed_pl = dist_tot_pl / timeInterval;
							//temps pour que l'asteroide/la planete atteigne le point
							double time_found_as = dist_found_as / speed_as;
							double time_found_pl = dist_found_pl / speed_pl;
							//position de la planete quand l'asteroide est à l'intersection
							double vector_pl_vx = (meteor->x_cartesien - meteor->previous_x_cartesien) / dist_tot_pl * speed_pl;
							double vector_pl_vy = (meteor->y_cartesien - meteor->previous_y_cartesien) / dist_tot_pl * speed_pl;
							double vector_pl_x = vector_pl_vx * time_found_as;
							double vector_pl_y = vector_pl_vy * time_found_as;
							double x_planet_t = planets[i].previous_x_cartesien + vector_pl_x;
							double y_planet_t = planets[i].previous_y_cartesien + vector_pl_y;
							//position de l'asteroide quand la planete est a l'intersection
							double vector_as_vx = (planets[i].x_cartesien - planets[i].previous_x_cartesien)/ dist_tot_as * speed_as;
							double vector_as_vy = (planets[i].y_cartesien - planets[i].previous_y_cartesien)/ dist_tot_as * speed_as;
							double vector_as_x = vector_as_vx * time_found_pl;
							double vector_as_y = vector_as_vy * time_found_pl;
							double x_asteroid_t = meteor->previous_x_cartesien + vector_as_x;
							double y_asteroid_t = meteor->previous_y_cartesien + vector_as_y;
							//distance de la planete/asteroide et l'asteroide/planete quand l'asteroide/planete est au point
							double dist_aspl_as = sqrt( pow(x_found - x_planet_t,2) + pow(y_found - y_planet_t,2));
							double dist_aspl_pl = sqrt( pow(x_found - x_asteroid_t,2) + pow(y_found - y_asteroid_t,2));
							
							double sum_radius = meteor->radius + planets[i].radius;
							if ( dist_aspl_as < sum_radius + 50000 || dist_aspl_pl < sum_radius + 50000){
								meteor->collision = 1;
								meteor->collisionWith = planets[i].planetName;
								meteor->number_of_pl = i;
							}
						}	
				}
}
}
void enregistre_coordonnees(int i, int length,  struct donneePlanet * planets, struct meteorite * meteor, struct donneePlanet * Asteroide_reference) {
	//Cette fonction permet d'enregistrer les coordonnées de chaque objets dans des tableaux au temps t 
	for(int j = 0; j<length; j++ ) {
		planets[j].tableau_coordonnees_X[i] = planets[j].x_cartesien; 
		planets[j].tableau_coordonnees_Y[i] = planets[j].y_cartesien; 
	}
	Asteroide_reference->tableau_coordonnees_X[i] = Asteroide_reference->x_cartesien; 
	Asteroide_reference->tableau_coordonnees_Y[i] = Asteroide_reference->y_cartesien; 
	meteor->tableau_coordonnees_X[i] = meteor->x_cartesien; 
	meteor->tableau_coordonnees_Y[i] = meteor->y_cartesien; 
	meteor->tableau_distance_soleil[i] = meteor->distanceSoleil; 
	meteor->tableau_vitesses[i] = meteor->vitesse_tot ;
}
void initialize_tables(int length, struct donneePlanet * planets, struct meteorite * meteor, struct donneePlanet * Asteroide_reference, int NombrePoints) {
	//Cette fonction initialise les tableaux de chaque astre pour ensuite enregistrer leurs coordonnées 
	for(int j = 0; j<length; j++ ) {
		planets[j].tableau_coordonnees_X  = malloc(NombrePoints * sizeof (double)); 
		planets[j].tableau_coordonnees_Y = malloc(NombrePoints * sizeof (double)); 
	}
	Asteroide_reference->tableau_coordonnees_X = malloc(NombrePoints * sizeof (double));
	Asteroide_reference->tableau_coordonnees_Y = malloc(NombrePoints * sizeof (double));
	meteor->tableau_coordonnees_X = malloc(NombrePoints * sizeof (double));
	meteor->tableau_coordonnees_Y = malloc(NombrePoints * sizeof (double));
	meteor->tableau_distance_soleil = malloc(NombrePoints * sizeof (double));
	meteor->tableau_vitesses= malloc(NombrePoints * sizeof (double));
}
void free_tables(int length, struct donneePlanet * planets, struct meteorite * meteor, struct donneePlanet * Asteroide_reference, int NombrePoints) {
	//Cette fonction permet de libéré la mémoire des tableaux de données précédemment créé
	for(int j = 0; j<length; j++ ) {
		free(planets[j].tableau_coordonnees_X);
		free(planets[j].tableau_coordonnees_Y); 
	}
	free(Asteroide_reference->tableau_coordonnees_X);
	free(Asteroide_reference->tableau_coordonnees_Y);
	free(meteor->tableau_coordonnees_X);
	free(meteor->tableau_coordonnees_Y);
	free(meteor->tableau_distance_soleil);
	free(meteor->tableau_vitesses);
}
int graduation_kuiper (double debut_kuiper, double fin_kuiper, struct meteorite * asteroid, struct Image * image){
	//Cette fonction permet de lire et interprété notre image de probabilité de colision en donnant la couleur du pixel correspondant aux données de l'asteroide
	double speed_as = sqrt( pow( asteroid->initialspeed_x,2) + pow( asteroid->initialspeed_y,2) );
	double interval_kuiper = fin_kuiper - debut_kuiper;
	double max_speed = 10;
	double position_x = asteroid->distanceSoleil - debut_kuiper;
	int graduation_x = floor( position_x / interval_kuiper * image->width );
	int graduation_v = image->height -1 - floor( speed_as /max_speed * image->height );
	
	int pixel_number = graduation_x + graduation_v * image->width ;
	return pixel_number;
}
double echelle_kuiper (double proba_list [], struct meteorite * asteroid, struct Image * echelle, struct Pixel * pixel){
	//Cette fonction nous donne la probabilité de collision de l'asteroide dans la ceinture de kuiper en interprétant notre échelle de proba
	double point = 0;
	int ecart = 0;
	while (point == 0){
		ecart +=10;
		for (int i=0; i<echelle->height; i++){
			int nbr = echelle->width * i;
			if ( (echelle->pixels[nbr].r < pixel->r+ecart && echelle->pixels[nbr].r > pixel->r-ecart) && (echelle->pixels[nbr].g < pixel->g+ecart && echelle->pixels[nbr].g > pixel->g-ecart) && (echelle->pixels[nbr].b < pixel->b+ecart && echelle->pixels[nbr].b > pixel->b-ecart)){
				point = nbr;
			}
		}
	}
	int proba_graduation = floor ( point / (echelle->width * echelle->height) * 6);
	double proba = proba_list[proba_graduation];
	return proba;
}
void proba_collision (double proba, struct meteorite * asteroid){
	//fonction permettant de créer l'alétoire de la collision grâce a une proba donnée
	int number = floor(1/proba);
	double randomDomaine = RAND_MAX + 1.0;
	rand();
	int a = (int) rand()/randomDomaine * number; 
	int b = (int) rand()/randomDomaine * number;
	//si a=b alors il y a collision avec la ceinture de kuiper
	if (a==b){
		asteroid->collision = 1;
		asteroid->collisionWith = "kuiper belt";
		asteroid->number_of_pl = 15;
	}
}
int repetitionDeFonctions (int NombrePointsMax, double timeInterval, double gravitationalConstant, struct donneePlanet * planets, int lenght, struct meteorite * meteor,  struct donneePlanet * Asteroide_reference){
	//fonction qui apelle les fonctions précédentes a des temps différents afin de faire bouger tout le systeme
	double time = timeInterval;
	int i = 0; 
	//Parametrisation de la ceinture de kuiper
	char * fichier1 = "image_kuiper.jpg";
	char * fichier2 = "echelle_kuiper.jpg";
	struct Image image;
	image_read(&image, fichier1);
	struct Image echelle;
	image_read(&echelle, fichier2);
	double debut_kuiper = 4500000000;
	double fin_kuiper = 7500000000;
	double proba_list [] = { pow(10,-2) , pow(10,-3), pow(10,-4), pow(10,-5), pow(10,-6), pow(10,-7), pow(10,-8)};
		
	while ( 0 == meteor->collision && i< NombrePointsMax){
		GlobalPlanetAvancement(time, planets, lenght);
		avancementParametrisation_Asteroid(time, Asteroide_reference);
		struct forceCaract forceOnMeteor = AdditionGravitationalForce (gravitationalConstant, planets, lenght, meteor);
		applicationForceMeteor(timeInterval, meteor, &forceOnMeteor);
		conditionCollision (timeInterval,planets, lenght, meteor);
		enregistre_coordonnees(i, lenght, planets, meteor, Asteroide_reference);
		
		//collision avec ceinture de kuiper
		if ( meteor->distanceSoleil > debut_kuiper && meteor->distanceSoleil < fin_kuiper ){
			if ( meteor->vitesse_tot < 10){
				int pixel_number = graduation_kuiper (debut_kuiper, fin_kuiper , meteor, &image); 
				double proba = echelle_kuiper( proba_list, meteor, &echelle, &image.pixels[pixel_number] );
				proba_collision (proba, meteor);
			}
			else {
				double proba = proba_list[6];
				proba_collision (proba, meteor);
			}
		}
		else if ( meteor->distanceSoleil > fin_kuiper){
			meteor->collision = 1;
			meteor->collisionWith = "L'astéroide à quitté le système solaire";
			meteor->number_of_pl = 16;
		}
		
		//printf("Meteor Position: (%f , %f)\n", meteor->x_cartesien,meteor->y_cartesien);
		time+=timeInterval;
		i+=1; 
	}
	//Libération des fichiers image
	image_free(&echelle);
	image_free(&image);
	return i; 
}
int lireLigne(char * ligne, struct donneePlanet * planets) {
	//Cette fonction permet de lire une ligne de notre tableau de donnée et d'affecter les valeurs aux endroids correspondant
	char * virgule1 = strchr(ligne, ',');
	if (virgule1 == NULL) return 0;
	char * virgule2 = strchr(virgule1 + 1, ',');
	if (virgule2 == NULL) return 0;
	char * virgule3 = strchr(virgule2 + 1, ',');
	if (virgule3 == NULL) return 0;
	char * virgule4 = strchr(virgule3 + 1, ',');
	if (virgule4 == NULL) return 0;
	char * virgule5 = strchr(virgule4 + 1, ',');
	if (virgule5 == NULL) return 0;
	char * virgule6 = strchr(virgule5 + 1, ',');
	if (virgule6 == NULL) return 0;
	char * virgule7 = strchr(virgule6 + 1, ',');
	if (virgule7 == NULL) return 0;
	int len = virgule1 - ligne;
	strncpy(planets->planetName, ligne, len);
	planets->planetName[len]='\0';;
	planets->demiGrandAxe = atof(virgule1 + 1);
	planets->demiPetitAxe = atof(virgule2 + 1);
	planets->Vrai_demiGrandAxe = atof(virgule3 + 1);
	planets->masse = atof(virgule4 + 1);
	planets->radius = atof(virgule5 + 1);
	planets->fullOrbitTime = atof(virgule6 + 1);
    return 1;
   }
int lireFichier(char * nomFichier, struct donneePlanet * tableauARemplir, int longueur) {
	//Cette fonction permet de lire le fichiers csv et de nous donner toutes les informations dont nous avons besoin
    FILE * file = fopen(nomFichier, "r");
    if (file == NULL) return -1;
    int ligne = 0;
    char buffer[100];
    while (fgets(buffer, 100, file) != NULL) {
        if (ligne >= longueur) return ligne;
        int ok = lireLigne(buffer, tableauARemplir + ligne);
        if (ok) ligne = ligne + 1;
    }
    fclose(file);
    return ligne;
}
void comparaison_vraie_asteroide(double time, double gravitationalConstant, struct meteorite * meteor, struct donneePlanet * Asteroide_reference,struct donneePlanet * planets ) { 
	//Asteroide_reference->x_cartesien = Asteroide_reference->demiGrandAxe;
	//Asteroide_reference->y_cartesien = 0; 
	
	planets[9].x_cartesien = planets[9].demiGrandAxe; 
	planets[9].y_cartesien = 0; 
	avancementParametrisation(time, &planets[9]);
	//avancementParametrisation(time, Asteroide_reference);
	//double delta_x = Asteroide_reference->x_cartesien - Asteroide_reference->previous_x_cartesien; 
	//double delta_y = Asteroide_reference->y_cartesien - Asteroide_reference->previous_y_cartesien; 
	
	double delta_x = planets[9].x_cartesien - planets[9].previous_x_cartesien; 
	double delta_y = planets[9].y_cartesien - planets[9].previous_y_cartesien; 
	double vx_ini = delta_x / (time * 3600 * 24); 
	double vy_ini = delta_y / (time * 3600 * 24); 
	
	
	//double V_test = sqrt(gravitationalConstant* 1.989 * pow(10,30)/Asteroide_reference->demiGrandAxe);
	
	//double v_norme = sqrt(pow(vx_ini,2) + pow(vy_ini,2));
	//initialising the meteors orbital properties and initial postion with Asteroid_reference's
	meteor->x_cartesien= -planets[9].demiGrandAxe;
	meteor->y_cartesien= 13000;
	meteor->initialspeed_x= 1.41*vx_ini; 
	meteor->initialspeed_y= 0.678*vy_ini;
	meteor->distanceSoleil=planets[9].distanceSoleil;
	meteor->masse= planets[9].masse;
	meteor->radius= planets[9].radius;
	
	// reinitialise Asteroide_reference cooordinates before launching avancementParametrisation 
	
	//Asteroide_reference->x_cartesien = Asteroide_reference->demiGrandAxe; 
	//Asteroide_reference->y_cartesien = 0;
	planets[9].x_cartesien = planets[9].demiGrandAxe; 
	planets[9].y_cartesien = 0; 
	//Asteroide_reference->y_cartesien = 0;
}
void PrintCoordinates(struct donneePlanet * planets,struct donneePlanet * Asteroide_reference, struct meteorite * meteor, int lenght, int nbPoints, double interval_time, int vitesse_sim) {
	//Cette fonction print les valeurs des tableaux dans le terminal qui vont ensuite être récupérées par le fichier python pour la simulation
	for(int j=1; j<lenght; j++) {
		for(int i=0;i<nbPoints;i++) {
			printf ("%f\t",planets[j].tableau_coordonnees_X[i]); 
		}
		printf("\n"); 
		
		for(int i=0;i<nbPoints;i++) {
			printf ("%f\t",planets[j].tableau_coordonnees_Y[i]); 
		}
		printf("\n"); 
		
	}
	for(int i=0;i<nbPoints;i++) {
		printf ("%f\t",meteor->tableau_coordonnees_X[i]); 
	}
	printf("\n"); 
	for(int i=0;i<nbPoints;i++) {
		printf ("%f\t",meteor->tableau_coordonnees_Y[i]); 
	}
	printf("\n"); 
	
	for(int i=0;i<nbPoints;i++) {
		printf ("%f\t",Asteroide_reference->tableau_coordonnees_X[i]); 
	}
	printf("\n"); 
	
	for(int i=0;i<nbPoints;i++) {
		printf ("%f\t",Asteroide_reference->tableau_coordonnees_Y[i]); 
	}
	
	printf("\n"); 
	printf("%f \n", interval_time);  // pour récupérer l'intervalle de temps sur python 
	printf("%d", nbPoints); // récupérer le nombre de points (est différent de NombrePointsMax si il y a eu collision avec un astre) 
	printf("\n"); 
	
	for(int i=0;i<nbPoints;i++) {
		printf ("%f\t",meteor->tableau_distance_soleil[i]); 
	}	
	printf("\n"); 
	
	for(int i=0;i<nbPoints;i++) {
		printf ("%f\t",meteor->tableau_vitesses[i]); 
	}	
	printf("\n"); 
	
	printf("%d", meteor->number_of_pl);
	
	printf("\n"); 
	printf("%d", vitesse_sim);
	
}


int main(int argc, char * argv[]) {
	srand(time(NULL));
	
	double gravitationalConstant = 6.67408*pow(10,-20);
	double interval_time = 1;  //en jours
	int NombrePointsMax = 10000;
	int vitesse_sim = 1;
	
	struct donneePlanet planets[15]; // j'ai mit à 15
	int lenght = lireFichier("bodies-3.csv", planets, 15); // 15 !!!!! 
	
	// L'astéroide de référence suivant nous permet de vérifier la trajectoire de notre ast. modélisé en comparant les deux trajectoires 
	struct donneePlanet Asteroide_reference;
	strcpy(Asteroide_reference.planetName, "Benu");
	Asteroide_reference.demiGrandAxe = 202.84 * pow(10,6); // in km 
	Asteroide_reference.demiPetitAxe = 134.173 * pow(10,6); // in km
	Asteroide_reference.masse = 7.329 * pow(10,10); //in kg
	Asteroide_reference.radius = 0.28237; //in km 
	Asteroide_reference.fullOrbitTime = 436.65; //in days
	
	for (int i = 1; i<lenght; i++) {
		planets[i].dephasage = 0;
	}
	
	struct meteorite asteroid;
	asteroid.number_of_pl = 0;
	initialize_tables(lenght, planets, &asteroid, &Asteroide_reference, NombrePointsMax);
	
	int Mode;
    fprintf(stderr, " Choisissez un scénario: \n\t 1: Modèle de collision \n\t 2: Collision avec la ceinture de kuiper (limite extérieure) \n\t 3: Modèle de trajectoire prédéfini \n\t 4: Mode libre (choix de toutes les données) \n");
    scanf("%d", &Mode);
    
    if (Mode == 1){			//collision
		int n_collision;
		fprintf(stderr, " Ce scenario permet de vérifier notre système de collision en prenant des exemples de collision que nous avons calculés \n Toutes les données sont déja rentrées pour montrer une collision en particulier \n À vous d'en trouver d'autres sur le mode libre ;)");
		fprintf(stderr, "\n Choisissez une collision parmi: \n\t 1: Soleil \n\t 2: Ceres \n");
		scanf("%d", &n_collision);
		
		if (n_collision == 1){
			asteroid.x_cartesien = -100000000;
			asteroid.y_cartesien = 120000000;
			asteroid.initialspeed_x = 0;
			asteroid.initialspeed_y = 0;
			asteroid.radius = 200;
			asteroid.masse = 2 * pow(10,12) * 4/3 * M_PI * pow(asteroid.radius,3);
			interval_time = 0.1;
			
		}
		else if (n_collision == 2){
			interval_time = 0.1;
			NombrePointsMax = 10000;
			planets[1].dephasage = 2*M_PI * 35.5 / 1868.48;
			asteroid.masse = 8 * pow(10,10); 
			asteroid.radius = 20; 
			asteroid.initialspeed_x = 30; 
			asteroid.initialspeed_y = -3; 
			asteroid.x_cartesien = -10*pow(10,8);
			asteroid.y_cartesien = 30000;
		}
	}
	
	else if (Mode == 2){		//kuiper
		asteroid.x_cartesien = 4490000000; 
		asteroid.y_cartesien = 0;
		global_dephasage(planets, lenght);
		fprintf(stderr, " Vous devez choisir une valeur positive pour la vitesse initiale de l'asteroide; \n Si vous entrez v0 > 6km/s l'asteroide va souvent sortir du systeme solaire \n Si vous entrez v0 < 2km/s l'asteroide va souvent collisionner dans la ceinture de Kuiper\n On a une bonne répartition des cas pour v0 entre 2 et 3Km/s\n v0 : ");
		scanf("%lf", &asteroid.initialspeed_x);
		fprintf(stderr, " Taille de l'astéroide [km]: \n (Indication : Les rayons des asteroides connus varient entre 0 et 1000km) \n (Culture : l'asteroide Chicxulub qui a causé l'extinction des dinosaures avait un rayon de seulemnt 12km) \n\t rayon = ");
		scanf("%lf",&asteroid.radius);
		asteroid.masse = 2 * pow(10,12) * 4/3 * M_PI * pow(asteroid.radius,3);
		fprintf(stderr," Intervalle de temps entre deux points [jour]: \n (Indication : c'est mieux de le faire avec un intervalle entre 0.1 et 2 jours)\n");
		scanf("%lf", &interval_time);
		
	}
	
	else if (Mode == 3){		//ellipse
		fprintf(stderr, " Le but de ce scenario est de montrer que notre modélisation physique est proche de la réalité et que l'asteroide suit bien une ellipse de référence\n On suivra ici la trajectoire de Mars\n");
		//int * poubelle;
		//scanf("%d",poubelle);
		
		comparaison_vraie_asteroide(interval_time, gravitationalConstant, &asteroid, &Asteroide_reference, planets);
	}
	
	else if (Mode == 4){		//manuel
		fprintf(stderr, " Sur ce mode libre, toutes les données (de l'asteroide) sont modifiables, à vous de tester ce que vous voulez!\n");
		fprintf(stderr, " Position initiale de l'asteroide [km]: \n (Indication : le systeme solaire s'étend sur un rayon de 75*10^8 km autour du soleil,qui a un rayon de 7*10^5 km)\n (Culture : la Terre se trouve en moyenne a 150 000 000 km du soleil\n");
		int condition = 0;
		while (condition == 0){
			fprintf(stderr, "\t x0 = ");
			scanf("%lf",&asteroid.x_cartesien);
			fprintf(stderr, "\t y0 = ");
			scanf("%lf",&asteroid.y_cartesien);
			asteroid.distanceSoleil = sqrt( pow(asteroid.x_cartesien,2) + pow( asteroid.y_cartesien,2));
			if (asteroid.distanceSoleil < 7*pow(10,5) || asteroid.distanceSoleil > 75*pow(10,8)){
				fprintf(stderr," Vous avez placé votre asteroide soit dans le soleil soit en dehors du systeme solaire,\n Veuillez rentrer d'autres mesures\n");
			}
			else condition = 1;
		}
		
		fprintf(stderr, " Vitesse initiale de l'asteroide [km/s]: \n (Indication : la vitesse moyenne des asteroides du systeme solaire est de 5km/s)\n (Culture : le dernier asteroide qui à 'frôlé' la terre avait une vitesse totale de 12.4km/s)\n\t v0_x = ");
		scanf("%lf",&asteroid.initialspeed_x);
		fprintf(stderr, "\t v0_y = ");
		scanf("%lf",&asteroid.initialspeed_y);
		fprintf(stderr, " Taille de l'asteroide [km]: \n (Indication : Les rayons des asteroides connus varient entre 0 et 1000km) \n (Culture : l'asteroide Chicxulub qui a cause l'extinction des dinosaures n'avais un rayon que de 12km) \n\t rayon = ");
		scanf("%lf",&asteroid.radius);
		fprintf(stderr, " Type de l'asteroide (Densite) [g.cm^-3]:\n (Indication : Dans le systeme solaire il y a 75%% d'asteroide de type C, 17%% de type S et 8%% de type M)\n (Culture : Les inuits utilisaient les debris d'asteroides de type M (composé de fer) pour fabriquer leurs outils)\n");
		fprintf(stderr, "\t 1: type C (carbonné), 2 g.cm^-3\n\t 2: type S (silicaté), 2.5 g.cm^-3\n\t 3: type M (metalique), 8 g.cm^-3\n");
		int type;
		scanf("%d", &type);
		if (type == 1){
			asteroid.masse = 2 * pow(10,12) * 4/3 * M_PI * pow(asteroid.radius,3);
		}
		else if (type == 2){
			asteroid.masse = 2.5 * pow(10,12) * 4/3 * M_PI * pow(asteroid.radius,3);
		}
		else if (type ==3){
			asteroid.masse = 8 * pow(10,12) * 4/3 * M_PI * pow(asteroid.radius,3);
		}
		fprintf(stderr, " Position initiale des planètes : \n\t 1: position aleatoire sur leurs ellipses \n\t 2: les planetes sont disposées aux positions du 9/12/2020 \n\t 3: position allignée sur l'axe y=0\n");
		int angle;
		scanf("%d", &angle);
		if (angle == 1){
			global_dephasage(planets, lenght);
		}
		else if (angle ==2){
			double dephasage_list [] = {-M_PI/2,-M_PI/3,-M_PI/4,-3*M_PI/4,-M_PI/2,3*M_PI/4,2*M_PI/3,-3*M_PI/4,-M_PI/6,5*M_PI/6,-3*M_PI/4,0,0,2*M_PI/3};
			for (int i=1; i<lenght; i++){
				planets[i].dephasage = dephasage_list[i];
			}
		}
		fprintf(stderr," Intervalle de temps entre deux points [jour]: \n (Indication : c'est mieux de le faire avec un intervalle entre 0.1 et 2 jours)\n");
		scanf("%lf", &interval_time);
		fprintf(stderr," Temps de simulation [jour]: \n (Indication : 10 000 jours pour un intervalle de temps de 1 jour est bien pour ne pas avoir une simulation trop longue) \n (Culture : un an sur Neptune, qui est la planete la plus eloignée du soleil, dure 60 189 jours)\n");
		int temps;
		scanf("%d", &temps);
		NombrePointsMax = temps / interval_time;
		fprintf(stderr," Vitesse de simulation :\n (Indication : vitesse x1, vitesse x2, vitesse x3, vitesse x4)\n\t\t\t Bonne simulation! \n\t vitesse x");
		scanf("%d", &vitesse_sim);
	}
	
	int NombrePointsWhile = repetitionDeFonctions(NombrePointsMax,interval_time,gravitationalConstant,planets,lenght,&asteroid, &Asteroide_reference);	
	PrintCoordinates(planets,&Asteroide_reference,&asteroid,lenght, NombrePointsWhile, interval_time, vitesse_sim); // boucle while
	free_tables(lenght, planets, &asteroid, &Asteroide_reference, NombrePointsMax);

	return 0; 
	
}
