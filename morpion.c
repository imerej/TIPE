#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "minmax.h"

// j'implemente ici un puissance 4 de 7*6 en 2D (avec gravité)//plus maintenant
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
	int **mat; //la matrice d'adjacence avec les lignes pour les arrêtes et les colonnes pour les sommets
	int nb_alignements;
	int nb_cases;
} hgraphe;

// la matrice 

typedef struct Coup {
	int indice; // indice du coup joué
	int* coordonnees; // coup joué
	int joueur; // joueur qui joue
} coup;

hgraphe* cree_graphe(int dimension, int taille, int combo, int nbj)
{
	int nb_a = (pow(3*taille - 2*combo + 2, dimension)-pow(taille,dimension))/2;//nombre d'arrêtes de l'hypergraphe
	int nb_c = pow(taille,dimension);//nombre de côtés
	
	hgraphe* h = malloc(sizeof( hgraphe ));
	
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

int *incremente(int *m, int i) // on suppose m < 2^d // i indice de retenue //La fonction permet d'ajouter 1 à un nombre représenté en binaire par un tableau
{
	if(m[i] == 0)
	{
		m[i] = 1;
		return m;
	}
	return ajoute(m, i+1);
}//Complexité constante en moyenne

int *ajoute(int *n, int *m, int dimension)//Ajouter deux nombre représenté par des tableaux (en base la dimension, il faut donc faire attention de ne pas sortir du plateau)
{
	for(int i = 0; i < dimension; i++)
		n[i] += m[i];
	return n;
}

void initialise_arretes(hgraphe *h)
{
	int nb_vecteurs = (1 << (h -> dimension)) - 1; //Nombre de vecteurs directeurs de chaque direction
	int **delta = malloc(nb_vecteurs * sizeof(int *)); //Tableau des directions donc tableau de vecteurs
	for (int i = 0; i < nb_vecteurs; i++)
		delta[i] = malloc((h -> dimension) * sizeof(int));
	delta[0][0] = 1;
	for (int i = 1; i < h -> dimension; i++)
		delta[0][i] = 0;
	for (int i = 1; i < nb_vecteurs; i++)
		delta[i] = incremente[delta[i-1], 0);//Nouvelle direction (c'est un de plus si on considère le vecteur comme un nombre en base deux)
	int *sommet = malloc((h -> dimension) * sizeof(int));
	for (int i = 0; i< h -> dimension; i++)
		sommet[i] = 0;
	for (int i = 0; i < h -> nb_cases; i++)
	{
		sommet = converti_indice_vers_coordonnees(i, sommet,h -> nb_cases, h -> taille, h -> dimension);
		for(int j = 0; j < nb_vecteurs; j++)
		{
			
			while()
			{
			
			}
		}
	}	
}

void liberer_graphe(hgraphe *h)
{
	for(int i = 0; i < h -> nb_alignements; i++)
		free(h -> mat[i]);
	free(h -> mat);
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

int converti_coordonnees_vers_indice(int *c, int taille, int dimension)
{
	int somme = 0;
	int puissance = 1;
	for(int i = 0; i < dimension; i++)
	{
		somme += puissance * c[i];
		puissance *= taille;
	}
	return somme;
}

int *converti_indice_vers_coordonnees(int i, int *c, int nb_cases, int taille, int dimension)
{
	for(int k = dimension - 1; k >= 0; k--)
		{
			c[k] = i / (nb_cases / taille);
			i = i - (nb_cases / taille) * c[k];
			nb_cases = nb_cases / taille;
		}
	return c;
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
	for (int j = 0; j < h -> taille; j++) {
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
} */

bool verifier_victoire(hgraphe *h, coup *c)//A refaire
{
	bool victoire = true;
	for(int i = 0; i < h->nb_alignements; i++)
		if(h->mat[i][coup -> indice] != coup -> joueur)
			victoire = false;
	return victoire;
}
/*
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
