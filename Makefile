all: main.c pathfinding.c list.c
	gcc -c -g map2d_walls.c -o map2d_walls.o
	gcc -c -g main.c -o main.o
	gcc -c -g pathfinding.c -o pathfinding.o
	gcc -c -g list.c -o list.o
	gcc -c -g map2d.c -o map2d.o
	gcc -lm -g main.o pathfinding.o list.o map2d.o map2d_walls.o -o a.out
