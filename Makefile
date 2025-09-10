CC=gcc
CFLAGS= -Wall
DEPS = sys.h

myshell: myshell.o redirection.o commandes.o arriere-plan.o
	$(CC) myshell.o redirection.o commandes.o arriere-plan.o -o myshell

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# pour forcer clean/cleanall même s'il existe un fichier de même nom
.PHONY: clean cleanall

clean:
	rm -rf *.o

cleanall: clean
	rm -rf $(EXEC)
