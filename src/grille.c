#include "grille.h"
#include <stdio.h>

// Codes de couleur ANSI pour le dégradé

#define RESET   "\033[0m"
// BLEU - Début du dégradé
#define b1  "\e[48;5;17m"  // Bleu très foncé
#define b2  "\e[48;5;18m"  // Bleu foncé
#define b3  "\e[48;5;19m"  // Bleu marine
#define b4  "\e[48;5;20m"  // Bleu
#define b5  "\e[48;5;21m"  // Bleu roi

// CYAN/TURQUOISE - Transition
#define c1  "\e[48;5;27m"  // Bleu vif
#define c2  "\e[48;5;33m"  // Cyan foncé
#define c3  "\e[48;5;39m"  // Cyan
#define c4  "\e[48;5;45m"  // Cyan clair
#define c5  "\e[48;5;51m"  // Cyan très clair

// VERT - Milieu
#define v1  "\e[48;5;40m"  // Vert foncé
#define v2  "\e[48;5;46m"  // Vert lime
#define v3  "\e[48;5;82m"  // Vert vif
#define v4  "\e[48;5;118m" // Vert clair
#define v5  "\e[48;5;154m" // Vert lime clair

// JAUNE - Transition
#define j1  "\e[48;5;190m" // Jaune-vert
#define j2  "\e[48;5;226m" // Jaune vif
#define j3  "\e[48;5;220m" // Jaune or
#define j4  "\e[48;5;214m" // Jaune-orange
#define j5  "\e[48;5;208m" // Orange clair

// ORANGE
#define o1  "\e[48;5;202m" // Orange
#define o2  "\e[48;5;196m" // Orange-rouge
#define o3  "\e[48;5;160m" // Rouge-orange
#define o4  "\e[48;5;124m" // Rouge foncé
#define o5  "\e[48;5;88m"  // Rouge bordeaux

#define ro "\e[38;5;205m" // ROSE_VIF (texte pour les chiffres)

/*
================================================================================
Fonction   : initialiser_grille
Auteur     : Anouk
Param      : grille (Grille*) - Pointeur vers la structure à initialiser
             lignes (int) - Nombre de lignes souhaité
             colonnes (int) - Nombre de colonnes souhaité
Traitement : Définit les dimensions de la grille (bornées par MIN/MAX) et
             initialise toutes les cellules à 0.
Retour     : Aucun (void)
================================================================================
*/
void initialiser_grille(Grille *grille, int lignes, int colonnes) {
    if (!grille) return;

    // Bornage des dimensions
    if (lignes < TAILLE_MIN) lignes = TAILLE_MIN;
    if (lignes > TAILLE_MAX) lignes = TAILLE_MAX;
    if (colonnes < TAILLE_MIN) colonnes = TAILLE_MIN;
    if (colonnes > TAILLE_MAX) colonnes = TAILLE_MAX;

    grille->lignes = lignes;
    grille->colonnes = colonnes;

    // Initialisation des données
    for (int i = 0; i < lignes; ++i) {
        for (int j = 0; j < colonnes; ++j) {
            grille->cellules[i][j].numero = 0;
            grille->cellules[i][j].chiffre = 0;
        }
    }
}

/*
================================================================================
Fonction   : afficher_grille
Auteur     : Anouk
Param      : grille (Grille*) - Pointeur vers la grille
             curseur (Position) - Position actuelle du joueur pour affichage
Traitement : Affiche la grille caractère par caractère en appliquant un
             dégradé de couleurs ANSI basé sur la progression du chemin.
Retour     : Aucun (void)
================================================================================
*/
void afficher_grille(Grille *grille, Position curseur) {
    // Tableau de couleurs avec un dégradé fluide : ROUGE -> JAUNE -> VERT -> CYAN -> BLEU
    const char* couleurs[] = {
        o5, o4, o3, o2, o1,
        j5, j4, j3, j2, j1,
        v5, v4, v3, v2, v1,
        c5, c4, c3, c2, c1,
        b5, b4, b3, b2, b1
    };
    int nb_couleurs = 25;
    int total_cases = grille->lignes * grille->colonnes;

    for (int i = 0; i < grille->lignes; i++) {
        for (int j = 0; j < grille->colonnes; j++) {
            int numero_case = grille->cellules[i][j].numero;

            // 1. Affichage du CURSEUR (X / Coeur)
            if (curseur.x == j && curseur.y == i) {
                int index_couleur = (numero_case - 1) * nb_couleurs / total_cases;
                if (index_couleur >= nb_couleurs) index_couleur = nb_couleurs - 1;
                if (index_couleur < 0) index_couleur = 0;

                printf("%s| ♥  |%s ", couleurs[index_couleur], RESET);
            }
            // 2. Affichage des cases VISITÉES (Chemin coloré)
            else if (numero_case != 0) {
                int index_couleur = (numero_case - 1) * nb_couleurs / total_cases;
                if (index_couleur >= nb_couleurs) index_couleur = nb_couleurs - 1;
                if (index_couleur < 0) index_couleur = 0;

                printf("%s|    |%s ", couleurs[index_couleur], RESET);
            }
            // 3. Affichage des CHIFFRES PRESETS (Objectifs)
            else if (grille->cellules[i][j].chiffre != 0) {
                int chiffre = grille->cellules[i][j].chiffre;

                // Si la case a été visitée (cas rare ici car géré par le if précédent,
                // mais conservé pour robustesse d'affichage)
                if (grille->cellules[i][j].numero != 0) {
                    int max_chiffre = 0;
                    // Recherche du max pour calibrer la couleur
                    for (int pi = 0; pi < grille->lignes; pi++) {
                        for (int pj = 0; pj < grille->colonnes; pj++) {
                            if (grille->cellules[pi][pj].chiffre > max_chiffre) {
                                max_chiffre = grille->cellules[pi][pj].chiffre;
                            }
                        }
                    }

                    int numero_estime = (chiffre - 1) * total_cases / max_chiffre + 1;
                    int index_couleur = (numero_estime - 1) * nb_couleurs / total_cases;
                    if (index_couleur >= nb_couleurs) index_couleur = nb_couleurs - 1;
                    if (index_couleur < 0) index_couleur = 0;

                    if (chiffre < 10) printf("%s| %d  |%s ", couleurs[index_couleur], chiffre, RESET);
                    else printf("%s| %d |%s ", couleurs[index_couleur], chiffre, RESET);
                }
                // Affichage standard du chiffre (non visité) en rose
                else {
                    if (chiffre < 10) printf(ro "| %d  |" RESET " ", chiffre);
                    else printf(ro "| %d |" RESET " ", chiffre);
                }
            }
            // 4. Affichage des cases VIDES
            else {
                printf("|    | ");
            }
        }
        printf("\n");
    }
}