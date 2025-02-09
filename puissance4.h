#include <stdbool.h>

#ifndef PUISSANCE4_H
#define PUISSANCE4_H
bool est_pleine(int** grille);
int** cree_grille();
char affiche(int n);
void liberer_grille(int** grille);
void afficher_grille(int** grille);
bool verifier_victoire(int** grille, int joueur);
bool coup_correcte(int** grille,int coup, int* hauteur);
void joue_a_2(int** grille, int joueur, int* hauteur);
int score(int **grille, int joueur, int *hauteur, int coup);
void joue_solo(int** grille, int joueur, int* hauteur);
void print_victoire(int j);
#endif