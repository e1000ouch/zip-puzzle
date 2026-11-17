#include "grille.h"
#include <stdlib.h>

/*
================================================================================
Fonction   : depart_aleatoire
Auteur     : Anouk
Param      : x (int*) - Pointeur pour stocker l'abscisse générée
             y (int*) - Pointeur pour stocker l'ordonnée générée
             taille_grille (int) - Dimension de la grille carrée
Traitement : Génère des coordonnées aléatoires et vérifie la parité pour
             assurer la faisabilité d'un chemin hamiltonien sur grille impaire.
Retour     : Position - La position de départ générée.
================================================================================
*/
Position depart_aleatoire(int *x, int *y, int taille_grille) {
    while (true) {
        *x = rand() % taille_grille;
        *y = rand() % taille_grille;

        if (taille_grille * taille_grille % 2 == 1 && ((*x + *y) % 2 == 1)) {
            continue;
        }

        Position p = { *x, *y, 1 };
        return p;
    }
}

/*
================================================================================
Fonction   : est_valide
Auteur     : Anouk
Param      : x (int) - Coordonnée X à tester
             y (int) - Coordonnée Y à tester
             grille (int) - Taille de la grille
             visite (int**) - Tableau des cases déjà visitées
Traitement : Vérifie si la case est dans les limites et n'a pas encore été visitée.
Retour     : bool - true si la case est accessible, false sinon.
================================================================================
*/
bool est_valide(int x, int y, int grille, int **visite) {
    return x >= 0 && x < grille && y >= 0 && y < grille && !(visite[x][y]);
}

// Directions pour se déplacer dans la grille
static const int directionsx[4] = {0, 1, 0, -1};
static const int directionsy[4] = {-1, 0, 1, 0};

/*
================================================================================
Fonction   : nb_voisins_libres
Auteur     : Emilien
Param      : ligne, colonne (int) - Case à évaluer
             taille_grille (int) - Taille de la grille
             visite (int**) - Matrice de suivi du chemin
Traitement : Compte les voisins orthogonaux encore libres d'une case.
Retour     : int - Nombre de voisins libres (0 à 4).
================================================================================
*/
static int nb_voisins_libres(int ligne, int colonne, int taille_grille, int **visite) {
    int n = 0;
    for (int i = 0; i < 4; i++) {
        if (est_valide(ligne + directionsx[i], colonne + directionsy[i], taille_grille, visite)) n++;
    }
    return n;
}

/*
================================================================================
Fonction   : hamiltonien
Auteur     : Anouk (heuristique d'ordre des directions : Emilien)
Param      : ligne, colonne (int) - Coordonnées actuelles
             pas (int) - Étape actuelle dans le chemin (1 à N*N)
             taille_grille (int) - Taille de la grille
             visite (int**) - Matrice de suivi du chemin
             debut_temps (clock_t) - Temps de départ pour le timeout
             limite_temps (double) - Durée max autorisée en secondes
Traitement : Algorithme récursif (backtracking) pour trouver un chemin
             hamiltonien passant par toutes les cases. Inclut une sécurité
             temporelle pour éviter les blocages sur les grandes grilles.
             Les directions sont mélangées aléatoirement puis triées selon
             l'heuristique de Warnsdorff (on explore d'abord la case qui a le
             moins d'issues) : chaque grille a un chemin différent et la
             recherche aboutit quasi instantanément, même en 10x10.
Retour     : int - 1 si un chemin est trouvé, 0 sinon.
================================================================================
*/
int hamiltonien(int ligne, int colonne, int pas, int taille_grille, int **visite, clock_t debut_temps, double limite_temps) {

    // Vérification du timeout
    double temps_ecoule = (double)(clock() - debut_temps) / CLOCKS_PER_SEC;
    if (temps_ecoule > limite_temps) {
        return 0;
    }

    visite[ligne][colonne] = pas;

    // Condition d'arrêt : Chemin complet trouvé
    if (pas == taille_grille * taille_grille) {
        return 1;
    }

    // Ordre d'exploration : mélange aléatoire (Fisher-Yates)...
    int ordre[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--) {
        int k = rand() % (i + 1);
        int tmp = ordre[i]; ordre[i] = ordre[k]; ordre[k] = tmp;
    }
    // ... puis tri stable par nombre d'issues croissant (Warnsdorff)
    int issues[4];
    for (int i = 0; i < 4; i++) {
        int l = ligne + directionsx[ordre[i]];
        int c = colonne + directionsy[ordre[i]];
        issues[i] = est_valide(l, c, taille_grille, visite) ? nb_voisins_libres(l, c, taille_grille, visite) : 99;
    }
    for (int i = 1; i < 4; i++) {
        int d = ordre[i], e = issues[i], j = i - 1;
        while (j >= 0 && issues[j] > e) {
            ordre[j + 1] = ordre[j];
            issues[j + 1] = issues[j];
            j--;
        }
        ordre[j + 1] = d;
        issues[j + 1] = e;
    }

    for (int i = 0; i < 4; i++) {
        int new_l = ligne + directionsx[ordre[i]];
        int new_c = colonne + directionsy[ordre[i]];

        if (est_valide(new_l, new_c, taille_grille, visite)) {
            if (hamiltonien(new_l, new_c, pas + 1, taille_grille, visite, debut_temps, limite_temps)) {
                return 1;
            }

            // Vérification post-récursion pour arrêter proprement en cas de timeout
            if ((double)(clock() - debut_temps) / CLOCKS_PER_SEC > limite_temps) {
                visite[ligne][colonne] = 0;
                return 0;
            }
        }
    }

    // Backtracking : on libère la case
    visite[ligne][colonne] = 0;
    return 0;
}

/*
================================================================================
Fonction   : placer_numeros_sur_chemin
Auteur     : Anouk
Param      : grille (Grille*) - Grille à modifier
             visited (int**) - Le chemin hamiltonien généré
             taille (int) - Taille de la grille
             nb_numeros (int) - Nombre de chiffres presets à placer
Traitement : Répartit les chiffres (objectifs) le long du chemin généré
             avec une variation aléatoire pour éviter la linéarité parfaite.
Retour     : Aucun (void)
================================================================================
*/
void placer_numeros_sur_chemin(Grille *grille, int **visited, int taille, int nb_numeros) {
    if (nb_numeros <= 0) return;

    int total_cases = taille * taille;
    int espacement_base = total_cases / nb_numeros;

    typedef struct {
        int x, y;
    } coordonees;

    coordonees *chemin = (coordonees*)malloc(total_cases * sizeof(coordonees));

    // Reconstruction linéaire du chemin
    for (int pas = 1; pas <= total_cases; pas++) {
        for (int i = 0; i < taille; i++) {
            for (int j = 0; j < taille; j++) {
                if (visited[i][j] == pas) {
                    chemin[pas-1].x = j;
                    chemin[pas-1].y = i;
                }
            }
        }
    }

    // Placement des numéros
    for (int num = 1; num <= nb_numeros; num++) {
        int position;

        if (num == 1) {
            position = 0;
        } else if (num == nb_numeros) {
            position = total_cases - 1;
        } else {
            // Calcul position avec bruit aléatoire
            int position_ideale = (num - 1) * espacement_base;
            int variation = espacement_base / 5;
            position = position_ideale + (rand() % (2 * variation + 1)) - variation;

            if (position < num - 1) position = num - 1;
            if (position >= total_cases) position = total_cases - 1;
        }

        int x = chemin[position].x;
        int y = chemin[position].y;

        grille->cellules[y][x].chiffre = num;
    }

    free(chemin);
}
