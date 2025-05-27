#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
//#include "minmax.h"
#include "morpion.h"

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

int puiss(int a, int b)
{
	if (b==0)
		return 1;
	if (b==1)
		return a;
	if(b%2 == 0)
	{
		int c = puiss(a,b/2);
		return c*c;
	}
	int c = puiss(a,(b-1)/2);
	return a*c*c;
}

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
  int nb_a = (puiss(3 * taille - 2 * combo + 2, dimension) - puiss(taille, dimension)) / 2;
  int nb_c = puiss(taille, dimension);

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
  h -> combo = combo;
  h -> nbj = nbj;

  return h;
}

coup* init_coup(hgraphe* h)
{
	int* coordonnees = malloc(sizeof(int) * h -> dimension);
	coup* c = malloc(sizeof(coup));
	c -> indice = 0;
	c -> joueur = 1;
	c -> coordonnees = coordonnees;
	return c;
}

void indice_vers_coordonnees(int i, int *x, int dimension, int taille)
{
	for (int k = 0; k < dimension; k++)
		x[k] = 0;
	int k=0;
	while(i != 0)
	{
		x[k] = i%taille;
		i = i/taille;
		k++;
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
	indice_vers_coordonnees(i, x, dimension, 3);
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
	int nb_vecteurs = (puiss(3,h -> dimension) - 1)/2; //Nombre de vecteurs directeurs de chaque direction
	int **delta = malloc(nb_vecteurs * sizeof(int *)); //Tableau des directions donc tableau de vecteurs
	for (int i = 0; i < nb_vecteurs; i++)
		delta[i] = malloc((h -> dimension) * sizeof(int));
	for (int i = 0; i < nb_vecteurs; i++)
		for (int j = 0; j < h -> dimension; j++)
			delta[i][j] = 0;
	int p = 1;
	int i_v = 0;
	while (i_v < nb_vecteurs)
	{
		for (int j = p; j<2*p; j++)
		{
			indice_vers_vecteur(j, delta[i], h -> dimension);
			i_v++;
		}
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
			indice_vers_coordonnees(i_s, sommet, h -> dimension, h -> taille);
			if (alignement_valide(sommet, delta[i_v], h->dimension, h->taille, h->combo))
			{
				for (int i = 0; i < h ->combo; i++)
				{	
					h->mat[i_a][coordonnees_vers_indice(sommet, h->dimension, h->taille)] = 0;
					sommet = ajoute(sommet, delta[i_v], h->dimension);
				}
				i_a++;
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
		if (h -> mat[j][i] == 0)
			return true;
	}
	return true;
}


int converti_coup_vers_indice(hgraphe *h, coup *c)
{
	c->indice = coordonnees_vers_indice(c->coordonnees, h->dimension, h->taille);
	return c->indice;
}

void converti_indice_vers_coup(hgraphe *h, coup *c)
{
	indice_vers_coordonnees(c -> indice, c->coordonnees, h->dimension, h->taille);
}

void coup_joueur(hgraphe *h, coup *c)
{
  printf("Quel coup voulez-vous jouer, joueur numero %d ? (< %d)\n", c -> joueur,h -> taille);
  //int *coup = malloc(h -> dimension*sizeof(int));

  for(int i = 0; i < h -> dimension; i++) {
    printf("- pour la %deme coordonnée : ?\n",i);
    scanf("%d", &c -> coordonnees[i]);

    if(c -> coordonnees[i] >= h -> taille){
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
	return h -> progression == puiss(h -> taille, h -> dimension);
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
        while( k < h -> nb_cases && (h -> mat[j][k] == -1 || h -> mat[j][k] == c->joueur)){
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
		print_victoire();
		return;
	}
	afficher_graphe(h);
	c -> joueur = (c -> joueur) % (h -> nbj) + 1;
	joue_a_2(h,c);
}



void print_victoire(){
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

bool verifier_victoire_alignement(hgraphe *h, int i, int joueur)
{
	for (int j=0; j<h->nb_cases; j++)
		if((h->mat[i][j] != -1) && (h->mat[i][j] != joueur))
             		return false
	return true;
}

bool verifier_victoire(hgraphe *h, coup *c)
{
    int n = c->joueur;
    int j = c->indice;
    for(int i=0; i<h->nb_alignements; i++)
        if((h->mat[i][j]!=-1) && verifier_victoire_alignement(h,i,n))
            return true;
    return false;
}
 
bool alignement_potentiel(int i, hgraphe *h, int joueur)
{
    for(int j=0; j<h->nb_cases; j++)
        if((h->mat[i][j] != -1) && (h->mat[i][j] != 0) && (h->mat[i][j] != joueur))
            return false;
    return true;
}
 
int score(hgraphe *h, coup *c)
{
    int compteur = 0;
    int j = c->indice;
    for (int i=0; i<h->nb_alignements; i++)
    {
        if (h->mat[i][j] > 0)
            return -1;
        if (h->mat[i][j] == 0)
            if (alignement_potentiel(i,h,coup->joueur))
                compteur++;
    }
    return compteur;
}

int minimax(hgraphe *h, coup *coup, int profondeur, bool maxjoueur, int joueur_actuel) {
    if (verifier_victoire(h, coup)) {
        // Victoire si joueur actuel vient de jouer
        return maxjoueur ? -1000 : 1000;
    }
    if (est_pleine(h) || profondeur == 0) {
        return score(h, coup);  // Heuristique
    }

    int meilleur_score = maxjoueur ? -10000 : 10000;
    coup c_temp;
    *(&c_temp) = *coup;  // Copie superficielle, attention aux pointeurs

    for (int i = 0; i < h->nb_cases; i++) {
        // Préparer un coup possible sur la case i (convertir indice en coord)
        converti_indice_vers_coup(h, &c_temp);
        c_temp.indice = i;
        indice_vers_coordonnees(i, c_temp.coordonnees, h->dimension, h->taille);
        c_temp.joueur = joueur_actuel;

        if (!sommet_libre(h, &c_temp)) {
            continue; // case déjà prise
        }

        // Simuler le coup
        prend_sommet(h, &c_temp);
        h->progression++;

        int score_courant = minimax(h, &c_temp, profondeur - 1, !maxjoueur,
                                   (joueur_actuel % h->nbj) + 1);

        // Annuler le coup simulé
        // Il faut écrire une fonction annuler_coup() pour restaurer la matrice !
        annuler_coup(h, &c_temp);
        h->progression--;

        if (maxjoueur) {
            if (score_courant > meilleur_score)
                meilleur_score = score_courant;
        } else {
            if (score_courant < meilleur_score)
                meilleur_score = score_courant;
        }
    }

    return meilleur_score;
}

coup* choisir_coup_ordinateur(hgraphe *h, int profondeur, int joueur) {
    coup* meilleur_coup = init_coup(h);
    meilleur_coup->joueur = joueur;

    int meilleur_score = -10000;
    coup c_test;
    *(&c_test) = *meilleur_coup;

    for (int i = 0; i < h->nb_cases; i++) {
        indice_vers_coordonnees(i, c_test.coordonnees, h->dimension, h->taille);
        c_test.indice = i;
        c_test.joueur = joueur;

        if (!sommet_libre(h, &c_test)) continue;

        prend_sommet(h, &c_test);
        h->progression++;

        int score_courant = minimax(h, &c_test, profondeur - 1, false,
                                   (joueur % h->nbj) + 1);

        annuler_coup(h, &c_test);
        h->progression--;

        if (score_courant > meilleur_score) {
            meilleur_score = score_courant;
            *meilleur_coup = c_test;
            // Attention : gérer copie profonde des coordonnees si besoin
        }
    }
    return meilleur_coup;
}

void annuler_coup(hgraphe *h, coup *c) {
    int i = c->indice;
    int joueur = c->joueur;

    for (int j = 0; j < h->nb_alignements; j++) {
        // Si la case appartient à cet alignement
        if (h->mat[j][i] == joueur) {
            // On remet la case libre (0)
            h->mat[j][i] = 0;
        }
    }
}

void joue_avec_ordinateur(hgraphe *h, int profondeur_max) {
    coup *c = init_coup(h);

    bool victoire = false;
    while (!victoire && !est_pleine(h)) {
        afficher_graphe(h);

        if (c->joueur == 1) {
            // Tour du joueur humain (joueur 1)
            printf("Tour du joueur humain (joueur 1)\n");
            coup_joueur(h, c);
        } else {
            // Tour de l'ordinateur (joueur 2)
            printf("Tour de l'ordinateur (joueur 2)\n");
            minimax(h, profondeur_max, c->joueur, c);
            printf("L'ordinateur joue le coup à l'indice %d\n", c->indice);
            converti_indice_vers_coup(h, c);  // si besoin d'avoir coordonnees lisibles
        }

        // Appliquer le coup
        if (!sommet_libre(h, c)) {
            printf("Coup non valide, rejouez.\n");
            continue; // redemande coup si invalidité
        }

        // Appliquer le coup sur le graphe
        prend_sommet(h, c);
        h->progression++;

        // Vérifier victoire
        victoire = verifie_victoire(h, c);
        if (victoire) {
            afficher_graphe(h);
            if (c->joueur == 1)
                printf("Bravo, joueur humain a gagné !\n");
            else
                printf("L'ordinateur a gagné !\n");
            break;
        }

        if (est_pleine(h)) {
            afficher_graphe(h);
            printf("Match nul, plateau plein.\n");
            break;
        }

        // Changer de joueur
        c->joueur = (c->joueur == 1) ? 2 : 1;
    }

    liberer_coup(c);
}

/*
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
