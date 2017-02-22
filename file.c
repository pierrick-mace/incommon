#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "options.h"
#include "file.h"

// parse_file: lit un flot caractère par caractère, jusqu'à ce qu'un séparateur
// soit atteint, insère/met à jour le nombre d'occurence du mot dans la table
// de hachage, ensuite la lecture reprend.

int parse_file(FILE *file, options *optvar, int filetype, int file_idx) {
  size_t size = STRLEN_MAX;
	char *string = malloc((size + 1) * sizeof(*string));
  
  IS_NULL_INST(string, 
               fprintf(stderr, "Erreur lors de l'allocation dynamique.\n"), 
               F_ERROR)

	bool is_word = false;
	size_t idx = 0;
	char c;
	
  // Lecture du fichier caractère par caratctère jusqu'à la fin du fichier
	while((c = fgetc(file)) != EOF)
	{
    // Si un séparateur est trouvé, et que l'option correspond alors la chaine
    // string forme un mot et peut être insérée dans la table de hachage
    // Séparateur par défaut: espace
    
		if((isspace(c) && strstr(optvar -> separator, "s") != NULL)
			|| (isalpha(c) && strstr(optvar -> separator, "a") != NULL)
			|| (iscntrl(c) && strstr(optvar -> separator, "c") != NULL)
			|| (isdigit(c) && strstr(optvar -> separator, "d") != NULL)
			|| (c == '\n' && strstr(optvar -> separator, "n") != NULL)
			|| (ispunct(c) && strstr(optvar -> separator, "p") != NULL))
		{
			is_word = true;
			string[idx] = '\0';
		}

    // Si is_word == true alors string forme un mot et son nombre d'occurence
    // est incrémentée ou ajoutée
		if(is_word == true)
		{
      // Si l'option --case est utilisée, convertit la chaine en
      // majuscule/minuscule
			
			if(optvar -> type != NIL) {
				optvar -> type == UPPER ? stringToUpper(string) : 
        stringToLower(string); 
			}
	
			if (filetype == file_excluded) {
				size_t *c = (size_t *)hashtable_value(optvar -> hashtbl, string);
				
				if (c == NULL) {
          IF_ERROR_INST(add_excluded_word(optvar, string), free(string), 
                        F_ERROR)
				}
			} else if (filetype == file_first) {
				if (optvar -> xfileptr != NULL) {
					size_t *c = (size_t *)hashtable_value(optvar -> hashtbl, string);
					
					if(c == NULL) {
            IF_ERROR_INST(add_word(optvar, string, file_idx), free(string), 
                          F_ERROR)
					} else {
            if (c[optvar -> nb_files] == 0) {
              IF_ERROR_INST(add_word(optvar, string, file_idx), free(string), 
                            F_ERROR)
            }
          }
				}	else {   
          IF_ERROR_INST(add_word(optvar, string, file_idx), free(string), 
                        F_ERROR)        
        }			
			} else {
        size_t *c = (size_t *)hashtable_value(optvar -> hashtbl, string);
        if (c != NULL) {
          if (c[0] > 0) {
            IF_ERROR_INST(add_word(optvar, string, file_idx), free(string), 
                          F_ERROR)
          }
        }
      }
      
      // Remise à zéro des variables pour la prochaine itération de la boucle
			is_word = false;	
			string[0] = 0;
			idx = 0;
		}
		
    // Si le mot n'est pas complet, on ajoute le caractère à string
		else
		{
      // Si la taille maximale de string est atteinte, on réalloue deux fois
      // la taille précédente
			if(idx == size)
			{
        size *= 2;
				char *tmp = realloc(string, size);        
        IS_NULL_INST(tmp, 
        fprintf(stderr, "Erreur lors de la reallocation de memoire\n"); 
        free(string), 
        F_ERROR)
				string = tmp;
			}
			
      // Ajout du caractère c dans string et incrémentation de la variable 
      // index pour la prochaine itération de la boucle
			string[idx] = c;
			idx++;
		}
	}
	
  // Lorsque la fonction est terminée, on libère la mémoire
	free(string);
	return F_SUCCESS;
}

int open_files(options *optvar) {
	for(size_t i = 0; i < optvar -> nb_files; i++) {
		optvar -> fileptr[i] = fopen(optvar -> files[i], "r");
    IS_NULL_INST(optvar -> fileptr[i], fprintf(stderr, 
    "Erreur a l'ouverture du fichier \"%s\"\n", optvar -> files[i]), 
    F_ERROR)
	}

	if (optvar -> xfile != NULL) {
		optvar -> xfileptr = fopen(optvar -> xfile, "r");
    IS_NULL_INST(optvar -> xfileptr, 
                fprintf(stderr, "Erreur a l'ouverture du fichier \"%s\"\n", 
                         optvar -> xfile), 
                F_ERROR)
	}

	return F_SUCCESS;
}

