/*
 * Tests de la logique du jeu ZIP (sans interface console).
 * Compilation et exécution : make test
 */
#include "grille.h"
#include "sauvegarde.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int nb_tests = 0;
static int nb_echecs = 0;

#define VERIFIER(condition, message)                                   \
    do {                                                               \
        nb_tests++;                                                    \
        if (!(condition)) {                                            \
            nb_echecs++;                                               \
            printf("  ECHEC : %s (ligne %d)\n", message, __LINE__);    \
        }                                                              \
    } while (0)

/* Génère une grille jouable comme le fait main.c et renvoie le chemin solution. */
static int **generer(Grille *grille, Position *curseur, int taille, int nb_numeros) {
    int **visite = malloc(taille * sizeof(int *));
    for (int i = 0; i < taille; i++) visite[i] = calloc(taille, sizeof(int));

    initialiser_grille(grille, taille, taille);
    int x, y;
    bool trouve = false;
    while (!trouve) {
        for (int i = 0; i < taille; i++)
            for (int j = 0; j < taille; j++) visite[i][j] = 0;
        *curseur = depart_aleatoire(&x, &y, taille);
        trouve = hamiltonien(x, y, 1, taille, visite, clock(), LIMITE_GENERATION_S);
    }
    placer_numeros_sur_chemin(grille, visite, taille, nb_numeros);

    for (int i = 0; i < taille; i++)
        for (int j = 0; j < taille; j++)
            if (grille->cellules[i][j].chiffre == 1) {
                curseur->x = j;
                curseur->y = i;
                curseur->chiffre_actuel = 1;
                grille->cellules[i][j].numero = 1;
            }
    return visite;
}

static void liberer(int **visite, int taille) {
    for (int i = 0; i < taille; i++) free(visite[i]);
    free(visite);
}

/* Touche à presser pour aller de (x1, y1) à la case voisine (x2, y2). */
static char direction(int x1, int y1, int x2, int y2) {
    if (x2 == x1 + 1) return 'd';
    if (x2 == x1 - 1) return 'q';
    if (y2 == y1 + 1) return 's';
    return 'z';
}

/* Retrouve la case d'ordre `pas` dans le chemin solution. */
static void case_du_pas(int **visite, int taille, int pas, int *x, int *y) {
    *x = 0;
    *y = 0;
    for (int i = 0; i < taille; i++)
        for (int j = 0; j < taille; j++)
            if (visite[i][j] == pas) { *x = j; *y = i; return; }
}

static void test_chemin_hamiltonien(int taille) {
    Grille grille;
    Position curseur;
    int **visite = generer(&grille, &curseur, taille, 6);
    int n = taille * taille;

    // Chaque pas 1..n apparaît exactement une fois, et deux pas consécutifs sont voisins
    bool complet = true, contigu = true;
    for (int pas = 1; pas <= n; pas++) {
        int occurrences = 0;
        for (int i = 0; i < taille; i++)
            for (int j = 0; j < taille; j++)
                if (visite[i][j] == pas) occurrences++;
        if (occurrences != 1) complet = false;
    }
    for (int pas = 1; pas < n && complet; pas++) {
        int x1, y1, x2, y2;
        case_du_pas(visite, taille, pas, &x1, &y1);
        case_du_pas(visite, taille, pas + 1, &x2, &y2);
        if (abs(x1 - x2) + abs(y1 - y2) != 1) contigu = false;
    }
    VERIFIER(complet, "le chemin passe une seule fois par chaque case");
    VERIFIER(contigu, "le chemin n'avance que d'une case orthogonale a la fois");
    liberer(visite, taille);
}

static void test_partie_gagnee(int taille, int nb_numeros) {
    Grille grille;
    Position curseur;
    int **visite = generer(&grille, &curseur, taille, nb_numeros);
    int n = taille * taille;

    int x, y;
    case_du_pas(visite, taille, 1, &x, &y);
    VERIFIER(curseur.x == x && curseur.y == y, "le joueur demarre sur le chiffre 1");

    bool gagne_trop_tot = false;
    for (int pas = 2; pas <= n; pas++) {
        int nx, ny;
        case_du_pas(visite, taille, pas, &nx, &ny);
        deplacer_curseur(&curseur, &grille, direction(curseur.x, curseur.y, nx, ny));
        if (pas < n && a_gagne(&curseur, &grille)) gagne_trop_tot = true;
    }
    VERIFIER(!gagne_trop_tot, "pas de victoire avant la derniere case");
    VERIFIER(a_gagne(&curseur, &grille), "suivre la solution mene a la victoire");
    VERIFIER(curseur.chiffre_actuel == nb_numeros, "tous les chiffres sont atteints dans l'ordre");
    liberer(visite, taille);
}

