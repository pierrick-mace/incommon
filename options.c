#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <strings.h>
#include <string.h>
#include "options.h"

void print_help(void) {
  printf("Usage: incommon [OPTIONS] Fichiers\n");
  printf("Options:\n");
  printf("-x FICHIER, --EXCLUDE=FICHIER Exclure les mots qui figurent dans le fichier FICHIER\n");
  printf("-c TYPE, --case=TYPE considerer les mots apres la transformation "
  			"TYPE, de valeur upper, en majuscules, ou lower, en minuscules\n");
  printf("-s MOTIF, --separators=MOTIF considerer que les caracteres decrits "
		  "par MOTIF sont les separateurs des mots.\n\tMOTIF est une suite non vide de "
		  "caracteres choisis parmi:\n\ta (lettres)\n\tc (controle)\n\td (chiffres)\n"
			"\tn (fin de ligne)\n\tp (ponctuation)\n\ts (espaces)\n");
  printf("-S, --sort trier les mots dans l'ordre croissant\n");

  exit(EXIT_SUCCESS);
}

options *initialize_options(void) {
	options *opt = malloc(sizeof(options));
	if(opt == NULL) {
		fprintf(stderr, "Erreur a l'allocation\n");
		exit(EXIT_FAILURE);
	}
  
	opt -> sort = false;
	opt -> type = NIL;
	opt -> separator = NULL;
  	opt -> files = NULL;
	opt -> xfile = NULL;
	opt -> fileptr = NULL;
	opt -> xfileptr = NULL;
	opt -> nb_files = 0;
	opt -> words = NULL;
	opt -> counters = NULL;
	opt -> xlist = NULL;

	return opt;
}

options *parse_arguments(int argc, char *argv[]) {
	int c,
		option_index = 0;

	options *optvar = initialize_options();
	
	static struct option long_options[] = {
		{"exclude", required_argument, 0, option_exclude},
		{"case", required_argument, 0, option_case},
		{"separators", required_argument, 0, option_separator},
		{"sort", no_argument, 0, option_sort},
		{"help", no_argument, 0, option_help},
		{0, 0, 0, 0}
	};

	char separators_array[] = {'a', 'c', 'd', 'n', 'p', 's'}; 

	while ((c = getopt_long(argc, argv, "x:c:s:Sh", long_options, &option_index)) != -1) {
		switch(c) {
			case 'x':
			case option_exclude:
				optvar -> xfile = optarg;
				break;
			case 'c':
			case option_case:
				if (strcasecmp(optarg, "upper") == 0) {
					optvar -> type = UPPER;
				} else if (strcasecmp(optarg, "lower") == 0) {
					optvar -> type = LOWER;
				} else {
					fprintf(stderr, "Erreur: l'argument de l'option -c est invalide.\n\n");
		  			free(optvar -> separator);
		  			free(optvar);
					print_help();
				}
				break;
			case 's':
			case option_separator:
				optvar -> separator = calloc(strlen(optarg) + 1, sizeof(char));
				
				if(optvar -> separator == NULL) {
				  fprintf(stderr, "Erreur lors de la reallocation de memoire.\n");
				  free(optvar);
				  exit(EXIT_FAILURE);
				}

				strcpy(optvar -> separator, optarg);
				break;
			case 'S':
			case option_sort:
				optvar -> sort = true;
				break;
			case 'h':
			case option_help:
			case '?':
				free(optvar);
				free(optvar -> separator);
				print_help();
		}	
	}
  
	bool is_valid_arg = true;

	if(optvar -> separator != NULL) {
		size_t k = 0;
		
		while(optvar -> separator[k] != '\0') { 
		  	bool is_in_array = false;
		  	
		  	for(size_t i = 0; i < (sizeof(separators_array) / sizeof(char)); i++) {
				if(optvar -> separator[k] == separators_array[i]) {
			  		is_in_array = true;
				}
		  	}	
		
			if (!is_in_array) {
				is_valid_arg = false;
		  	}     
		 	
		 	k++;
		}
	} else {
		optvar -> separator = malloc(2 * sizeof(char));
	
		if(optvar -> separator == NULL) {
	  		fprintf(stderr, "Erreur lors de l'allocation de memoire.\n");
	  		free(optvar);
	  		exit(EXIT_FAILURE);
		}
		
		strcpy(optvar -> separator, "s");
  	}

	if (!is_valid_arg) {
		printf("Erreur: l'argument de l'option -s est invalide.\n\n");
		free(optvar -> separator);
		free(optvar);
		print_help();
	}
  
  	IS_NULL_INST((optvar -> files = calloc(argc - optind, sizeof(char *))), 
  					free(optvar), NULL)
  
	IS_NULL_INST((optvar -> fileptr = calloc(argc - optind, sizeof(FILE *))), 
  					free(optvar->files);free(optvar), NULL)
  
	size_t i = 0;

	while(optind < argc) {
		optvar -> files[i] = argv[optind];
		optvar -> nb_files++;
		i++;
		optind++;
	}	
  
	if (optvar -> nb_files < 2) {
		fprintf(stderr, "Erreur: deux fichiers sont requis au minimum.\n\n");
		free(optvar -> files);
		free(optvar -> fileptr);
		free(optvar -> separator);
		free(optvar);
		print_help();
	}

	return optvar;
}

void clean_memory(options *optvar) {

	if (optvar != NULL) {
		
		for (size_t i = 0; i < optvar -> nb_files; i++) {
			if(optvar -> fileptr[i] != NULL) {
				fclose(optvar -> fileptr[i]);
		  	}
		}
	
		if (optvar -> xfileptr != NULL) {
	  		fclose(optvar -> xfileptr);
	  
	  		if (optvar -> xlist != NULL) {
				sllist_apply(optvar -> xlist, free);
				sllist_dispose(&(optvar -> xlist));
	  		}
		}
	
		if (optvar -> words != NULL) {
			sllist_apply(optvar -> words, free);
			sllist_dispose(&(optvar -> words));
		}
	
		if (optvar -> counters != NULL) {
			sllist_apply(optvar -> counters, free);
			sllist_dispose(&(optvar -> counters));
		}
	
		if (optvar -> hashtbl != NULL) {
			hashtable_dispose(&(optvar -> hashtbl));
		}
	
		if (optvar -> files != NULL) {
			free(optvar -> files);
		}
	
		if (optvar -> fileptr != NULL) {
			free(optvar -> fileptr);  
		}
	
		if(optvar -> separator != NULL) {
			free(optvar -> separator);	
		}
	
		free(optvar);
  	}
}