int create_hashtable(options *optvar) {

  IS_NULL_INST((optvar -> hashtbl = hashtable_empty((size_t (*)(const void *)) \
  optvar -> hashfun, (int  (*)(const void *, const void *)) strcmp)), \
  fprintf(stderr, "Erreur lors de la creation de la table de hachage.\n"), \
  F_ERROR)

	return F_SUCCESS;
}

int create_lists(options *optvar) {
  IS_NULL_INST((optvar -> words = sllist_empty()), 
                fprintf(stderr, "Erreur lors de la creation de la liste.\n"), 
                F_ERROR)

  IS_NULL_INST((optvar -> counters = sllist_empty()), 
                fprintf(stderr, "Erreur lors de la creation de la liste.\n"), 
                F_ERROR)

  if(optvar -> xfile != NULL) {
    IS_NULL_INST((optvar -> xlist = sllist_empty()), 
                  fprintf(stderr, "Erreur lors de la creation de la liste.\n"), 
                  F_ERROR)
  }

	return F_SUCCESS;
}


int load_words(options *optvar) {
	
	if (optvar -> xfileptr != NULL) {
    IF_ERROR(
    parse_file(optvar -> xfileptr, optvar, file_excluded, optvar -> nb_files), 
    F_ERROR)
	}

	for(size_t i = 0; i < optvar -> nb_files; i++) {
		if (i == 0) {
      IF_ERROR(parse_file(optvar -> fileptr[i], optvar, file_first, i), F_ERROR)
		} else {
      IF_ERROR(parse_file(optvar -> fileptr[i], optvar, file_regular, i), 
              F_ERROR)
		}
	}

	return F_SUCCESS;
}

int add_excluded_word(options *optvar, char *string) {
  char *s = malloc(strlen(string) + 1);
  
  IS_NULL_INST(s, fprintf(stderr, "Erreur lors de l'allocation dynamique.\n"), 
              F_ERROR)
  
	strcpy(s, string);

  size_t *counter = calloc((optvar -> nb_files) + 1, sizeof(size_t));

  IS_NULL_INST(counter, 
                fprintf(stderr, "Erreur lors de l'allocation de memoire.\n"), 
                F_ERROR)
	
	counter[optvar -> nb_files] = 1;

  IS_NULL_INST(sllist_insert_tail(optvar -> counters, counter), 
  fprintf(stderr, "Erreur lors de l'insertion d'un element dans la liste.\n"), 
  F_ERROR)
	
  IS_NULL_INST(sllist_insert_tail(optvar -> xlist, s), 
  fprintf(stderr, "Erreur lors de l'insertion d'un element dans la liste.\n"), 
  F_ERROR)
	
  IS_NULL_INST(hashtable_add(optvar -> hashtbl, s, counter), 
  fprintf(stderr, "Erreur lors de l'ajout d'un element dans la table de "
  "hachage.\n"), 
  F_ERROR)
  
  return F_SUCCESS;
}

int add_word(options *optvar, char *string, int idx) {
  size_t *c = (size_t *)hashtable_value(optvar -> hashtbl, string);
  
  if (c != NULL) {
    c[idx]++;
  } else {
    char *s = malloc(strlen(string) + 1);
    
    IS_NULL_INST(s, fprintf(stderr, "Erreur lors de l'allocation dynamique.\n"), 
                  F_ERROR)
	
		strcpy(s, string);

    IS_NULL_INST(sllist_insert_tail(optvar -> words, s), 
    fprintf(stderr, "Erreur lors de l'insertion d'un element dans la liste.\n"), 
    F_ERROR)
	   			
		size_t *counter = calloc((optvar -> nb_files) + 1, sizeof(size_t));

    IS_NULL_INST(counter, 
                  fprintf(stderr, "Erreur lors de l'allocation de memoire.\n"), 
                  F_ERROR)
	
		counter[idx] = 1;

    IS_NULL_INST(sllist_insert_tail(optvar -> counters, counter), 
    fprintf(stderr, "Erreur lors de l'insertion d'un element dans la liste.\n"), 
    F_ERROR)
  
    IS_NULL_INST(hashtable_add(optvar -> hashtbl, s, counter), 
    fprintf(stderr, "Erreur lors de l'ajout d'un element dans la table de "
    "hachage.\n"), 
    F_ERROR) 
  }
  return F_SUCCESS;
}

// Transforme la chaine *s en majuscule
void stringToUpper(char *s)
{
	for (size_t i = 0; i < strlen(s); i++)
	{
		s[i] = toupper(s[i]);
	}
}

// Transforme la chaine *s en minuscule
void stringToLower(char *s)
{
  for (size_t i = 0; i < strlen(s); i++)
  {
    s[i] = tolower(s[i]);
  }
}
