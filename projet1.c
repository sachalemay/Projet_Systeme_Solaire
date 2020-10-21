#include <stdio.h>
#include <math.h>

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
	double y_cartesien;
};

struct meteorite {
	double initialspeed_x;
	double initialspeed_y;
	double distanceSoleil;
	double distancePlanet;
	double masse;
	double radius;
	double x_cartesien;
	double y_cartesien;
};

struct forceCaract{
	double intensitee;
	double angle;
	double direction_parrallele_sun;
	double direction_perpendiculaire_sun;
	double direction_x;
	double direction_y;
};

void avancementParametrisation ( int time, struct donneePlanet * planet){
	planet->distanceSoleil = planet->demiPetitAxe * planet->demiGrandAxe / sqrt( pow(planet->demiPetitAxe*cos(time*2*M_PI/planet->fullOrbitTime),2) + pow(planet->demiGrandAxe*sin(time*2*M_PI/planet->fullOrbitTime),2));
	planet->x_cartesien=planet->demiGrandAxe*cos(time*2*M_PI/planet->fullOrbitTime);
	planet->y_cartesien=planet->demiPetitAxe*sin(time*2*M_PI/planet->fullOrbitTime);
}

void gravitationalForce (double gravitationalConstant, struct donneePlanet * planet, struct meteorite * meteor, struct forceCaract * force){
	meteor->distancePlanet = sqrt(pow(planet->x_cartesien - meteor->x_cartesien,2)+pow(planet->y_cartesien - meteor->y_cartesien,2));
	double intensitee = gravitationalConstant * planet->masse * meteor->masse / pow(meteor->distancePlanet,2);
	double angle = acos( (pow(meteor->distancePlanet,2) + pow (meteor->distanceSoleil,2) - pow(planet->distanceSoleil,2)) / (2* meteor->distancePlanet * meteor->distanceSoleil));
	force->direction_parrallele_sun += intensitee*cos(angle);
	force->direction_perpendiculaire_sun += intensitee*sin(angle);
	//l'angle est toujours positif il faut qu'il puisse être negatif (regarder mon dessin)
}

int main(int argc, char * argv[]) {
	
	int time= 22;
	double gravitationalConstant = 6.6743*pow(10,-17);
	
	struct donneePlanet mercure;
	mercure.planetName= "Mercure";
	mercure.demiGrandAxe= 69816900;
	mercure.demiPetitAxe= 46001200;
	mercure.fullOrbitTime=88;
	mercure.masse=3.3*pow(10,23);
	
	/*struct donneePlanet terre;
	terre.planetName= "Terre";
	terre.demiGrandAxe= 152100000;
	terre.demiPetitAxe= 147095000;
	terre.fullOrbitTime=352;*/
	
	struct meteorite meteor;
	meteor.x_cartesien= 15000;
	meteor.y_cartesien= 47000000;
	meteor.distanceSoleil=47000002;
	meteor.masse= 8*pow(10,10);
	
	struct forceCaract force1;
	force1.direction_parrallele_sun=0;
	force1.direction_perpendiculaire_sun=0;
	
	//struct donneePlanet soleil;
	//soleil.masse=1.989*pow(10,30);
	//soleil.distance=0;
	
	avancementParametrisation(time,&mercure);
	//avancementParametrisation(time,&terre);
	
	printf("%s:\nAphelie: %f\nPerihelie: %f\nPar calcul: %f\n(%f,%f)\n\n",mercure.planetName,mercure.demiGrandAxe,mercure.demiPetitAxe,mercure.distanceSoleil,mercure.x_cartesien,mercure.y_cartesien);
	//printf("%s:\nAphelie: %f\nPerihelie: %f\nPar calcul: %f\n(%f,%f)\n\n",terre.planetName,terre.demiGrandAxe,terre.demiPetitAxe,terre.distanceSoleil,terre.x_cartesien,terre.y_cartesien);
	
	gravitationalForce (gravitationalConstant, &mercure, &meteor, &force1);
	printf("Soleil:%f\nPerpen:%f",force1.direction_parrallele_sun,force1.direction_perpendiculaire_sun);
	
	return 0;
} 
