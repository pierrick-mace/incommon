#ifndef __FILE_H__
#define __FILE_H__

#include "hashtbl.h"
#include "sllist.h"

// Enumeration pour la récupération des erreurs
enum 
{
	F_ERROR,
	F_SUCCESS
};

// Enumeration pour le type de fichier utilisé dans la fonction parse_file
enum
{
	file_first,
	file_regular,
	file_excluded
};

// Lit chaque caractère du fichier pointé par file, et construit les mots en
// fonction des séparateurs, le mot est ensuite inséré dans la table de hachage
// ou son nombre d'occurences est incrémenté s'il existe déjà
// Renvoie F_ERROR en cas d'erreur, F_SUCCESS sinon
int parse_file(FILE *file, options *optvar, int filetype, int file_idx);

// Ouvre les fichiers dont le chemin est contenu dans la variable de contrôle
// optvar
// Renvoie F_ERROR en cas d'erreur, F_SUCCESS sinon
int open_files(options *optvar);

// Convertit chaque caractère de s en majuscule
void stringToUpper(char *s);
// Convertit chaque caractère de s en minuscule
void stringToLower(char *s);

// Crée la table de hachage, renvoie F_ERROR en cas d'erreur, F_SUCCESS sinon
int create_hashtable(options *optvar);

// Crée les listes nécessaires au fonctionnement du programme, renvoie F_ERROR
// en cas d'erreur, F_SUCCESS sinon
int create_lists(options *optvar);

// Appelle la fonction parse_file pour chaque fichier ouver par open_files
// Renvoie F_ERROR en cas d'erreur, F_SUCCESS sinon
int load_words(options *optvar);

// Ajoute le mot à exclure string dans la table de hachage ainsi qu'un pointeur
// vers string dans la liste xlist
// Renvoie F_ERROR en cas d'erreur, F_SUCCESS sinon
int add_excluded_word(options *optvar, char *string);

// Ajoute le mot string dans la table de hachage, ou incrémente son nombre 
// d'occurence s'il est déjà présent
// Renvoie F_ERROR en cas d'erreur, F_SUCCESS sinon
int add_word(options *optvar, char *string, int idx);
#endif
