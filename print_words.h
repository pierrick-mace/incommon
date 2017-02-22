#ifndef __PRINT_WORDS_H__
#define __PRINT_WORDS_H__

#include <stdbool.h>
#include "options.h"

// Renvoie true si le tableau d'adresse c ne contient aucune valeur nulle
// Renvoie false sinon.
bool array_contains_no_zero(size_t *c, size_t size);

// Affiche le nombre d'occurences du mot word pour chaque fichier
void print_occurence_line(const char *word, size_t *c, size_t size);

// Fonction principale d'affichage des mots
void print_words(options *optvar);

// Affiche les noms des fichiers
void print_file_names(options *optvar);
#endif

