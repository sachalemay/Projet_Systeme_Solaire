#include <stdio.h>
#include <math.h>
#include <stdbool.h>

struct donneePlanet {
	char * planetName;			//"name"
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
};

struct forceCaract{
	double direction_x;
	double direction_y;
	double intensitee;
};

void avancementParametrisation ( int time, struct donneePlanet * planet){
	planet->previous_x_cartesien = planet->x_cartesien;
	planet->previous_y_cartesien = planet->y_cartesien;
	planet->distanceSoleil = planet->demiPetitAxe * planet->demiGrandAxe / sqrt( pow(planet->demiPetitAxe*cos(time*2*M_PI/planet->fullOrbitTime+planet->dephasage),2) + pow(planet->demiGrandAxe*sin(time*2*M_PI/planet->fullOrbitTime+planet->dephasage),2));
	planet->x_cartesien= planet->demiGrandAxe * cos(time*2*M_PI / planet->fullOrbitTime + planet->dephasage);
	planet->y_cartesien= planet->demiPetitAxe * sin(time*2*M_PI / planet->fullOrbitTime + planet->dephasage);
}

void GlobalPlanetAvancement (int time, struct donneePlanet * planets, int lenght){
	for (int i=0; i<lenght; i++){
		avancementParametrisation(time,&planets[i]);
	}
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
	for (int i=0; i<lenght; i++){
		gravitationalForce(gravitationalConstant,&planets[i],meteor,&forceOnMeteor);
		//printf("X:%f\nY:%f\n%f\n\n",forceOnMeteor.direction_x,forceOnMeteor.direction_y,forceOnMeteor.intensitee);
	}
	return forceOnMeteor;
}

void applicationForceMeteor ( int time, struct meteorite * meteor, struct forceCaract * forceOnMeteor){
	int timeInSec= time *3600 *24;
	meteor->previous_x_cartesien = meteor->x_cartesien;
	meteor->previous_y_cartesien = meteor->y_cartesien;
	meteor->x_cartesien= forceOnMeteor->direction_x * pow(timeInSec,2) / (2*meteor->masse) + meteor->initialspeed_x * timeInSec + meteor->x_cartesien;
	meteor->y_cartesien= forceOnMeteor->direction_y * pow(timeInSec,2) / (2*meteor->masse) + meteor->initialspeed_y * timeInSec + meteor->y_cartesien;
	meteor->initialspeed_x= forceOnMeteor->direction_x * timeInSec / meteor->masse + meteor->initialspeed_x;
	meteor->initialspeed_y= forceOnMeteor->direction_y * timeInSec / meteor->masse + meteor->initialspeed_y;
}

void conditionCollision (int timeInterval, struct donneePlanet * planets, int length, struct meteorite * meteor){
	for (int i=0; i<length; i++){
		
		if (planets[i].radius + meteor->radius >= planets[i].distanceAsteroide){
			meteor->collision = 1;
			meteor->collisionWith = planets[i].planetName;
		}
		
		else{
			double coeff_directeur_as = (meteor->y_cartesien - meteor->previous_y_cartesien) / (meteor->x_cartesien - meteor->previous_y_cartesien);
			double b_directeur_as = meteor->y_cartesien - coeff_directeur_as * meteor->x_cartesien + meteor->radius;
			
			/*double coeff_normal = -1 / coeff_directeur;
			double b_normal = planets[i].y_cartesien - coeff_normal * planets[i].x_cartesien;
			double x_found = (b_normal - b_directeur) / (coeff_directeur - coeff_normal);
			printf("(%f , %f , %f)\n",meteor->previous_x_cartesien,x_found, meteor->x_cartesien);
			
			if (x_found < meteor->x_cartesien && x_found > meteor->previous_x_cartesien){
				meteor->collision = 1;
				meteor->collisionWith = planets[i].planetName;
			}*/
			
			double coeff_directeur_pl = (planets[i].y_cartesien - planets[i].previous_y_cartesien) / (planets[i].x_cartesien - planets[i].previous_x_cartesien);
			double b_directeur_pl = planets[i].y_cartesien - coeff_directeur_pl * planets[i].x_cartesien - planets[i].radius;
			double x_found = (b_directeur_pl - b_directeur_as) / (coeffs_directeur_as - coeff_directeur_pl);
			double y_found = coeff_directeur_as * x_found + b_directeur_as;
			double dist = sqrt( pow( x_found - meteor->previous_x_cartesien,2) + pow(y_found - meteor->previous_y_cartesien,2) );
			double dist_tot = sqrt( pow(meteor->x_cartesien - meteor->previous_x_cartesien,2) + pow(meteor->y_cartesien - meteor->previous_y_cartesien,2) );
			double dist_tot_pl = sqrt(pow(planets[i].x_cartesien - planets[i].previous_x_cartesien,2) + pow(planets[i].y_cartesien - planets[i].previous_y_cartesien,2));
			double speed_as = dist_tot / timeInterval;
			double speed_pl = dist_tot_pl / timeInterval;
			double time_found = dist / speed_as;
			
		}
	}
}

