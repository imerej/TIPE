#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define INPUT_SIZE 9  
// 9 cases du jeu de Morpion
#define HIDDEN_SIZE 9
// Nombre de neurones dans la couche cachée
#define OUTPUT_SIZE 9 
// Sortie avec 9 valeurs pour chaque case

#define LEARNING_RATE 0.1
#define EPOCHS 10000
#define BATCH_SIZE 3
// Structure pour représenter un neurone
// Fonction sigmoïde
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

// Dérivée de la fonction sigmoïde
double sigmoid_derivative(double x) {
    return x * (1.0 - x); // bizarrre (1/(exp(x)+1)(1+exp(-x))) 
}

// Fonction d'activation ReLU
double relu(double x) {
    return (x > 0) ? x : 0;
}

// Dérivée de la ReLU
double relu_derivative(double x) {
    return (x > 0) ? 1 : 0;
}
// Structure représentant un réseau de neurones
typedef struct {
    double weights_input_hidden[INPUT_SIZE][HIDDEN_SIZE];
    double weights_hidden_output[HIDDEN_SIZE][OUTPUT_SIZE];
    double bias_hidden[HIDDEN_SIZE];
    double bias_output[OUTPUT_SIZE];
    double hidden_layer[HIDDEN_SIZE];
    double output_layer[OUTPUT_SIZE];
} NeuralNetwork;

void initialize_network(NeuralNetwork *nn) {
    // Initialisation des poids et des biais aléatoires
    srand(time(NULL));

    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            nn->weights_input_hidden[i][j] = (rand() % 1000 - 500) / 1000.0;  // Poids entre -0.5 et 0.5
        }
    }

    for (int j = 0; j < HIDDEN_SIZE; j++) {
        nn->bias_hidden[j] = (rand() % 1000 - 500) / 1000.0;  // Biais entre -0.5 et 0.5
    }

    for (int j = 0; j < HIDDEN_SIZE; j++) {
        for (int k = 0; k < OUTPUT_SIZE; k++) {
            nn->weights_hidden_output[j][k] = (rand() % 1000 - 500) / 1000.0;  // Poids entre -0.5 et 0.5
        }
    }

    for (int k = 0; k < OUTPUT_SIZE; k++) {
        nn->bias_output[k] = (rand() % 1000 - 500) / 1000.0;  // Biais entre -0.5 et 0.5
    }
}

void forward_propagate(NeuralNetwork *nn, double *input) {
    // Calcul des activations de la couche cachée
    for (int j = 0; j < HIDDEN_SIZE; j++) {
        nn->hidden_layer[j] = 0.0;
        for (int i = 0; i < INPUT_SIZE; i++) {
            nn->hidden_layer[j] += input[i] * nn->weights_input_hidden[i][j];
        }
        nn->hidden_layer[j] += nn->bias_hidden[j];
        nn->hidden_layer[j] = relu(nn->hidden_layer[j]);  // Activation ReLU
    }

    // Calcul des activations de la couche de sortie
    for (int k = 0; k < OUTPUT_SIZE; k++) {
        nn->output_layer[k] = 0.0;
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            nn->output_layer[k] += nn->hidden_layer[j] * nn->weights_hidden_output[j][k];
        }
        nn->output_layer[k] += nn->bias_output[k];
        nn->output_layer[k] = sigmoid(nn->output_layer[k]);  // Activation sigmoïde
    }
}

void backward_propagate(NeuralNetwork *nn, double *input, double *target) {
    double output_error[OUTPUT_SIZE];
    double hidden_error[HIDDEN_SIZE];

    // Calcul de l'erreur de la sortie
    for (int k = 0; k < OUTPUT_SIZE; k++) {
        output_error[k] = target[k] - nn->output_layer[k] ;
    }

    // Calcul des erreurs pour la couche cachée
    for (int j = 0; j < HIDDEN_SIZE; j++) {
        hidden_error[j] = 0.0;
        for (int k = 0; k < OUTPUT_SIZE; k++) {
            hidden_error[j] += output_error[k] * nn->weights_hidden_output[j][k];
        }
    }

    // Mise à jour des poids et biais de la couche de sortie
    for (int k = 0; k < OUTPUT_SIZE; k++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            nn->weights_hidden_output[j][k] += LEARNING_RATE * output_error[k] * nn->hidden_layer[j];
        }
        nn->bias_output[k] += LEARNING_RATE * output_error[k];
    }

    // Mise à jour des poids et biais de la couche cachée
    for (int j = 0; j < HIDDEN_SIZE; j++) {
        for (int i = 0; i < INPUT_SIZE; i++) {
            nn->weights_input_hidden[i][j] += LEARNING_RATE * hidden_error[j] * input[i];
        }
        nn->bias_hidden[j] += LEARNING_RATE * hidden_error[j];
    }
}

void train(NeuralNetwork *nn, double input_data[BATCH_SIZE][INPUT_SIZE], double target_data[BATCH_SIZE][OUTPUT_SIZE]) {
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        for (int i = 0; i < BATCH_SIZE; i++) {
            forward_propagate(nn, input_data[i]);
            backward_propagate(nn, input_data[i], target_data[i]);
        }
    }
}

// Fonction pour afficher la grille du jeu
void print_board(int board[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 0) printf(". ");
            else if (board[i][j] == 1) printf("X ");
            else printf("O ");
        }
        printf("\n");
    }
}

// Fonction pour jouer un tour
int make_move(NeuralNetwork *nn, int **board) {
    double input[INPUT_SIZE];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            input[i * 3 + j] = board[i][j];
        }
    }

    forward_propagate(nn, input);

    // Trouver la meilleure case à jouer
    double max_val = -1.0;
    int best_move = -1;
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        if (board[i / 3][i % 3] == 0 && nn->output_layer[i] > max_val) 
        {
            max_val = nn->output_layer[i];
            best_move = i;
        }
    }
    return best_move;
}


