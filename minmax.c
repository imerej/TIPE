#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "puissance4.h"
#include "minmax.h"
#include <limits.h>

int score_grille = 0;

#define LINE 6
#define COLS 7


void affiche_hauteur(int *hauteur){
	for(int i = 0; i < COLS; i++){
		printf(" %d ", hauteur[i]);
	}
	printf("\n");
	return;
}
int score(int **grille, int joueur, int *hauteur, int coup)
/*
	calcul d'un score:
   compte le nombre de façons potentielles de gagner en
   jouant un certain coup.
*/
{
  if (!coup_correcte(grille, coup, hauteur)){
	  printf("score incorrect");
   return (-1);
  }
  int score = 0;
  int ligne = hauteur[coup];
  int colonne = coup;
  int delta[4][2] = {{0, 1}, {1, 1}, {1, 0}, {-1, 1}};
  for (int j = 0; j < 4; j++) 
  {  
	 int case_de_suite = 1;
	 int i=1;

	 while (
		 i<4 
		 && ligne - (i * delta[j][0]) < LINE
		 && colonne - (i * delta[j][1]) < COLS
		 && ligne - (i * delta[j][0]) >= 0
		 && colonne - (i * delta[j][1]) >= 0
		 && grille[ligne - (i * delta[j][0])][colonne - (i * delta[j][1])] != (joueur % 2) + 1
	 ) {
		case_de_suite++;
		i++;
	 }
	 i=1;
	 while (
		 i<4 
		  && ligne + (i * delta[j][0]) < LINE
		  && colonne + (i * delta[j][1]) < COLS
		  && ligne + (i * delta[j][0]) >= 0
		  && colonne + (i * delta[j][1]) >= 0
		 && grille[ligne + (i * delta[j][0])][colonne + (i * delta[j][1])] != (joueur % 2) + 1
	) {
		case_de_suite++;
		i++;
	 }
	//printf("sens %d, case de suite %d\n", j, case_de_suite); // 0 ligne, 1 diagonale descendante, 2 colonne, 3 diagonale ascendatne
	 if (case_de_suite > 3)
		score = score + case_de_suite - 3;
  }
  return score;
}

int* minmax(int **grille, int joueur, int *hauteur, int profondeur, bool maximisation)
//maximisation est vrai si on cherche a maximiser le score, faux sinon
{	
	int* resultat = malloc(sizeof(int) * 2);
	resultat[0] = -1; //contient le coup a jouer 
	resultat[1] = 0; //contient la variation de score; 
	int variation_score = 0;// correspond a une quantifcation d'a quel point un coup est favorable
	//on rentrera celle ci dans resultat[1] a la fin de la fonction
	if (
		profondeur == 0 
		|| est_pleine(grille) 
		|| verifier_victoire(grille, 1) 
		|| verifier_victoire(grille, 2)
		) 
	{
		return resultat;
	}
	
	int meilleur_score = -1;
	int meilleur_coup = -1;
	for (int i = 0; i < COLS; i++)
	{
		int sc = score(grille, joueur, hauteur, i);
		printf("score pour la case %d : %d\n",i, sc);
		if (coup_correcte(grille, i, hauteur) && sc > meilleur_score)
		{
			meilleur_score = sc;
			meilleur_coup = i;
		}
	}
	if (meilleur_coup == -1)
		return resultat;
	variation_score += meilleur_score;
	grille[hauteur[meilleur_coup]][meilleur_coup] = joueur;
	
	hauteur[meilleur_coup]--; // hauteur du prochain coup dispo,    == -1 si la colonne est pleine
	affiche_hauteur(hauteur);
	score_grille += (2*maximisation - 1)*variation_score;
	printf("meilleur coup :%d\nvariation score %d\njoueur %d\n score_grille : %d\n", meilleur_coup, variation_score, joueur, score_grille);
	minmax(grille, joueur%2 + 1, hauteur, profondeur - 1, !maximisation);
	if(hauteur[meilleur_coup] < 0 || hauteur[meilleur_coup] > LINE-1 ||  meilleur_coup < 0 || meilleur_coup > COLS-1) { 
		printf("erreur, on essaie de vider la case %d %d de la grille", hauteur[meilleur_coup], meilleur_coup); 
		return resultat; 
	}
	else {
		hauteur[meilleur_coup]--;
		grille[hauteur[meilleur_coup]][meilleur_coup] = 0;//on  enleve le changement quon avait effectue
	}
	
	
	resultat[0] = meilleur_coup;
	resultat[1] =  variation_score;
	return resultat;
}








int alpha_beta(int **grille, int joueur, int *hauteur, int profondeur) {
	//un noeud de larbre etant une grille, une feuille correspond a une fin de partie
	//si nœud est une feuille alors retourner la valeur de nœud
	if (est_pleine(grille) || verifier_victoire(grille, 1) || verifier_victoire(grille, 2)) return 0;
		return 0;
}
		/*
fonction alphabeta(nœud, α, β)  α est toujours inférieur à β 

	
sinon 
			si nœud est de type Min alors
						  v = +∞
						  pour tout fils de nœud faire
								v = min(v, alphabeta(fils, α, β))                
								si α ≥ v alors   coupure alpha 
								  retourner v
								β = min(β, v)           
			 sinon
						  v = -∞
						  pour tout fils de nœud faire
								v = max(v, alphabeta(fils, α, β))                
								si v ≥ β alors coupure beta 
									 retourner v
								α = max(α, v)
 retourner v
*/