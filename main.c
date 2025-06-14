//#include "puissance4.h"
//#include "minmax.h"
//#include "reseau_neurones.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "morpion.h"
//#define COLS 7
//#define LINE 6


int main() {
  // pour le puissance 4
  /*
  int *hauteur = malloc(sizeof(int) * COLS+2);
  for (int i = 1; i <= COLS; i++) {
    hauteur[i] = LINE;
  }
  hauteur[0] = -1;
  hauteur[COLS + 1] = -1;
  int **grille = cree_grille();
  joue_solo(grille, 2, hauteur);

  liberer_grille(grille);
  free(hauteur);
  */

  // pour le morpion
  hgraphe* h = cree_graphe(2,3,3,2);
  coup* c = init_coup(h);
  //joue_a_2(h,c);
  int profondeur = 3;
  //joue_ordi(h,c,profondeur);
  joue_alea(h,c);

  return 0;
}

