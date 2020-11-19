#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


struct donneePlanet {
	char planetName[100];			//"name"
	double demiGrandAxe; 		//ou "aphelion"
	double demiPetitAxe; 		//ou "perihelion"
	double masse; 					// "massValue" + "massExponent"
	double radius;					//"equaRadius"
	double fullOrbitTime;		//"sideralOrbit"
	double gravity; 				//si on est chaud
	double rotationTime;			//si on est chaud
	double distanceSoleil;
	double x_cartesien;
	double previous_x_cartesien;
	double y_cartesien;
	double previous_y_cartesien;
	double distanceAsteroide;
	double dephasage;
	double Vrai_demiGrandAxe;
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
	_Bool collision;
	char * collisionWith;
	int number_of_pl;
};
struct forceCaract{
	double direction_x;
	double direction_y;
	double intensitee;
};
double random_radian() {
	double radian =((double)rand()) / ((double)RAND_MAX)*2*M_PI;
	return radian; 
}
void aphel_dephasage(struct donneePlanet * planet){
	srand(time(NULL));
	random_radian();
	planet->dephasage = random_radian(); 	
}
void global_dephasage (struct donneePlanet * planets, int lenght){
	for (int i=1; i<lenght; i++){
		aphel_dephasage(&planets[i]);
	}
}
void avancementParametrisation ( int time, struct donneePlanet * planet){
	planet->previous_x_cartesien = planet->x_cartesien;
	planet->previous_y_cartesien = planet->y_cartesien;
	planet->distanceSoleil = planet->demiPetitAxe * planet->demiGrandAxe / sqrt( pow(planet->demiPetitAxe*cos(time*2*M_PI/planet->fullOrbitTime+planet->dephasage),2) + pow(planet->demiGrandAxe*sin(time*2*M_PI/planet->fullOrbitTime+planet->dephasage),2));
	planet->x_cartesien= planet->demiGrandAxe * cos(time*2*M_PI / planet->fullOrbitTime + planet->dephasage);
	planet->y_cartesien= planet->demiPetitAxe * sin(time*2*M_PI / planet->fullOrbitTime + planet->dephasage);
}
void avancementParametrisation_Asteroid ( int time, struct donneePlanet * Asteroide_reference){
	Asteroide_reference->previous_x_cartesien = Asteroide_reference->x_cartesien;
	Asteroide_reference->previous_y_cartesien = Asteroide_reference->y_cartesien;
	Asteroide_reference->distanceSoleil = Asteroide_reference->demiPetitAxe * Asteroide_reference->demiGrandAxe / sqrt( pow(Asteroide_reference->demiPetitAxe*cos(time*2*M_PI/Asteroide_reference->fullOrbitTime+Asteroide_reference->dephasage),2) + pow(Asteroide_reference->demiGrandAxe*sin(time*2*M_PI/Asteroide_reference->fullOrbitTime+Asteroide_reference->dephasage),2));
	Asteroide_reference->x_cartesien= Asteroide_reference->demiGrandAxe * cos(time*2*M_PI / Asteroide_reference->fullOrbitTime + Asteroide_reference->dephasage);
	Asteroide_reference->y_cartesien= Asteroide_reference->demiPetitAxe * sin(time*2*M_PI / Asteroide_reference->fullOrbitTime + Asteroide_reference->dephasage);
}
void GlobalPlanetAvancement (int time, struct donneePlanet * planets, int lenght){
	for (int i=1; i<lenght; i++){
		avancementParametrisation(time,&planets[i]);
	}
	//printf("%f \n", planets[13].x_cartesien);
}
void gravitationalForce (double gravitationalConstant, struct donneePlanet * planet, struct meteorite * meteor, struct forceCaract * force){
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
	struct forceCaract forceOnMeteor = {0,0,0};
	for (int i=1; i<lenght; i++){
		gravitationalForce(gravitationalConstant,&planets[i],meteor,&forceOnMeteor);
		//printf("planet: %s\n",planets[i].planetName);
		//printf("force en X:%f\nforce en Y:%f\ntotal force:%f\n\n",forceOnMeteor.direction_x,forceOnMeteor.direction_y,forceOnMeteor.intensitee);
	}
	return forceOnMeteor;
}
void applicationForceMeteor ( int time, struct meteorite * meteor, struct forceCaract * forceOnMeteor){
	int timeInSec= time *3600 *24;
	printf("forcex= %f \n", forceOnMeteor->direction_x);
	printf("forcey= %f \n", forceOnMeteor->direction_y);
	meteor->previous_x_cartesien = meteor->x_cartesien;
	meteor->previous_y_cartesien = meteor->y_cartesien;
	meteor->x_cartesien= forceOnMeteor->direction_x * pow(timeInSec,2) / (2*meteor->masse) + meteor->initialspeed_x * time + meteor->x_cartesien;
	meteor->y_cartesien= forceOnMeteor->direction_y * pow(timeInSec,2) / (2*meteor->masse) + meteor->initialspeed_y * time + meteor->y_cartesien;
	meteor->initialspeed_x= forceOnMeteor->direction_x * timeInSec / meteor->masse + meteor->initialspeed_x;
	meteor->initialspeed_y= forceOnMeteor->direction_y * timeInSec / meteor->masse + meteor->initialspeed_y;
}
void conditionCollision (int timeInterval, struct donneePlanet * planets, int length, struct meteorite * meteor){
	for (int i=1; i<length; i++){
		
		if (planets[i].radius + meteor->radius >= planets[i].distanceAsteroide){
			meteor->collision = 1;
			meteor->collisionWith = planets[i].planetName;
			meteor->number_of_pl = i;
		}
		
		else{
			/*
			double coeff_directeur_as = (meteor->y_cartesien - meteor->previous_y_cartesien) / (meteor->x_cartesien - meteor->previous_y_cartesien);
			double b_directeur_as = meteor->y_cartesien - coeff_directeur_as * meteor->x_cartesien + meteor->radius;
			double coeff_normal = -1 / coeff_directeur_as;
			double b_normal = planets[i].y_cartesien - coeff_normal * planets[i].x_cartesien;
			double x_found = (b_normal - b_directeur_as) / (coeff_directeur_as - coeff_normal);
			printf("(%f , %f , %f)\n",meteor->previous_x_cartesien,x_found, meteor->x_cartesien);
			
			if (x_found < meteor->x_cartesien && x_found > meteor->previous_x_cartesien){
				meteor->collision = 1;
				meteor->collisionWith = planets[i].planetName;
			}
			*/
			if (planets[i].planetName[1] == 'o'){
				double coeff_directeur_as = (meteor->y_cartesien - meteor->previous_y_cartesien) / (meteor->x_cartesien - meteor->previous_x_cartesien);
				double b_directeur_as = meteor->y_cartesien - coeff_directeur_as * meteor->x_cartesien;
				double coeff_directeur_normal = -1 / coeff_directeur_as;
				double x_found = (b_directeur_as) / (coeff_directeur_as - coeff_directeur_normal);
				double y_found = coeff_directeur_as * x_found + b_directeur_as;
				if (pow(x_found,2)+pow(y_found,2)<=pow(planets[i].radius,2)){
						meteor->collision = 1;
						meteor->collisionWith = planets[i].planetName;
						meteor->number_of_pl = i;
						printf("found: ( %f , %f )\n",x_found,y_found);

			}
			else{

				double coeff_directeur_pl = (planets[i].y_cartesien - planets[i].previous_y_cartesien) / (planets[i].x_cartesien - planets[i].previous_x_cartesien);
				double b_directeur_pl = planets[i].y_cartesien - coeff_directeur_pl * planets[i].x_cartesien;
				double points[2];
				points[0] = meteor->radius;
				points[1] = -meteor->radius;
				for (int j=0; j<2; j++){
					double coeff_directeur_as = (meteor->y_cartesien - meteor->previous_y_cartesien) / (meteor->x_cartesien - meteor->previous_x_cartesien);
					double b_directeur_as = meteor->y_cartesien - coeff_directeur_as * meteor->x_cartesien + points[j];
					double x_found = (b_directeur_pl - b_directeur_as) / (coeff_directeur_as - coeff_directeur_pl);
					double y_found = coeff_directeur_as * x_found + b_directeur_as;
					if (x_found < meteor->x_cartesien && x_found > meteor->previous_x_cartesien){
						double dist_found = sqrt( pow( x_found - meteor->previous_x_cartesien,2) + pow(y_found - meteor->previous_y_cartesien,2) );
						double dist_tot = sqrt( pow(meteor->x_cartesien - meteor->previous_x_cartesien,2) + pow(meteor->y_cartesien - meteor->previous_y_cartesien,2) );
						double dist_tot_pl = sqrt(pow(planets[i].x_cartesien - planets[i].previous_x_cartesien,2) + pow(planets[i].y_cartesien - planets[i].previous_y_cartesien,2));
						double speed_as = dist_tot / timeInterval;
						double speed_pl = dist_tot_pl / timeInterval;
						double time_found = dist_found / speed_as;
						double vector_pl_vx = (meteor->x_cartesien - meteor->previous_x_cartesien)/dist_tot_pl*speed_pl;
						double vector_pl_vy = (meteor->y_cartesien - meteor->previous_y_cartesien)/dist_tot_pl*speed_pl;
						double vector_pl_x = vector_pl_vx * time_found;
						double vector_pl_y = vector_pl_vy * time_found;
						double x_planet_t = planets[i].previous_x_cartesien + vector_pl_x;
						double y_planet_t = planets[i].previous_y_cartesien + vector_pl_y;
						if (pow(x_planet_t-x_found,2)+pow(y_planet_t-y_found,2)<=pow(planets[i].radius,2)){
							meteor->collision = 1;
							meteor->collisionWith = planets[i].planetName;
							meteor->number_of_pl = i;
							printf("found: ( %f , %f )\n",x_found,y_found);
						}
					}
				}
			}
			
			/*double coeff_directeur_as = (meteor->y_cartesien - meteor->previous_y_cartesien) / (meteor->x_cartesien - meteor->previous_y_cartesien);
			double b_directeur_as1 = meteor->y_cartesien - coeff_directeur_as1 * meteor->x_cartesien + meteor->radius;
			double b_directeur_as2 = meteor->y_cartesien - coeff_directeur_as1 * meteor->x_cartesien - meteor->radius;
			double coeff_directeur_pl = (planets[i].y_cartesien - planets[i].previous_y_cartesien) / (planets[i].x_cartesien - planets[i].previous_x_cartesien);
			double b_directeur_pl = planets[i].y_cartesien - coeff_directeur_pl * planets[i].x_cartesien;
			double x_found_1 = (b_directeur_pl - b_directeur_as) / (coeffs_directeur_as - coeff_directeur_pl);
			double y_found_1 = coeff_directeur_as * x_found + b_directeur_as;
			double dist_found_1 = sqrt( pow( x_found - meteor->previous_x_cartesien,2) + pow(y_found - meteor->previous_y_cartesien,2) );
			double dist_tot = sqrt( pow(meteor->x_cartesien - meteor->previous_x_cartesien,2) + pow(meteor->y_cartesien - meteor->previous_y_cartesien,2) );
			double dist_tot_pl = sqrt(pow(planets[i].x_cartesien - planets[i].previous_x_cartesien,2) + pow(planets[i].y_cartesien - planets[i].previous_y_cartesien,2));
			double speed_as = dist_tot / timeInterval;
			double speed_pl = dist_tot_pl / timeInterval;
			double time_found_1 = dist_found_1 / speed_as;
			double vector_pl_vx = (meteor->x_cartesien - meteor->previous_x_cartesien)/dist_tot_pl*speed_pl;
			double vector_pl_vy = (meteor->y_cartesien - meteor->previous_y_cartesien)/dist_tot_pl*speed_pl;
			double vector_pl_x_1 = vector_pl_vx * time_found;
			double vector_pl_y_1 = vector_pl_vy * time_found;
			double x_planet_t = planets[i]->previous_x_cartesien + vector_pl_x_1;
			double y_planet_t = planets[i]->previous_y_cartesien + vector_pl_y_1;*/
		}
	}
}
}
void repetitionDeFonctions (int reps, int timeInterval, double gravitationalConstant, struct donneePlanet * planets, int lenght, struct meteorite * meteor,  struct donneePlanet * Asteroide_reference){
	int time = timeInterval;
	//while ( 0 == meteor->collision){
	for (int i=0; i<reps; i++){
		GlobalPlanetAvancement(time, planets, lenght);
		avancementParametrisation_Asteroid(time, Asteroide_reference);
		struct forceCaract forceOnMeteor = AdditionGravitationalForce (gravitationalConstant, planets, lenght, meteor);
		applicationForceMeteor(timeInterval, meteor, &forceOnMeteor);
		conditionCollision (timeInterval,planets, lenght, meteor);
		printf("Meteor Position: (%f , %f)\n", meteor->x_cartesien,meteor->y_cartesien);
		printf("RefAsteroidPosi: (%f , %f)\n \n",Asteroide_reference->x_cartesien, Asteroide_reference->y_cartesien);
		//printf("New Speed: (%f,%f)\n\n",meteor->initialspeed_x,meteor->initialspeed_y);
		time+=timeInterval;
		
	}
	printf("time=%d",time);
}
int lireLigne(char * ligne, struct donneePlanet * planets) {
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
	planets->gravity = atof(virgule7 + 1);
	
    return 1;
   }
  
