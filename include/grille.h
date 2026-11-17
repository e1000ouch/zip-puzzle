#ifndef GRILLE_H
#define GRILLE_H

#include <stdbool.h>
#include <time.h>

#define TAILLE_MIN 5
#define TAILLE_MAX 10
#define NB_NUMEROS_MAX 12
#define LIMITE_GENERATION_S 0.9 // timeout d'une tentative de generation

typedef struct {
    int numero;  // ordre de passage du joueur sur la case (0 = case non visitee)
    int chiffre; // chiffre objectif impose (1..12), 0 si la case n'en porte pas
} Case;

typedef struct {
    int lignes;
    int colonnes;
    Case cellules[TAILLE_MAX][TAILLE_MAX];
} Grille;

typedef struct {
    int x;
    int y;
    int chiffre_actuel; // dernier chiffre objectif atteint par le joueur
} Position;


/*
Affiche la grille de jeu dans la console avec la position actuelle du curseur et les couleurs.
 */
void afficher_grille(Grille *grille, Position curseur);

/*
Initialise une grille avec les dimensions spécifiées et met toutes les cases à zéro.
 */
void  initialiser_grille(Grille* grille, int lignes, int colonnes);

/*
Gère le déplacement du curseur (touches Z/Q/S/D ou W/A/S/D) et applique les règles du jeu.
Les flèches directionnelles sont converties en Z/Q/S/D par la boucle de jeu.
 */
void deplacer_curseur(Position *curseur, Grille *grille, char touche);

/*
Vérifie si les coordonnées sont dans la grille et si la case n'a pas encore été visitée.
 */
bool est_valide(int x, int y, int taille_grille, int **visite);

/*
Génère une position de départ aléatoire respectant les contraintes de parité pour le chemin hamiltonien.
 */
Position depart_aleatoire(int *x, int *y, int taille_grille);

/*
Algorithme récursif qui tente de trouver un chemin hamiltonien dans la grille avec une limite de temps.
 */
int hamiltonien(int x, int y, int pas, int taille_grille, int **visite, clock_t debut_temps, double limite_temps);

/*
Vérifie si le joueur a gagné : toutes les cases sont visitées et le curseur
se trouve sur le dernier chiffre objectif.
 */
bool a_gagne(Position *curseur, Grille *grille);

/*
Place les numéros (objectifs) sur la grille en suivant le chemin hamiltonien généré.
 */
void placer_numeros_sur_chemin(Grille *grille, int **visited, int taille, int nb_numeros);


#endif
