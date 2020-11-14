#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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


int main(int argc, char **argv)
{
	return 0;
}
