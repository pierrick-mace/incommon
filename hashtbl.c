#include <stdint.h> // SIZE_MAX, uintmax_t
#include <stdio.h>  // fprintf, stderr
#include "hashtbl.h"

#define HT_LDFACT_MAX  0.75
#define HT_NSLOTS_MIN  1
#define HT_RESIZE_MUL  2
#define HT_RESIZE_ADD  0

#define ARRAYALLOC(base, nelem)                 \
  if ((nelem) > SIZE_MAX / sizeof(*(base))) { \
    base = NULL;                              \
  } else {                                    \
    base = malloc(nelem * sizeof(*(base)));   \
  }

typedef struct clist clist;

struct clist {
  const void *key;
  const void *value;
  size_t hkey;
  clist *next;
};

struct hashtable{
  size_t (*fun)(const void *);
  int (*compar)(const void *, const void *);
  clist **array;
  size_t nslots;
  size_t nentries;
};

hashtable *hashtable_empty(size_t (*hashfun)(const void *),
    int (*compar)(const void *, const void *)) {
  hashtable *ht = malloc(sizeof *ht);
  if (ht == NULL) {
    return NULL;
  }
  ht -> fun = hashfun;
  ht -> compar = compar;
  ARRAYALLOC(ht -> array, HT_NSLOTS_MIN);
  if (ht -> array == NULL) {
    free(ht);
    return NULL;
  }
  for (size_t k = 0; k < HT_NSLOTS_MIN; ++k) {
    ht -> array[k] = NULL;
  }
  ht -> nslots = HT_NSLOTS_MIN;
  ht -> nentries = 0;
  return ht;
}

void hashtable_dispose(hashtable **ptrht) {
  for (size_t k = 0; k < (*ptrht) -> nslots; ++k) {
    clist *p = (*ptrht) -> array[k];
    while (p != NULL) {
      clist *t = p;
      p = p -> next;
      free(t);
    }
  }
  free((*ptrht) -> array);
  free(*ptrht);
  *ptrht = NULL;
}

typedef struct htsearch htsearch;

struct htsearch {
  size_t hkey;
  clist **pcurr;
};

static inline htsearch ht_search(hashtable *ht, const void *key) {
  size_t hkey = ht -> fun(key);
  clist **pp = &(ht -> array[hkey % ht -> nslots]);
  while (*pp != NULL && ht -> compar((*pp) -> key, key) != 0) {
    pp = &((*pp) -> next);
  }
  return (htsearch) {
    .hkey = hkey,
    .pcurr = pp
  };
}

const void *hashtable_value(hashtable *ht, const void *key) {
  htsearch hts = ht_search(ht, key);
  return *hts.pcurr == NULL ? NULL : (*hts.pcurr) -> value;
}

const void *hashtable_remove(hashtable *ht, const void *key) {
  htsearch hts = ht_search(ht, key);
  if (*hts.pcurr == NULL) {
    return NULL;
  }
  clist *t = *hts.pcurr;
  const void *value = t -> value;
  *hts.pcurr = t -> next;
  free(t);
  ht -> nentries -= 1;
  return value;
}

static int ht_resize(hashtable *ht, size_t nslots) {
  clist **a;
  ARRAYALLOC(a, nslots);
  if (a == NULL) {
    return 1;
  }
  for (size_t k = 0; k < nslots; ++k) {
    a[k] = NULL;
  }
  for (size_t k = 0; k < ht -> nslots; ++k) {
    clist *p = ht -> array[k];
    while (p != NULL) {
      clist **ps = &(a[p -> hkey % nslots]);
      clist *t = p;
      p = t -> next;
      t -> next = *ps;
      *ps = t;
    }
  }
  free(ht -> array);
  ht -> array = a;
  ht -> nslots = nslots;
  return 0;
}

const void *hashtable_add(hashtable *ht, const void *key, const void *value) {
  if (value == NULL) {
    return NULL;
  }
  htsearch hts = ht_search(ht, key);
  if (*hts.pcurr != NULL) {
    (*hts.pcurr) -> value = value;
    return value;
  }
  if (ht -> nentries + 1 > HT_LDFACT_MAX * ht -> nslots) {
    if (ht -> nslots >= (SIZE_MAX - HT_RESIZE_ADD) / HT_RESIZE_MUL
        || ht_resize(ht, ht -> nslots * HT_RESIZE_MUL + HT_RESIZE_ADD)) {
      return NULL;
    }
    hts = ht_search(ht, key);
  }
  *hts.pcurr = malloc(sizeof **hts.pcurr);
  if (*hts.pcurr == NULL) {
    return NULL;
  }
  **hts.pcurr = (clist) {
    .key = key,
    .value = value,
    .hkey = hts.hkey,
    .next = NULL,
  };
  ht -> nentries += 1;
  return value;
}

void hashtable_printhealth(hashtable *ht) {
  size_t maxlen = 0;
  uintmax_t spos = 0;
  for (size_t k = 0; k < ht -> nslots; ++k) {
    size_t len = 0;
    clist *p = ht -> array[k];
    while (p != NULL) {
      ++len;
      spos += len;
      p = p -> next;
    }
    if (len > maxlen) {
      maxlen = len;
    }
  }
  fprintf(stderr,
      "--- hashtable health\n"
      "%12s\t%zu\n"
      "%12s\t%zu\n"
      "%12s\t%lf\n"
      "%12s\t%lf\n"
      "%12s\t%zu\n"
      "%12s\t%lf\n"
      "%12s\t%lf\n",
      "nslots",     ht -> nslots,
      "nentries",   ht -> nentries,
      "LDFACT_MAX", (double) HT_LDFACT_MAX,
      "ldfact",     (double) ht -> nentries / ht -> nslots,
      "max len",    maxlen,
      "theo pos",   1.0 + (ht -> nentries - 1.0) / (2.0 * ht -> nslots),
      "curr pos",   (double) spos / ht -> nentries);
}
