#include "console.h"
#include <windows.h>

/*
================================================================================
Fonction   : initialiser_console
Auteur     : Emilien
Param      : Aucun
Traitement : Passe la console en UTF-8 (caracteres speciaux du menu et du
             curseur) et active le traitement des sequences ANSI, necessaire
             pour afficher les couleurs dans l'ancienne console Windows.
Retour     : Aucun (void)
================================================================================
*/
void initialiser_console(void) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE sortie = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(sortie, &mode)) {
        SetConsoleMode(sortie, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

/*
================================================================================
Fonction   : gotoxy
Auteur     : Anouk
Param      : x (int) - Coordonnée colonne
             y (int) - Coordonnée ligne
Traitement : Déplace le curseur de la console à une position précise pour
             permettre un affichage fixe sans clignotement.
Retour     : Aucun (void)
================================================================================
*/
void gotoxy(int x, int y) {
    COORD coord = {(SHORT) x, (SHORT) y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/*
================================================================================
Fonction   : cacher_curseur
Auteur     : Anouk
Param      : Aucun
Traitement : Utilise l'API Windows pour rendre le curseur de saisie invisible
             pendant l'affichage de la grille.
Retour     : Aucun (void)
================================================================================
*/
void cacher_curseur(void) {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

/*
================================================================================
Fonction   : afficher_curseur
Auteur     : Emilien
Param      : Aucun
Traitement : Rend de nouveau visible le curseur de saisie (menus, saisies).
Retour     : Aucun (void)
================================================================================
*/
void afficher_curseur(void) {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 25;
    info.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}
