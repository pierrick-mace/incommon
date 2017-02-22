CC = gcc
CFLAGS = -O2 -pedantic -Wall -Wextra -Werror -std=c11
objects = main.o hashtbl.o options.o sllist.o file.o print_words.o
executable = incommon

all: executable

clean:
	$(RM) $(objects) $(executable)

executable: $(objects) $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

file.o: file.c options.h sllist.h hashtbl.h file.h defines.h
hashtbl.o: hashtbl.c hashtbl.h
main.o: main.c options.h sllist.h hashtbl.h file.h print_words.h \
 defines.h
options.o: options.c options.h sllist.h hashtbl.h defines.h
print_words.o: print_words.c sllist.h print_words.h options.h hashtbl.h
sllist.o: sllist.c sllist.h
