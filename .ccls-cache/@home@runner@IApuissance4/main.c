#include "puissance4.h"
#include "minmax.h"
#include "reseau_neurones.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define COLS 7
#define LINE 6


int main() {
  // pour le puissance 4
  int *hauteur = malloc(sizeof(int) * COLS);
  for (int i = 0; i < COLS; i++) {
    hauteur[i] = LINE - 1;
  }
  int **grille = cree_grille();
  joue_solo(grille, 2, hauteur);

  liberer_grille(grille);
  free(hauteur);

  // pour le morpion
  return 0;
}

