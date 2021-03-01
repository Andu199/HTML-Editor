build:
	gcc -Wall -Wextra def_types.h Stiva_Coada.h Stiva_Coada.c interpret.h interpret.c utils.h utils.c tema3.c -o tema3
clean:
	rm -rf *.o tema3