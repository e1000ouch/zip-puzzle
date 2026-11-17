#include "grille.h"

/*
================================================================================
Fonction   : a_gagne
Auteur     : Anouk
Param      : curseur (Position*) - Position actuelle du joueur
             grille (Grille*) - Pointeur vers la grille
Traitement : Vérifie que toutes les cases ont été visitées (numero != 0) et
             que le joueur termine son parcours sur le dernier chiffre objectif.
Retour     : bool - true si la partie est gagnée, false sinon.
================================================================================
*/
bool a_gagne(Position *curseur, Grille *grille) {
    int dernier_chiffre = 0;

    for (int i = 0; i < grille->lignes; i++) {
        for (int j = 0; j < grille->colonnes; j++) {
            // Si une seule case a le numéro 0, la partie n'est pas finie
            if (grille->cellules[i][j].numero == 0) {
                return false;
            }
            if (grille->cellules[i][j].chiffre > dernier_chiffre) {
                dernier_chiffre = grille->cellules[i][j].chiffre;
            }
        }
    }

    // Le parcours doit se terminer sur le dernier chiffre objectif
    return grille->cellules[curseur->y][curseur->x].chiffre == dernier_chiffre;
}
