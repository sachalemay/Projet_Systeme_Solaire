
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct donneePlanet {
	char * planetName;			
	double aphelion;
	double perihelion; 	
	double demiGrandAxe; 		
	double masse; 					
	double radius;					
	double fullOrbitTime;
	double gravity; 				
	double rotationTime;			
	double distanceSoleil;
	double x_cartesien;
	double y_cartesien;
	double distanceAsteroide;
};

void afficher(struct donneePlanet * planete) {
    printf("planetName : %s aphelion : %f masse: %f\n", planete->planetName, planete->aphelion, planete->masse);
}

int lireLigne(char * ligne, struct donneePlanet * planet) {
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
	//printf("%s \n", ligne);
	int len = virgule1 - ligne;
	char name [30000];
	strncpy(name, ligne, len);
	planet->planetName = name;
	planet->aphelion = atof(virgule1 + 1);
	planet->perihelion = atof(virgule2 + 1);
	planet->demiGrandAxe = atof(virgule3 + 1);
	planet->masse = atof(virgule4 + 1);
	planet->radius = atof(virgule5 + 1);
	planet->fullOrbitTime = atof(virgule6 + 1);
	planet->gravity = atof(virgule7 + 1);
	printf("%s \n", name);
	
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

    // Fermer le fichier et renvoyer le nombre de lignes lues
    fclose(file);
    return ligne;
}

int main(int argc, char **argv)
{
	struct donneePlanet planete[14];
	int x = lireFichier("bodies-2.csv", planete, 14);
	printf("%d", x);
	for (int i = 0; i<14; i++) {
		afficher(&planete[i]);
	}

	//lireLigne("Tamerelefacochere12,445410000,382620000,413690250,9.393000000000001e+20,487.0,1681.63,0.28", &planet1);
	//afficher(&planet1);
}

