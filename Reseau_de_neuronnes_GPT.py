import torch
import torch.nn as nn
import torch.nn.functional as F
import random
import numpy as np
from torch.utils.data import DataLoader

# ----- Plateau nD avec grille 1D -----

class Plateau:
    def __init__(self, dimension, taille, combo):
        self.dimension = dimension
        self.taille = taille
        self.combo = combo
        self.grille = [0]*(taille**dimension)#torch.zeros((taille**dimension,), dtype=int)

    def index(self, coord):
        idx = 0
        for i in range(self.dimension):
            idx = idx * self.taille + coord[i]
        return idx

    def coord(self, index):
        crd = np.zeros((self.dimension,), dtype=int)
        for i in range(self.dimension-1,-1,-1):
            crd[i] = index%self.taille
            index //= self.taille
        return crd

    def __getitem__(self, coord):
        if type(coord) is not int:
            coord = self.index(coord)
        return self.grille[coord]

    def __setitem__(self, coord, val):
        if type(coord) is not int:
            coord = self.index(coord)
        self.grille[coord] = val
    
    def get_component(self, index, i):
        return (index // (self.taille**(self.dimension-1-i))) % self.taille


# ----- Vérification de victoire -----

def directions(dimension):
    r = np.zeros((dimension,), dtype=int)
    r[0] = 1
    while True:
        yield r
        i = 0
        while i < dimension:
            if r[i] == 1:
                r[i] = -1
            else:
                r[i] += 1
                break
            i += 1
        if i == dimension:
            return

def verifie_victoire(plateau, coup):
    joueur = plateau[coup]
    for d in directions(plateau.dimension):
        compteur = 1
        for s in [d, -d]:
            for j in range(1,plateau.taille):
                pos = coup + j * s
                if not all(0 <= x < plateau.taille for x in pos):
                    break
                if plateau[pos] != joueur:
                    break
                compteur += 1
                if compteur >= plateau.combo:
                    return True
    return False

# ----- Réseau de Neurones -----

class NeuralNetwork(nn.Module):
    def __init__(self, dimension, taille, combo, nbj, p):
        super().__init__()
        self.dimension = dimension
        self.taille = taille
        self.combo = combo
        self.nbj = nbj
        self.nb_cases = taille ** dimension
        self.nb_alignements = (3 * taille - 2 * combo + 2) ** dimension - self.nb_cases
        self.nb_alignements //= 2
        self.flatten = torch.flatten

        self.model = nn.Sequential(
            nn.Flatten(),
            nn.Linear(self.nbj * self.nb_cases, p * self.nb_alignements),
            nn.ReLU(),
            nn.Linear(p * self.nb_alignements, 3),
        )

    def forward(self, x):
        return self.model(x)


# ----- Conversion plateau → tenseur d’entrée -----

def plateau_to_tensor(plateau, nbj):
    x = torch.zeros(len(plateau.grille), nbj)
    for i, j in enumerate(plateau.grille):
        if j > 0:
            x[i, j - 1] = 1.0
    return x.view(-1)


# ----- Générateur de parties aléatoires -----

def nth_zero_index(array, n):
    for i in range(len(array)):
        if array[i] != 0:
            continue
        if n == 0:
            return i
        n -= 1
    return None

def generer_parties(dimension, taille, combo, nbj, nb_echantillons):
    plateau = Plateau(dimension, taille, combo)
    libres = taille ** dimension
    parties = []

    while len(parties) < nb_echantillons:
        for j in range(1,1+nbj):
            c = random.randint(0, libres - 1)
            libres -= 1
            i = nth_zero_index(plateau.grille, c)
            plateau[i] = j
            v = verifie_victoire(plateau, plateau.coord(i))
            if v or libres==0:
                if (v):
                    if j==1:
                        r=2
                    else:
                        r=0
                else:
                    r=1
                parties += [(plateau_to_tensor(plateau, nbj), r)]
                plateau = Plateau(dimension, taille, combo)
                libres = taille ** dimension
                break
    return parties

class MyDataset(torch.utils.data.Dataset):
    def __init__(exemple):
        self.data = exemple
    def __iter__(self):
        start = 0
        end = len(self.data)
        worker_info = torch.utils.data.get_worker_info()
        if worker_info is None:  # single-process data loading, return the full iterator
            iter_start = start
            iter_end = end
        else:  # in a worker process
            # split workload
            per_worker = int(math.ceil((end - start) / float(worker_info.num_workers)))
            worker_id = worker_info.id
            iter_start = start + worker_id * per_worker
            iter_end = min(iter_start + per_worker, end)
        return iter(self.data[start,end])
    def __len__(self):
        return len(self.data)
    def __get_item__(self, i):
        return self.data[i]

# ----- Entraînement du réseau -----

batch_size = 64
lr = 1e-3

def train_model(model, exemple):
    
    #Dataloader
    dataloader = DataLoader(exemple, batch_size, shuffle=True)

    model.train()
    ident = torch.eye(3)
    loss_fn = nn.CrossEntropyLoss()
    optimizer = torch.optim.SGD(model.parameters(), lr, momentum = 0.5)

    for batch, (X, y) in enumerate(dataloader):
        # Compute prediction and loss
        pred = model(X)
        loss = loss_fn(pred, torch.index_select(ident,0,y))
        # Backpropagation
        loss.backward()
        optimizer.step()
        optimizer.zero_grad()

        if batch % 100 == 0:
            size = len(exemple)
            loss, current = loss.item(), batch * dataloader.batch_size + len(X)
            print(f"loss: {loss:>7f}  [{current:>5d}/{size:>5d}]")

def test_loop(model, exemple):
    
    #Dataloader
    dataloader = DataLoader(exemple, batch_size, shuffle=True)

    model.eval()
    loss_fn = nn.CrossEntropyLoss()
    ident = torch.eye(3)
    size = len(exemple)
    test_loss, correct = 0, 0

    # Evaluating the model with torch.no_grad() ensures that no gradients are computed during test mode
    # also serves to reduce unnecessary gradient computations and memory usage for tensors with requires_grad=True
    with torch.no_grad():
        for X, y in dataloader:
            pred = model(X)
            test_loss += loss_fn(pred, torch.index_select(ident,0,y)).sum().item()
            correct += (pred.argmax(1) == y).type(torch.float).sum().item()

    test_loss /= size
    correct /= size
    print(f"Test Error: \n Accuracy: {(100*correct):>0.1f}%, Avg loss: {test_loss:>8f} \n")

# ----- Utilisation complète -----

def afficher_plateau(plateau):
    taille = plateau.taille
    cases = plateau.grille
    dim = plateau.dimension
    if dim != 2:
        print("Affichage uniquement disponible pour 2D.")
        return
    print("\nPlateau :")
    for y in range(taille):
        for x in range(taille):
            val = plateau[(y, x)]
            c = "." if val == 0 else str(val)
            print(c, end=" ")
        print()
    print()


def jouer_contre_reseau(model, dimension, taille, combo, nbj):
    if dimension != 2:
        print("Ce mode de jeu est pour l’instant limité à 2D.")
        return

    plateau = Plateau(dimension, taille, combo)
    joueur_humain = 1
    joueur_reseau = 2
    joueur = joueur_humain
    poids = torch.tensor([-15.,2.,1.])

    while True:
        afficher_plateau(plateau)

        if joueur == joueur_humain:
            print("Votre tour (ex: 0 2) :")
            coup = tuple(map(int, input("> ").split()))
            if len(coup) != dimension or not all(0 <= c < taille for c in coup):
                print("Entrée invalide.")
                continue
            if plateau[coup] != 0:
                print("Case déjà occupée.")
                continue
        else:
            # Tour du réseau : choisir le meilleur coup
            meilleurs_score = -float('inf')
            meilleur_coup = None
            for idx in range(len(plateau.grille)):
                pos = tuple(plateau.coord(idx))
                if plateau[pos] == 0:
                    plateau[pos] = joueur_reseau
                    x = plateau_to_tensor(plateau, nbj)
                    with torch.no_grad():
                        output = model(x.unsqueeze(0)).squeeze()
                        print(output)
                        proba = torch.softmax(output,0).matmul(poids)
                    if proba > meilleurs_score:
                        meilleurs_score = proba
                        meilleur_coup = pos
                    plateau[pos] = 0  # undo move
            coup = meilleur_coup
            print(f"Le réseau joue : {coup}")

        # Jouer le coup
        plateau[coup] = joueur

        if verifie_victoire(plateau, coup):
            afficher_plateau(plateau)
            if joueur == joueur_humain:
                print("🎉 Vous avez gagné !")
            else:
                print("🤖 Le réseau a gagné.")
            break

        if all(v != 0 for v in plateau.grille):
            afficher_plateau(plateau)
            print("Match nul.")
            break

        joueur = (joueur % nbj) + 1

def main():
    epochs = 10000
    # Paramètres du jeu
    dimension = 2
    taille = 3
    combo = 3
    nbj = 2

    # Crée le réseau
    model = NeuralNetwork(dimension, taille, combo, nbj, 2)

    # Génère des exemples pour l'entraînement
    exemples_train = generer_parties(dimension, taille, combo, nbj, 600)
    exemples_test = generer_parties(dimension, taille, combo, nbj, 400)
    for _ in range(epochs):
        train_model(model, exemples_train)
        test_loop(model, exemples_test)

    # Lance le jeu contre le réseau
    jouer_contre_reseau(model, dimension, taille, combo, nbj)


# Lancement du programme
if __name__ == "__main__":
    main()

