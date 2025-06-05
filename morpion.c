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
 
int score(hgraphe *h)
{
	int compteur = 0;
	for (int i=0; i<h->nb_alignements; i++)
	{
		bool align1 = false;
		bool align2 = false;
		bool align1et2 = false;
		for(int j=0; j<h->nb_cases; j++)
		{
			if (h->mat[i][j] == -1 || h->mat[i][j] == 0)
				continue;
			if (h->mat[i][j] == 1 && !align2 && !align1et2)
				align1 = true;
			if (h->mat[i][j] == 2 && !align1 && !align1et2)
				align2 = true;
			if ((h->mat[i][j] == 1 && align2)||(h->mat[i][j] == 2 && align1))
			{
				align1 = false;
				align2 = false;
				align1et2 = true;
			}
		}
		if(align1)
			compteur++;
		if(align2)
			compteur--;
	}
	return compteur;
}

int minmax(hgraphe *h, int profondeur, int joueur)
{
    bool max = (joueur == 1);
    int score_ext = max ? -h->nb_alignements : h->nb_alignements;

    if (profondeur == 0)
        return score(h);

    for (int j = 0; j < h->nb_cases; j++)
    {
        // Vérifier si le coup est valide
        bool valide = false;
        for (int i = 0; i < h->nb_alignements; i++)
        {
            if (h->mat[i][j] == 0)
            {
                valide = true;
                break;
            }
        }
        if (!valide)
            continue;

        // Simuler le coup
        for (int i = 0; i < h->nb_alignements; i++)
            if(h->mat[i][j] != -1)
                h->mat[i][j] = joueur;
        h->progression++;

        if(est_pleine(h))
        {
            for(int i = 0; i < h->nb_alignements; i++)
                if(h->mat[i][j] != -1)
                    h->mat[i][j] = 0;
            h->progression--;
            return 0;
        }

        coup *c = init_coup(h);
        c->indice = j;
        converti_indice_vers_coup(h, c);
        c->joueur = joueur;

        if(max && verifier_victoire(h, c))
        {
            for(int i = 0; i < h->nb_alignements; i++)
                if(h->mat[i][j] != -1)
                    h->mat[i][j] = 0;
            h->progression--;
            return h->nb_alignements;
        }

        if(!max && verifier_victoire(h, c))
        {
            for(int i = 0; i < h->nb_alignements; i++)
                if(h->mat[i][j] != -1)
                    h->mat[i][j] = 0;
            h->progression--;
            return -h->nb_alignements;
        }

        liberer_coup(c);

        int score_tmp = minmax(h, profondeur - 1, joueur % 2 + 1);

        if (max && score_tmp > score_ext)
            score_ext = score_tmp;
        else if (!max && score_tmp < score_ext)
            score_ext = score_tmp;

        // Annuler le coup
        for (int i = 0; i < h->nb_alignements; i++)
            if(h->mat[i][j] != -1)
                h->mat[i][j] = 0;
        h->progression--;
    }
    return score_ext;
}

void joue_ordi(hgraphe* h, coup *c, int profondeur)
{
    if(est_pleine(h))
    {
        printf("\nMATCH NUL, LA GRILLE EST PLEINE \n");
        return;
    }

    if (c->joueur == 1)
    {
        coup_joueur(h, c);
        if (prend_sommet(h, c))
        {
            printf("\n victoire du joueur %d \n", c->joueur);
            print_victoire();
            return;
        }
        c->joueur = 2;
        c->indice = 0;
        for(int i = 0; i < h->dimension; i++)
            c->coordonnees[i] = 0;
        joue_ordi(h, c, profondeur);
    }
    else
    {
        int meilleur_score = h->nb_alignements;
        int meilleur_coup = -1;

        for (int j = 0; j < h->nb_cases; j++)
        {
            // Vérifie si le coup est valide
            bool valide = false;
            for (int i = 0; i < h->nb_alignements; i++)
            {
                if (h->mat[i][j] == 0)
                {
                    valide = true;
                    break;
                }
            }
            if (!valide)
                continue;

            // Simule le coup
            for (int i = 0; i < h->nb_alignements; i++)
                if(h->mat[i][j] != -1)
                    h->mat[i][j] = c->joueur;
            h->progression++;

            int score_tmp = minmax(h, profondeur - 1, 1);  // Le joueur humain joue ensuite
            printf("Score_tmp à la profondeur %d: %d \n", profondeur, score_tmp);

            if (score_tmp < meilleur_score)
            {
                meilleur_score = score_tmp;
                meilleur_coup = j;
            }

            // Annule le coup
            for (int i = 0; i < h->nb_alignements; i++)
                if(h->mat[i][j] != -1)
                    h->mat[i][j] = 0;
            h->progression--;
        }

        // Joue le meilleur coup trouvé
        if (meilleur_coup != -1)
        {
            c->indice = meilleur_coup;
            converti_indice_vers_coup(h, c);
            if (prend_sommet(h, c))
            {
                printf("\nVictoire de l'ordinateur\n");
                return;
            }
        }
        afficher_graphe(h);
        printf("\nL'ordinateur joue dans la case %d.\n", meilleur_coup);
        printf("Le score est: %d \n", meilleur_score);
        c->joueur = 1;
        c->indice = 0;
        for (int i = 0; i < h->dimension; i++)
            c->coordonnees[i] = 0;
        joue_ordi(h, c, profondeur);
    }
}

coup* nieme_libre(hgraphe *h, coup *c, int n)
{
    if (sommet_libre(h, c) && n == 0)
        return c;

    c->indice += 1;
    converti_indice_vers_coup(h, c);

    if (sommet_libre(h, c))
        return nieme_libre(h, c, n - 1);

    return nieme_libre(h, c, n);
}

void joue_alea(hgraphe *h, coup *c)
{
    if (est_pleine(h))
    {
        printf("\nMATCH NUL, LA GRILLE EST PLEINE \n");
        return;
    }

    if (c->joueur == 1)
    {
        coup_joueur(h, c);
        if (prend_sommet(h, c))
        {
            printf("\nVictoire du joueur %d\n", c->joueur);
            print_victoire();
            return;
        }
        c->joueur = 2;
        c->indice = 0;
        for (int i = 0; i < h->dimension; i++)
            c->coordonnees[i] = 0;

        joue_alea(h, c);
    }
    else
    {
        int libre = h->nb_cases - h->progression;
        int n = rand() % libre;
        c->indice = n;
        converti_indice_vers_coup(h, c);
        c = nieme_libre(h, c, n);

        if (prend_sommet(h, c))
        {
            printf("\nVictoire de l'ordinateur\n");
            return;
        }

        c->joueur = 1;
        c->indice = 0;
        for (int i = 0; i < h->dimension; i++)
            c->coordonnees[i] = 0;

        afficher_graphe(h);
        joue_alea(h, c);
    }
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
