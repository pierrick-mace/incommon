#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "options.h"
#include "file.h"
#include "sllist.h"
#include "hashtbl.h"
#include "print_words.h"

size_t str_hashfun(const char *s);

int main(int argc, char *argv[]) {

    options *optvar;
    bool error = false;
    
    if ((optvar = parse_arguments(argc, argv)) == NULL) {
        fprintf(stderr, "Erreur lors de la lecture de la ligne de commande\n");
        ERROR;
    }
    
    optvar -> hashfun = str_hashfun;
    
    if (create_hashtable(optvar) == F_ERROR) {
        ERROR;
    }

    if (create_lists(optvar) == F_ERROR) {
        ERROR;
    }

    if (open_files(optvar) == F_ERROR) {
        ERROR;
    }

    if (load_words(optvar) == F_ERROR) {
        ERROR;
    }
    
    if (optvar -> sort) {
        if(sllist_sort(optvar -> words, 
                *(int (*)(const void *, const void *))strcmp) == NULL) {
            ERROR;
        }
    }
    
    print_words(optvar);
    
    error:
    
    clean_memory(optvar);
    
    if (error) {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

size_t str_hashfun(const char *s) {
    size_t h = 0;
    
    for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
        h = 37 * h + *p;
    }
    
    return h;
}
