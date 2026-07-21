grnsh: util/builtins.c util/cleanup.c util/parser.c src/grnsh.c
	gcc -std=gnu17 util/builtins.c util/cleanup.c util/parser.c src/setup.c src/grnsh.c -D_POSIX_C_SOURCE=200809L -o grnsh
clean: grnsh
	rm grnsh
grnsh-w: util/builtins.c util/cleanup.c util/parser.c src/grnsh.c
	gcc -Wall -g -std=gnu17 util/builtins.c util/cleanup.c util/parser.c src/setup.c src/grnsh.c -D_POSIX_C_SOURCE=200809L -o grnsh