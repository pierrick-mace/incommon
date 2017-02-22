#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <stdbool.h>
#include "sllist.h"
#include "hashtbl.h"
#include "defines.h"

typedef struct options options;

// Enumeration des types de casse differents pour l'option --case
enum {
  NIL,
  UPPER,
  LOWER
};

// Enumeration des différentes options
enum {
	option_exclude,
	option_case,
	option_separator,
	option_sort,
	option_help
};


// Structure principale du projet
// sort: true si l'option --case est utilisée, false sinon
// type: prend une valeur parmis l'énumeration des types de casse différents
// separator: chaine de caractères contenant le type des séparateurs (1 ou plus)
// files: tableau de chaînes de caractères contenant les noms des différents
// fichiers
// xfile: chaîne de caractères contenant le nom du fichier de mots à exclure
// fileptr: tableau contenant le pointeur vers le flot de chaque fichier
// xfileptr: contient le pointeur vers le flot du fichier des mots à exclure
// nb_files: nombre de fichiers à comparer
// words: liste contenant les mots du fichier insérés comme clé dans la table 
// de hachage
// counters: liste des compteurs insérés comme valeur dans la table de hachage
// xlist: liste des mots à exclure
// hashtbl: pointeur sur la table de hachage
// hashfun: pointeur sur la fonction de hachage 

struct options {
	bool sort;
	int type;
	char *separator;
	char **files;
	char *xfile;
	FILE **fileptr;
	FILE *xfileptr;
	size_t nb_files;
	sllist *words;
	sllist *counters;
  sllist *xlist;
	hashtable *hashtbl;
	size_t (*hashfun)(const char *);
};

// Affiche l'aide
void print_help(void);

// Gère la ligne de commande, renvoie l'adresse d'une structure de type
// options, allouée dynamiquement au préalable
options *parse_arguments(int argc, char *argv[]);

// Alloue une zone mémoire contenant une structure de type options, renvoie 
// l'adresse de la structure
options *initialize_options(void);

// Désalloue toutes les zones mémoire allouées précédemment
void clean_memory(options *optvar);
#endif
