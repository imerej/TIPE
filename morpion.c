#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "minmax.h"

// j'implemente ici un puissance 4 de 7*6 en 2D (avec gravité)
//rajouter une verification de la p pleine

/*
typedef struct Plateau {
	int* grille;
	int progression; // nombre de coups joués
	int score; //score de la p
	int dimension; // dimension de la p
	int taille;// taille de la p
	int combo; // nombre de pions à aligner pour gagner
} plateau;
*/

typedef struct Hypergraphe {
	int combo; // nombre de pions à aligner pour gagner
	int progression; // nombre de coups joués
	int score; //score de la p
	int dimension; // dimension de la p
	int taille;// taille de la p
	int nbj; // nombre de joueurs
	int **mat; //la matrice d'adjacence
	int nb_alignements;
	int nb_cases;
} hgraphe;

// la matrice 

typedef struct Coup {
	int indice; // indice du coup joué
	int* coordonnees; // coup joué
	int joueur; // joueur qui joue
} coup;

hgraphe* cree_graphe(int dimension, int taille, int combo, int nbj,)
{
	int nb_a = (pow(3*taille - 2*combo + 2, dimension)-pow(taille,dimension))/2;
	int nb_c = pow(taille,dimension);
	
	h hgraphe* = malloc(sizeof( hgraphe ));
	
	int **mat = malloc(nb_a*sizeof(int*));
	
	for (int i = 0; i < nb_a; i++)
		mat[i] = malloc(nb_c*sizeof(int));
	
	for (int i = 0; i < nb_a; i++)
		for(int j = 0; j < nb_c; j++)
			mat[i][j] = -1;
	
	h -> nb_cases = nb_c;
	h -> nb_alignements = nb_a;
	h -> mat = mat;
	h -> progression = 0;
	h -> score = 0;
	h -> dimension = dimension;
	h -> taille = taille;
	
	return h;
}

void liberer_graphe(hgraphe *h)
{
	for(int i = 0; i < h -> nb_alignements; i++)
		free( h -> mat[i]);
	free(mat);
	free(h);
	return;
}

void liberer_coup(coup *c)
{
	free(c -> coordonnees);
	free(c);
	return;
}


void coup_joueur(hgraphe *h, coup *c)
{
	printf("Quel coup voulez-vous jouer, joueur numero %d?\n", c -> joueur);
	//int *coup = malloc(h -> dimension*sizeof(int));
	
	for(int i = 0; i < h -> dimension; i++) {
		scanf("%deme coordonnée : ?%d (<= %d)\n",i , &c->coordonnees[i], h -> taille);
		if(&c->coordonnees[i] > h -> taille){
			printf("ce n'est pas un coup valide");
			i--;
		}
	}
	return;
}

void converti_coup_vers_indice(hgraphe *h, coup *c)
{
	int somme = 0;
	int puissance = 1;
	for(int i = 0; i < h -> dimension; i++)
	{
		somme += puissance * c -> coordonnees[i];
		puissance *= h -> taille;
	}
	return;
}

void converti_indice_vers_coup(hgraphe *h, coup *c)
{
	for(int k = h -> dimension; k >= 0; k--)
		{
			c -> coordonnees[k] = c -> indice / h -> taille;
			c -> indice = c->indice - h -> taille * c -> coordonnees[k];
		}
	return;
}


bool est_pleine(hgraphe* h)
{
	return h -> progression == pow(h -> taille, h -> dimension);
}


/*
char affiche(int n) {
	 if(n==0){return ' ';}
	 if(n==1){return 'X';}
	 if(n==-1){return '#';}
	 return 'O';
}
*/

void afficher_graphe(hgraphe *h) {
for (int i = 0; i < h -> nb_alignements ; i++) 
	for (int j = 0; j < h -> nb_cases; j++) 
		printf(" %d ", j);
	printf("|\n");
  printf("    ");
	for (int j = 0; j < p->taille; j++) {
	 printf("  %d ", j);
  }
  printf("\n\n");
}


