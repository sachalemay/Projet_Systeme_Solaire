#include <stdio.h>
#include <math.h>

struct donneePlanet {
	char * planetName;
	double demiGrandAxe; 		//ou Aphelie
	double demiPetitAxe; 		//ou Perihelie
	double eccentricity; 		//pour calcul de l'elipse
	double masse; 					//pour force de gravitation
	double radius;					//pour etablir la collision
	double fullOrbitTime;		//pour vitesse planete
	double gravity; 				//si on est chaud
	double rotationTime;			//si on est chaud
	double distanceSoleil;
	double x_cartesien;
	double y_cartesien;
};

void avancementParametrisation ( int time, struct donneePlanet * planet){
	planet->distanceSoleil = planet->demiPetitAxe * planet->demiGrandAxe / sqrt( pow(planet->demiPetitAxe*cos(time*2*M_PI/planet->fullOrbitTime),2) + pow(planet->demiGrandAxe*sin(time*2*M_PI/planet->fullOrbitTime),2));
	planet->x_cartesien=planet->demiGrandAxe*cos(time*2*M_PI/planet->fullOrbitTime);
	planet->y_cartesien=planet->demiPetitAxe*sin(time*2*M_PI/planet->fullOrbitTime);
}



//Il va peut-être falloir s'interesser a Kepler pour la vitesse de rotation autour du soleil

int main(int argc, char * argv[]) {
	
	int time= 481;
	
	struct donneePlanet mercure;
	mercure.planetName= "Mercure";
	mercure.demiGrandAxe= 69816900;
	mercure.demiPetitAxe= 46001200;
	mercure.fullOrbitTime=88;
	
	struct donneePlanet terre;
	terre.planetName= "Terre";
	terre.demiGrandAxe= 152100000;
	terre.demiPetitAxe= 147095000;
	terre.fullOrbitTime=352;
	
	avancementParametrisation(time,&mercure);
	avancementParametrisation(time,&terre);
	
	printf("%s:\nAphelie: %f\nPerihelie: %f\nPar calcul: %f\n(%f,%f)\n\n",mercure.planetName,mercure.demiGrandAxe,mercure.demiPetitAxe,mercure.distanceSoleil,mercure.x_cartesien,mercure.y_cartesien);
	printf("%s:\nAphelie: %f\nPerihelie: %f\nPar calcul: %f\n(%f,%f)\n\n",terre.planetName,terre.demiGrandAxe,terre.demiPetitAxe,terre.distanceSoleil,terre.x_cartesien,terre.y_cartesien);
	
	return 0;
} 
