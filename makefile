project : game.o menu.o
	cc game.o menu.o -lncurses -o project
game.o : game.c
	cc -c game.c
menu.o : menu.c
	cc -c menu.c
clean :
	rm -f *.o project