/*
bool verifier_victoire( plateau *p, coup *c) {
  // Vérification horizontale
  for (int k = 0; k < LINE; k++) {
	 for (int l = 0; l < COLS - 3; l++) {
		int i = k+1;
		int j = l+1;
		if (p[i][j] == joueur && p[i][j + 1] == joueur &&
			 p[i][j + 2] == joueur && p[i][j + 3] == joueur) {
		  return true;
		}
	 }
  }
  // Vérification verticale
  for (int k = 0; k < LINE - 3; k++) {
	 for (int l = 0; l < COLS; l++) {
		int i = k+1;
		int j = l+1;
		if (p[i][j] == joueur && p[i + 1][j] == joueur &&
			 p[i + 2][j] == joueur && p[i + 3][j] == joueur) {
		  return true;
		}
	 }
  }
  // Vérification diagonale (bas-gauche à haut-droit)
  for (int k = 3; k < LINE; k++) {
	 for (int l = 0; l < COLS - 3; l++) {
		int i = k+1;
		int j = l+1;
		if (p[i][j] == joueur && p[i - 1][j + 1] == joueur &&
			 p[i - 2][j + 2] == joueur && p[i - 3][j + 3] == joueur) {
		  return true;
		}
	 }
  }
  // Vérification diagonale (haut-gauche à bas-droit)
  for (int k = 0; k < LINE - 3; k++) {
	 for (int l = 0; l < COLS - 3; l++) {
		int i = k+1;
		int j = l+1;
		if (p[i][j] == joueur && p[i + 1][j + 1] == joueur &&
			 p[i + 2][j + 2] == joueur && p[i + 3][j + 3] == joueur) {
		  return true;
		}
	 }
  }
  return false;
}

bool coup_correcte(int **p, int coup, int* hauteur) {
  return p[hauteur[coup]][coup] == 0;
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

void joue_a_2(int **p, int joueur, int *hauteur) {
  if(est_pleine(p)){
	 printf("MATCH NUL, LA GRILLE EST PLEINE");
	 return;
  }
  printf(" Joueur %d, dans quelle colonne voulez vous jouer ?", joueur);
  int coup;
  if (scanf("%d", &coup) != 1) {
	 printf("erreur chelou");
	 return;
  }
  if (coup_correcte(p, coup, hauteur)) {
	 p[hauteur[coup]][coup] = joueur;
	 hauteur[coup]--;
	 if (verifier_victoire(p, joueur)) {
		afficher_p(p);
		print_victoire(joueur);
		return;
	 }
	 afficher_p(p);
	 if (joueur == 1) {
		joue_a_2(p, 2, hauteur);
		return;
	 }
	 if (joueur == 2) {
		joue_a_2(p, 1, hauteur);
		return;
	 }
  }
  printf("ce coup n'est pas possible \n");
  joue_a_2(p, joueur, hauteur);
}

void joue_solo(int **p, int joueur, int *hauteur) {
  if(est_pleine(p)){
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
	 if (coup_correcte(p, coup, hauteur)) {
		p[hauteur[coup]][coup] = joueur;
		hauteur[coup]--;
		if (verifier_victoire(p, joueur)) {
		  afficher_p(p);
		  print_victoire(joueur);
		  return;
		}
		afficher_p(p);
		joue_solo(p, 2, hauteur);
		return;
	 }
	 printf("ce coup n'est pas possible \n");
	 joue_solo(p, 1, hauteur);
	 return;
  }

  int* resultat = minmax(p, joueur, hauteur, 10, 1);
  int coup =  resultat[0];
  p[hauteur[coup]][coup] = joueur;
  hauteur[coup]--;
  printf("%d\n", coup);
  for(int i=0; i<COLS; i++)
	  printf(" %d ", hauteur[i]);
	printf("\n");
  if (verifier_victoire(p, joueur)) {
	 afficher_p(p);
	 printf("\n");
	 printf("\n victoire du joueur %d \n", joueur);
	 return;
  }
  afficher_p(p);
  joue_solo(p, 1, hauteur);
  return;

}
*/
//int score_p = 0;
