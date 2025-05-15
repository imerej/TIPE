#ifndef MORPION_H
#define MORPION_H

typedef struct Hypergraphe hgraphe;

typedef struct Coup coup;

hgraphe* cree_graphe(int dimension, int taille, int combo, int nbj);
coup* init_coup(hgraphe* h);
void indice_vers_coordonnees(int i, int *x, int dimension, int taille);
int coordonnees_vers_indice(int *x, int dimension, int taille);
void indice_vers_vecteur(int i, int *x, int dimension);
int *ajoute(int *n, int *m, int dimension);
int *mult_scal(int *x, int l, int dimension);
bool alignement_valide(int *s, int *v, int dimension, int taille, int combo);
void initialise_arretes(hgraphe *h);
void liberer_graphe(hgraphe *h);
void liberer_coup(coup *c);
bool sommet_libre(hgraphe *h, coup *c);
int converti_coup_vers_indice(hgraphe *h, coup *c);
void converti_indice_vers_coup(hgraphe *h, coup *c);
void coup_joueur(hgraphe *h, coup *c);
bool est_pleine(hgraphe* h);
void afficher_graphe(hgraphe *h);
bool prend_sommet(hgraphe *h, coup *c);
void joue_a_2(hgraphe* h, coup* c);


#endif