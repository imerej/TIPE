#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "minmax.h"

#define LINE 6
#define COLS 7
// j'implemente ici un puissance 4 de 7*6 en 2D (avec gravité)
//rajouter une verification de la grille pleine

int **cree_grille() {
  int **grille = malloc(LINE * sizeof(int *));
  assert(grille != NULL);
  for (int i = 0; i < LINE; i++) {
	 grille[i] = malloc(COLS * sizeof(int));
	 assert(grille[i] != NULL);
	 for (int k = 0; k < COLS; k++) {
		grille[i][k] = 0;
	 }
  }
  return grille;
}

bool est_pleine(int** grille){
  for(int i = 0; i < COLS; i++){
	 if(grille[0][i]==0){return false;}
  }
  return true;
}

void liberer_grille(int **grille) {
  for (int i = 0; i < LINE; i++) {
	 free(grille[i]);
  }
  free(grille);
}
char affiche(int n){
	 if(n==0){return ' ';}
	 if(n==1){return 'X';}
	 return 'O';
}
void afficher_grille(int **grille) {
  for (int i = 0; i < LINE; i++) {
	 for (int j = 0; j < COLS; j++) {
		printf("| %c ", affiche(grille[i][j]));
	 }
	 printf("|\n");
  }
  printf("-----------------------------\n");
  for (int j = 0; j < COLS; j++) {
	 printf("  %d ", j);
  }
  printf("\n\n");
}

bool verifier_victoire(
	 int **grille, int joueur) { // on peut ameliorer ca en regardant juste les
										  // nouveautees apportees par le dernier coup
  // Vérification horizontale
  for (int i = 0; i < LINE; i++) {
	 for (int j = 0; j < COLS - 3; j++) {
		if (grille[i][j] == joueur && grille[i][j + 1] == joueur &&
			 grille[i][j + 2] == joueur && grille[i][j + 3] == joueur) {
		  return true;
		}
	 }
  }
  // Vérification verticale
  for (int i = 0; i < LINE - 3; i++) {
	 for (int j = 0; j < COLS; j++) {
		if (grille[i][j] == joueur && grille[i + 1][j] == joueur &&
			 grille[i + 2][j] == joueur && grille[i + 3][j] == joueur) {
		  return true;
		}
	 }
  }
  // Vérification diagonale (bas-gauche à haut-droit)
  for (int i = 3; i < LINE; i++) {
	 for (int j = 0; j < COLS - 3; j++) {
		if (grille[i][j] == joueur && grille[i - 1][j + 1] == joueur &&
			 grille[i - 2][j + 2] == joueur && grille[i - 3][j + 3] == joueur) {
		  return true;
		}
	 }
  }
  // Vérification diagonale (haut-gauche à bas-droit)
  for (int i = 0; i < LINE - 3; i++) {
	 for (int j = 0; j < COLS - 3; j++) {
		if (grille[i][j] == joueur && grille[i + 1][j + 1] == joueur &&
			 grille[i + 2][j + 2] == joueur && grille[i + 3][j + 3] == joueur) {
		  return true;
		}
	 }
  }
  return false;
}

bool coup_correcte(int **grille, int coup) {
  for (int i = 0; i < COLS; i++) {
	 if (coup == i) {
		if (grille[0][coup] == 0) {
		  return true;
		}
	 }
  }
  return false;
}
void print_victoire(int joueur){
  printf("\n");
  printf("     .-. .-')  _  .-')     ('-.          (`-.                \n");
  printf(
		"     \\  ( OO )( \\( -O )   ( OO ).-.    _(OO  )_              \n");
  printf(
		"      ;-----.\\ ,------.   / . --. /,--(_/   ,. \\ .-'),-----. \n");
  printf("      | .-.  | |   /`. '  | \\-.  \\ \\   \\   /(__/( OO'  .-.  "
			"'\n");
  printf(
		"      | '-' /_)|  /  | |.-'-'  |  | \\   \\ /   / /   |  | |  |\n");
  printf("      | .-. `. |  |_.' | \\| |_.'  |  \\   '   /, \\_) |  |\\|  "
			"|\n");
  printf(
		"      | |  \\  ||  .  '.'  |  .-.  |   \\     /__)  \\ |  | |  |\n");
  printf(
		"      | '--'  /|  |\\  \\   |  | |  |    \\   /       `'  '-'  '\n");
  printf("      `------' `--' '--'  `--' `--'     `-'          `-----' \n");
  printf(" \n");
  printf("\n victoire du joueur %d \n", joueur);
  return;
}

void joue_a_2(int **grille, int joueur, int *hauteur) {
  if(est_pleine(grille)){
	 printf("MATCH NUL, LA GRILLE EST PLEINE");
	 return;
  }
  printf(" Joueur %d, dans quelle colonne voulez vous jouer ?", joueur);
  int coup;
  if (scanf("%d", &coup) != 1) {
	 printf("erreur chelou");
	 return;
  }
  if (coup_correcte(grille, coup)) {
	 grille[hauteur[coup]][coup] = joueur;
	 hauteur[coup]--;
	 if (verifier_victoire(grille, joueur)) {
		afficher_grille(grille);
		print_victoire(joueur);
		return;
	 }
	 afficher_grille(grille);
	 if (joueur == 1) {
		joue_a_2(grille, 2, hauteur);
		return;
	 }
	 if (joueur == 2) {
		joue_a_2(grille, 1, hauteur);
		return;
	 }
  }
  printf("ce coup n'est pas possible \n");
  joue_a_2(grille, joueur, hauteur);
}

void joue_solo(int **grille, int joueur, int *hauteur) {
  if(est_pleine(grille)){
	 printf("MATCH NUL, LA GRILLE EST PLEINE");
	 return;
  }
  if (joueur == 1) {
	 printf("SOLO : Dans quelle colonne voulez vous jouer ?");
	 int coup;
	 if (scanf("%d", &coup) != 1) {
		printf("erreur chelou");
		return;
	 }
	 if (coup_correcte(grille, coup)) {
		grille[hauteur[coup]][coup] = joueur;
		hauteur[coup]--;
		if (verifier_victoire(grille, joueur)) {
		  afficher_grille(grille);
		  print_victoire(joueur);
		  return;
		}
		afficher_grille(grille);
		joue_solo(grille, 2, hauteur);
		return;
	 }
	 printf("ce coup n'est pas possible \n");
	 joue_solo(grille, 1, hauteur);
	 return;
  }

  int* resultat = minmax(grille, joueur, hauteur, 3, 1);
  int coup =  resultat[0];
  grille[hauteur[coup]][coup] = joueur;
  hauteur[coup]--;
  printf("%d\n", coup);
  for(int i=0; i<COLS; i++)
	  printf(" %d ", hauteur[i]);
	printf("\n");
  if (verifier_victoire(grille, joueur)) {
	 afficher_grille(grille);
	 printf("\n");
	 printf("\n victoire du joueur %d \n", joueur);
	 return;
  }
  afficher_grille(grille);
  joue_solo(grille, 1, hauteur);
  return;

}

//int score_grille = 0;