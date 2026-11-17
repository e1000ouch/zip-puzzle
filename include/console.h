#ifndef CONSOLE_H
#define CONSOLE_H

/*
Configure la console Windows : UTF-8 et couleurs ANSI.
 */
void initialiser_console(void);

/*
Place le curseur de la console en (x, y) pour redessiner sans clignotement.
 */
void gotoxy(int x, int y);

/*
Masque / affiche le curseur clignotant de la console.
 */
void cacher_curseur(void);
void afficher_curseur(void);

#endif
