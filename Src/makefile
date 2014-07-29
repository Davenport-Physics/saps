OBJS = main.o particles.o constants.o newtonian.o elements.o electric.o systemtime.o engine.o mechanics.o
CC = gcc
DEBUG = -g
CFLAGS = -Wall -O3 -c -mtune=native -march=native $(DEBUG)
LFLAGS = -lpthread -lGL -lSDL2 -lm -D_GNU_SOURCE $(DEBUG)

physics : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o physics

main.o : main.c particles.h constants.h systemtime.h engine.h
	$(CC) $(CFLAGS) main.c

particles.o : particles.h particles.c constants.h newtonian.h electric.h systemtime.h mechanics.h
	$(CC) $(CFLAGS) particles.c

constants.o : constants.h constants.c
	$(CC) $(CFLAGS) constants.c

newtonian.o : newtonian.h newtonian.c constants.h
	$(CC) $(CFLAGS) newtonian.c

elements.o : elements.h elements.c
	$(CC) $(CFLAGS) elements.c

electric.o : electric.h electric.c constants.h
	$(CC) $(CFLAGS) electric.c

systemtime.o : systemtime.h systemtime.c
	$(CC) $(CFLAGS) systemtime.c

engine.o : engine.h engine.c particles.h systemtime.h
	$(CC) $(CFLAGS) engine.c

mechanics.o : mechanics.h mechanics.c
	$(CC) $(CFLAGS) mechanics.c

clean :
	\rm *.o physics
