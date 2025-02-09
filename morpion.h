#ifndef MORPION_H
#define MORPION_H

typedef struct Plateau {
	int* grille;
	int progression; // nombre de coups joués
	int score; //score de la p
	int dimension; // dimension de la p
	int taille; // taille de la p
} plateau;

typedef struct Coup {
	int indice; // indice du coup joué
	int* coordonnees; // coup joué
	int joueur; // joueur qui joue
} coup;

plateau* cree_plateau(int dimension, int taille);
void liberer_plateau(plateau *p);
void liberer_coup(coup *c);
void coup_joueur(plateau *p, coup *c);
void converti_coup_vers_indice(plateau *p, coup *c);
void converti_indice_vers_coup(plateau *p, coup *c);
bool est_pleine(plateau p);

void liberer_p(int *p);

#endif