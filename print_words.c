#include <stdio.h>
#include "sllist.h"
#include "print_words.h"
#include "hashtbl.h"

typedef struct csllist csllist;

struct csllist {
  void *value;
  csllist *next;
};

struct sllist {
  csllist *head;
  csllist *tail;
};

bool array_contains_no_zero(size_t *c, size_t size) {
  for(size_t i = 0; i < size; i++) {
      if(c[i] == 0) {
        return false;
      }
  }
  return true;
}

void print_occurence_line(const char *word, size_t *c, size_t size) {
  printf("%s", word);
  for (size_t k = 0; k < size; ++k) { 
    printf("\t%zu", c[k]);
  }
  printf("\n");
}

void print_file_names(options *optvar) {
  size_t i = 0;
  while(i < optvar -> nb_files) {
    printf("\t%s", optvar -> files[i]);
    i++;
  }
  printf("\n");
}

void print_words(options *optvar) {
  print_file_names(optvar);
  csllist *p = optvar -> words -> head;
  while (p != NULL) {
    size_t *c = (size_t*)hashtable_value(optvar -> hashtbl, p -> value);
    if (array_contains_no_zero(c, optvar -> nb_files)) {
      print_occurence_line(p -> value, c, optvar -> nb_files);
    }
    p = p -> next;
  }
}
