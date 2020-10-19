#include <stdio.h>

struct donneePlanet {
	double planetName;
	double demiGrandAxe; 		//ou Aphelie
	double demiPetitAxe; 		//ou Perihelie
	double eccentricity; 		//pour calcul de l'elipse
	double masse; 				//pour force de gravitation
	double radius;				//pour etablir la collision
	double fullOrbitTime;		//pour vitesse planete
	double gravity 				//si on est chaud
	double rotationTime			//si on est chaud
};

//Il va peut-être falloir s'interesser a Kepler pour la vitesse de rotation autour du soleil

int main(int argc, char * argv[]) {
	
} 