void repetitionDeFonctions (int reps, int timeInterval, double gravitationalConstant, struct donneePlanet * planets, int lenght, struct meteorite * meteor){
	int time = timeInterval;
	while ( 0 == meteor->collision){
	//for (int i=0; i<reps; i++){
		GlobalPlanetAvancement(time, planets, lenght);
		struct forceCaract forceOnMeteor = AdditionGravitationalForce (gravitationalConstant, planets, lenght, meteor);
		applicationForceMeteor(timeInterval, meteor, &forceOnMeteor);
		conditionCollision (timeInterval,planets, lenght, meteor);
		printf("New Position: (%f , %f)\n", meteor->x_cartesien,meteor->y_cartesien);
		printf("New Speed: (%f,%f)\n\n",meteor->initialspeed_x,meteor->initialspeed_y);
		time+=timeInterval;
		
	}
	printf("time=%d",time);
}

int main(int argc, char * argv[]) {
	
	int time= 10;
	double gravitationalConstant = 6.6743*pow(10,-20);
	
	struct donneePlanet mercure;
	mercure.planetName= "Mercure";
	mercure.demiGrandAxe= 69816900;
	mercure.demiPetitAxe= 46000000;
	mercure.fullOrbitTime=88;
	mercure.masse=3.3*pow(10,23);
	mercure.radius=244.53000;
	mercure.dephasage = 0;
	
	struct donneePlanet terre;
	terre.planetName= "Terre";
	terre.demiGrandAxe= 152100000;
	terre.demiPetitAxe= 147095000;
	terre.fullOrbitTime=352;
	terre.masse=5.97*pow(10,24);
	terre.radius=6378.13660;
	terre.dephasage = M_PI;
	
	struct donneePlanet planets[2];
	planets[0]=mercure;
	planets[1]=terre;
	
	struct meteorite meteor;
	meteor.x_cartesien= 0;
	meteor.y_cartesien= 0;
	meteor.initialspeed_x= 0;
	meteor.initialspeed_y= 0;
	meteor.distanceSoleil=sqrt(pow(meteor.x_cartesien,2)+pow(meteor.y_cartesien,2));
	meteor.masse= 8*pow(10,15);
	meteor.radius= 350;
	
	//GlobalPlanetAvancement(time, planets, 2);
	//printf("%s:\nAphelie: %f\nPerihelie: %f\nPar calcul: %f\n(%f,%f)\n\n",planets[0].planetName,planets[0].demiGrandAxe,planets[0].demiPetitAxe,planets[0].distanceSoleil,planets[0].x_cartesien,planets[0].y_cartesien);
	//printf("%s:\nAphelie: %f\nPerihelie: %f\nPar calcul: %f\n(%f,%f)\n\n",planets[1].planetName,planets[1].demiGrandAxe,planets[1].demiPetitAxe,planets[1].distanceSoleil,planets[1].x_cartesien,planets[1].y_cartesien);
	
	/*struct forceCaract forceOnMeteor = AdditionGravitationalForce (gravitationalConstant, planets,2, &meteor);
	printf("X:%f\nY:%f\n%f\n%f\n\n",forceOnMeteor.direction_x,forceOnMeteor.direction_y,forceOnMeteor.intensitee,sqrt(pow(forceOnMeteor.direction_x,2)+pow(forceOnMeteor.direction_y,2)));
	
	applicationForceMeteor(time,&meteor,&forceOnMeteor);
	printf("New: (%f , %f)\nOld: (152100000 , 0)\n\n", meteor.x_cartesien,meteor.y_cartesien);
	printf("New Speed: (%f,%f)\nOld: (0,0)\n\n",meteor.initialspeed_x,meteor.initialspeed_y);*/
	
	meteor.collision= 0;
	printf("first location: (%f , %f)\nfirst speed: (%f , %f)\n\n",meteor.x_cartesien,meteor.y_cartesien,meteor.initialspeed_x,meteor.initialspeed_y);
	repetitionDeFonctions( 8000,time,gravitationalConstant,planets,2,&meteor);
	printf("M:%d avec %s",meteor.collision,meteor.collisionWith);
	
	return 0;
} 
