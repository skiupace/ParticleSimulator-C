all:
	gcc -o bin/particle_simulator src/main.c -lSDL2 -lGLEW -lGL -lm && ./bin/particle_simulator