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
			   // -1 si le sommet n'appartient pas a l'arete, 0 s'il y est et est libre, et le numero du joueur l'ayant pris sinon
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
  int nb_a = (pow(3 * taille - 2 * combo + 2, dimension) - pow(taille, dimension)) / 2;
  int nb_c = pow(taille, dimension);

  hgraphe* h = malloc(sizeof( hgraphe ));

  int **mat = malloc(nb_a * sizeof(int*));

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

coup* init_coup(hgraphe* h)
{
	int* coordonnees = malloc(sizeof(int) * h -> dimension);
	coup* c = malloc(sizeof(coup));
	c -> indice = 0;
	c -> joueur = 0;
	c -> coordonnees = coordonnees;
	return c;
}

void indice_vers_coordonnees(int i, int *x, int dimension, int taille)
{
	for (int k = 0; k < dimension; k++)
		x[k] = 0;
	while(i != 0)
	{
		x[k] = i%taille;
		i = i/taille;
	}
  	return;
}

int coordonnees_vers_indice(int *x, int dimension, int taille)
{
	int somme = 0;
  	int puissance = 1;
  	for(int i = 0; i < dimension; i++)
 	{
    		somme += puissance * x[i];
    		puissance *= taille;
  	}
  	return somme;
}

void indice_vers_vecteur(int i, int *x, int dimension)
{
	indice_vers_coordonnees(i, x, dimension, 3)
	for(int k = 0; k < dimension; k++)
		if (x[k] == 2)
			x[k] = -1;
  	return;
}

int *ajoute(int *n, int *m, int dimension)//Ajouter deux nombre représenté par des tableaux (en base la taille, il faut donc faire attention de ne pas sortir du plateau)
{
	for(int i = 0; i < dimension; i++)
		n[i] += m[i];
	return n;
}

int *mult_scal(int *x, int l, int dimension)
{
	for(int i = 0; i < dimension; i++)
		x[i] *= l;
	return x;
}

bool alignement_valide(int *s, int *v, int dimension, int taille, int combo)
{
	for (int i=0; i<dimension; i++)
	{
		if (v[i] == 1)
			if (s[i] + combo > taille)
				return false;
		if (v[i] == -1)
			if (s[i] - combo < -1)
				return false;
	}
	return true;
			
}

void initialise_arretes(hgraphe *h)
{
	//Initialise toutes les directions possibles
	int nb_vecteurs = (pow(3,h -> dimension) - 1)/2; //Nombre de vecteurs directeurs de chaque direction
	int **delta = malloc(nb_vecteurs * sizeof(int *)); //Tableau des directions donc tableau de vecteurs
	for (int i = 0; i < nb_vecteurs; i++)
		delta[i] = malloc((h -> dimension) * sizeof(int));
	for (int i = 0; i < nb_vecteurs; i++)
		for (int j = 0; j < h -> dimension; j++)
			delta[i][j] = 0;
	int p = 1;
	for (int i = 0; i < nb_vecteurs; i++)
	{
		for (int j = p; j<2*p; j++)
			indice_vers_vecteur(j, delta[i], h -> dimension);
		p *= 3;
	}	


	
	int *sommet = malloc((h -> dimension) * sizeof(int));
	for (int i = 0; i < h -> dimension; i++)
		sommet[i] = 0;
	int i_a = 0;
	for (int i_v = 0; i_v < nb_vecteurs; i_v++)
	{	
		for (int i_s; i_s < h -> nb_cases; i_s ++)
		{	
			sommet = indice_vers_coordonnees(i_s, sommet,h -> nb_cases, h -> dimension, h -> taille);
			if (alignement_valide(sommet, delta[i_v]), h->dimension, h->taille, h->combo)
			{
				i_a++
				for (int i = 0; i < h ->combo; i++)
				{	
					sommet = ajoute(sommet, mult_scal(delta[i_v],i,h->dimension), h->dimension);
					mat[i_a][coordonnees_vers_indice(sommet, h->dimension, h->taille)] = 0;
				}
			}
		}
	}
	free(sommet);
}

void liberer_graphe(hgraphe *h)
{
	for(int i = 0; i < h -> nb_alignements; i++)
		free(h -> mat[i]);
	free(h -> mat);
	free(h);
}


void liberer_coup(coup *c)
{
	free(c -> coordonnees);
	free(c);
}