int lireFichier(char * nomFichier, struct donneePlanet * tableauARemplir, int longueur) {
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
void comparaison_vraie_asteroide(int time, struct meteorite * meteor, struct donneePlanet * Asteroide_reference) { 
	Asteroide_reference->x_cartesien = Asteroide_reference->demiGrandAxe;
	Asteroide_reference->y_cartesien = 0; 
	avancementParametrisation(time, Asteroide_reference);
	double delta_x = Asteroide_reference->x_cartesien - Asteroide_reference->previous_x_cartesien; 
	double delta_y = Asteroide_reference->y_cartesien - Asteroide_reference->previous_y_cartesien; 
	double vx_ini = delta_x / time; 
	double vy_ini = delta_y / time; 
	//initialising the meteors orbital properties and initial postion with Asteroid_reference's
	meteor->x_cartesien= Asteroide_reference->demiGrandAxe;
	meteor->y_cartesien= 0;
	meteor->initialspeed_x= vx_ini;
	meteor->initialspeed_y= vy_ini;
	meteor->distanceSoleil=Asteroide_reference->distanceSoleil;
	meteor->masse= Asteroide_reference->masse;
	meteor->radius= Asteroide_reference->radius;
	
	// reinitialise Asteroide_reference cooordinates before launching avancementParametrisation 
	Asteroide_reference->x_cartesien = Asteroide_reference->demiGrandAxe; 
	Asteroide_reference->y_cartesien = 0;
}
int main(int argc, char * argv[]) {
	
	int time= 1;
	double gravitationalConstant = 6.6743*pow(10,-20);
	struct donneePlanet planets[14];
	int lenght = lireFichier("bodies-3.csv", planets, 14);
	
	// L'astéroide de référence suivant nous permet de vérifier la trajectoire de notre ast. modélisé en comparant les deux trajectoires 
	struct donneePlanet Asteroide_reference; 
	strcpy(Asteroide_reference.planetName, "Benu");
	Asteroide_reference.demiGrandAxe = 202.84 * pow(10,6); // in km 
	Asteroide_reference.demiPetitAxe = 134.173 * pow(10,6); // in km
	Asteroide_reference.masse = 7.329 * pow(10,10); //in kg
	Asteroide_reference.radius = 0.28237; //in km 
	Asteroide_reference.fullOrbitTime = 436.65; //in days
	printf("%f \n", Asteroide_reference.x_cartesien);
	
	struct meteorite meteor;
	comparaison_vraie_asteroide(time, &meteor,&Asteroide_reference);
	
	//printf("Asteroide_refe %f : %f \n ", Asteroide_reference.x_cartesien, Asteroide_reference.y_cartesien);
	//printf("Asteroide test %f : %f \n ", meteor.x_cartesien, meteor.y_cartesien);
	repetitionDeFonctions( 436,time,gravitationalConstant,planets,lenght,&meteor, &Asteroide_reference);
	return 0;
}
