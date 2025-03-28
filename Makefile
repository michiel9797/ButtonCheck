OBJS	= settings.cc main.cc
SOURCE	= settings.o main.o
HEADER	=
OUT	= ButtonCheck
CC	= c++
FLAGS	= -Wall -Weffc++ -std=c++17 -O3 -Iinclude

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

settings.o: settings.cc
	$(CC) $(FLAGS) settings.cc

main.o: main.cc
	$(CC) $(FLAGS) main.cc

clean:
	rm -f $(SOURCE) $(OUT)
	rm -f a.out
	rm -f settings