bool sommet_libre(hgraphe *h, coup *c) // verifie que le coup c est bien libre, i.e. jouable, dans h
{
	int i = c -> indice;
	for( int j = 0; j < h -> nb_alignements; j++){
		if( h -> mat[j][i] > 0){
			return false;
		}
	}
	return true;
}


int converti_coup_vers_indice(hgraphe *h, coup *c)
{
	c->indice = coordonnees_vers_indice(c->coordonnees, h->dimension; h->taille);
	return c->indice;
}

void converti_indice_vers_coup(hgraphe *h, coup *c)
{
	indice_vers_coordonnees(c -> indice, c->coordonnees, h->dimension, h->taille);
}

void coup_joueur(hgraphe *h, coup *c)
{
  printf("Quel coup voulez-vous jouer, joueur numero %d ? (<= %d)\n", c -> joueur,h -> taille);
  //int *coup = malloc(h -> dimension*sizeof(int));

  for(int i = 0; i < h -> dimension; i++) {
    printf("- pour la %deme coordonnée : ?\n",i);
    scanf("%d \n", &c -> coordonnees[i]);

    if(c -> coordonnees[i] > h -> taille){
      printf("ce n'est pas un coup valide");
      i--;
    }
  }
  c -> indice = converti_coup_vers_indice(h,c);
  if(!sommet_libre(h,c)){
	printf("ce sommet est déjà occupé par un autre joueur, rejouez");
	coup_joueur(h,c);
  }
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

void afficher_graphe(hgraphe *h) 
{
	for (int i = 0; i < h -> nb_alignements ; i++) 
		for (int j = 0; j < h -> nb_cases; j++) 
			printf(" %d ", j);
		printf("|\n");
	printf("    ");
	for (int j = 0; j < h -> taille; j++) 
		printf("  %d ", j);
	printf("\n\n");
}


bool prend_sommet(hgraphe *h, coup *c){ // le booleen renvoie s'il y a victoire
  int i = c -> indice;
  bool victoire = false;
  for( int j = 0; j < h -> nb_alignements; j++){
    if(h -> mat[j][i] == 0){ //si le sommet est dans l'lignement on indique que le joueur l'a prit
      h -> mat[j][i] = c -> joueur;
      if(!victoire){ //si on se sait toujours pas s'il y a victoire on continue a vérifier
        int k = 0;
        while( k < h -> nb_cases && (h -> mat[j][k] == -1 || h -> mat[j][k] == 0)){
          k++;
        }
        if(k == h -> nb_cases){
          victoire = true; // si toutes les cases sont remplies on a victoire
        }
      }
    }
  }
  h -> progression ++;
  return victoire;
}




void joue_a_2(hgraphe* h, coup* c) {
	if(est_pleine(h)){
		printf("\nMATCH NUL, LA GRILLE EST PLEINE\n");
		return;
	}		
  	coup_joueur(h, c);
	if (prend_sommet(h, c)){ 
		printf("\n victoire du joueur %d \n", c -> joueur);
		return;
	}
	afficher_graphe(h);
	c -> joueur = (c -> joueur + 1) % (h -> nbj);
	joue_a_2(h,c);
}

/*

void print_victoire(int joueur){
  printf("\n");
  printf("     .-. .-')  _  .-')     ('-.          (`-.                \n");
  printf("     \\  ( OO )( \\( -O )   ( OO ).-.    _(OO  )_              \n");
  printf("      ;-----.\\ ,------.   / . --. /,--(_/   ,. \\ .-'),-----. \n");
  printf("      | .-.  | |   /`. '  | \\-.  \\ \\   \\   /(__/( OO'  .-.  ""'\n");
  printf("      | '-' /_)|  /  | |.-'-'  |  | \\   \\ /   / /   |  | |  |\n");
  printf("      | .-. `. |  |_.' | \\| |_.'  |  \\   '   /, \\_) |  |\\|  ""|\n");
  printf("      | |  \\  ||  .  '.'  |  .-.  |   \\     /__)  \\ |  | |  |\n");
  printf("      | '--'  /|  |\\  \\   |  | |  |    \\   /       `'  '-'  '\n");
  printf("      `------' `--' '--'  `--' `--'     `-'          `-----' \n");
  printf(" \n");

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
