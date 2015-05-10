gcc -c -g main.c -o main.o && gcc -c -g pathfinding.c -o pathfinding.o && gcc -c -g list.c -o list.o && gcc -lm -g main.o pathfinding.o list.o -o a.out