static void test_retour_arriere(void) {
    Grille grille;
    Position curseur;
    int taille = 6;
    int **visite = generer(&grille, &curseur, taille, 4);

    // Avance de 10 cases sur la solution, puis recule de 10 cases
    int chemin_x[11], chemin_y[11];
    chemin_x[0] = curseur.x;
    chemin_y[0] = curseur.y;
    for (int pas = 2; pas <= 11; pas++) {
        int nx, ny;
        case_du_pas(visite, taille, pas, &nx, &ny);
        deplacer_curseur(&curseur, &grille, direction(curseur.x, curseur.y, nx, ny));
        chemin_x[pas - 1] = curseur.x;
        chemin_y[pas - 1] = curseur.y;
    }
    for (int k = 9; k >= 0; k--) {
        deplacer_curseur(&curseur, &grille, direction(curseur.x, curseur.y, chemin_x[k], chemin_y[k]));
    }

    int cases_visitees = 0;
    for (int i = 0; i < taille; i++)
        for (int j = 0; j < taille; j++)
            if (grille.cellules[i][j].numero != 0) cases_visitees++;

    VERIFIER(curseur.x == chemin_x[0] && curseur.y == chemin_y[0], "le retour arriere ramene au depart");
    VERIFIER(cases_visitees == 1, "le retour arriere efface les cases parcourues");
    VERIFIER(curseur.chiffre_actuel == 1, "le chiffre courant revient a 1");
    liberer(visite, taille);
}

static void test_mouvements_interdits(void) {
    Grille grille;
    Position curseur;
    int taille = 5;
    int **visite = generer(&grille, &curseur, taille, 5);

    Position avant = curseur;
    deplacer_curseur(&curseur, &grille, 'h');   // touche sans effet
    deplacer_curseur(&curseur, &grille, 'x');
    VERIFIER(curseur.x == avant.x && curseur.y == avant.y, "une touche inconnue ne deplace pas le curseur");

    // Le chiffre 3 ne peut pas etre atteint avant le 2
    int x3 = -1, y3 = -1;
    for (int i = 0; i < taille; i++)
        for (int j = 0; j < taille; j++)
            if (grille.cellules[i][j].chiffre == 3) { x3 = j; y3 = i; }
    Grille copie = grille;
    Position autre = {x3 + 1 < taille ? x3 + 1 : x3 - 1, y3, 1};
    copie.cellules[autre.y][autre.x].numero = 5;
    deplacer_curseur(&autre, &copie, autre.x > x3 ? 'q' : 'd');
    VERIFIER(!(autre.x == x3 && autre.y == y3), "impossible d'atteindre un chiffre hors ordre");
    liberer(visite, taille);
}

static void test_sauvegarde(void) {
    Grille grille, relue;
    Position curseur, curseur_relu;
    int **visite = generer(&grille, &curseur, 7, 6);
    const char *fichier = "test_sauvegarde.txt";

    sauvegarder_partie(&grille, &curseur, fichier);
    bool ok = charger_partie(&relue, &curseur_relu, fichier);
    remove(fichier);

    bool identique = ok && relue.lignes == grille.lignes && relue.colonnes == grille.colonnes
                     && curseur_relu.x == curseur.x && curseur_relu.y == curseur.y
                     && curseur_relu.chiffre_actuel == curseur.chiffre_actuel;
    for (int i = 0; identique && i < grille.lignes; i++)
        for (int j = 0; j < grille.colonnes; j++)
            if (relue.cellules[i][j].numero != grille.cellules[i][j].numero
                || relue.cellules[i][j].chiffre != grille.cellules[i][j].chiffre)
                identique = false;

    VERIFIER(identique, "une partie rechargee est identique a la partie sauvegardee");
    VERIFIER(!charger_partie(&relue, &curseur_relu, "fichier_inexistant.txt"),
             "charger sans sauvegarde echoue proprement");

    FILE *corrompu = fopen(fichier, "w");
    fprintf(corrompu, "42 42\n0 0 1\n");
    fclose(corrompu);
    VERIFIER(!charger_partie(&relue, &curseur_relu, fichier), "une sauvegarde corrompue est refusee");
    remove(fichier);
    liberer(visite, 7);
}

int main(void) {
    srand((unsigned) time(NULL));

    printf("Generation de chemins hamiltoniens (5x5 a 10x10)...\n");
    for (int taille = TAILLE_MIN; taille <= TAILLE_MAX; taille++) test_chemin_hamiltonien(taille);

    printf("Parties completes en suivant la solution...\n");
    for (int k = 0; k < 20; k++) test_partie_gagnee(TAILLE_MIN + k % 6, 2 + k % 11);

    printf("Retour arriere...\n");
    for (int k = 0; k < 10; k++) test_retour_arriere();

    printf("Mouvements interdits...\n");
    test_mouvements_interdits();

    printf("Sauvegarde et chargement...\n");
    test_sauvegarde();

    printf("\n%d tests, %d echec(s)\n", nb_tests, nb_echecs);
    return nb_echecs == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
