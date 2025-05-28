typedef struct Hypergraphe {
    int combo;          // Number of pieces to align (e.g., 4 for Connect Four)
    int progression;    // Move count
    int score;          // Player score
    int dimension;      // Board dimension (e.g., 3 for 3D)
    int taille;         // Board size (e.g., 4 = 4x4x4...)
    int nbj;            // Number of players
    int **mat;          // Adjacency matrix (edges × vertices)
    int nb_alignements;  // Number of possible winning alignments
    int nb_cases;       // Total cells (taille^dimension)
} hgraphe;

typedef struct Coup {
    int indice;         // Move index
    int* coordonnees;   // n-D coordinates (e.g., [x,y,z,...])
    int joueur;         // Player ID (1 or 2)
} coup;
int coords_to_index(hgraphe *h, int *coords) {
    int index = 0;
    int stride = 1;
    for (int d = h->dimension - 1; d >= 0; d--) {
        index += coords[d] * stride;
        stride *= h->taille;
    }
    return index;
}
void apply_move(hgraphe *h, coup *c) {
    int idx = coords_to_index(h, c->coordonnees);
    h->mat[idx] = c->joueur;  // Update board state
    h->progression++;
}
void draw_slice(hgraphe *h, SDL_Renderer *renderer, int fixed_dim, int fixed_val) {
    int cell_size = 40;
    int coords[h->dimension];
    coords[fixed_dim] = fixed_val;  // Fixed dimension (e.g., Z=0)

    for (int i = 0; i < h->taille; i++) {         // X-axis
        for (int j = 0; j < h->taille; j++) {     // Y-axis
            // Set varying dimensions (skip fixed_dim)
            int dim = 0;
            for (int d = 0; d < h->dimension; d++) {
                if (d == fixed_dim) continue;
                coords[d] = (dim == 0) ? i : j;  // Assign X/Y
                dim++;
            }

            int idx = coords_to_index(h, coords);
            int player = h->mat[idx];

            SDL_Rect rect = {i * cell_size, j * cell_size, cell_size, cell_size};
            if (player == 0) SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);      // Empty
            else if (player == 1) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);     // P1
            else SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);                      // P2
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

// --- Your structs here ---

int main() {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("n-D Connect Four", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Initialize a 3D board (4x4x4)
    hgraphe h;
    h.dimension = 3;
    h.taille = 4;
    h.nb_cases = h.taille * h.taille * h.taille;
    h.mat = (int*)calloc(h.nb_cases, sizeof(int));  // Zero-initialized

    // Example move: Player 1 places at (1, 2, 0)
    coup c;
    c.joueur = 1;
    c.coordonnees = (int[]){1, 2, 0};
    apply_move(&h, &c);

    // Main loop
    int fixed_dim = 2;  // Z-axis
    int fixed_val = 0;  // Slice Z=0
    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP) fixed_val = (fixed_val + 1) % h.taille;
                if (event.key.keysym.sym == SDLK_DOWN) fixed_val = (fixed_val - 1 + h.taille) % h.taille;
            }
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        draw_slice(&h, renderer, fixed_dim, fixed_val);
        SDL_RenderPresent(renderer);
    }

    free(h.mat);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
