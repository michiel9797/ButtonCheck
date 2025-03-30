OBJS	= settings.cc benchmark.cc main.cc
SOURCE	= settings.o benchark.o main.o
HEADER	=
OUT	= ButtonCheck
CC	= c++
FLAGS	= -Wall -Weffc++ -std=c++17 -O2 -Iinclude

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

settings.o: settings.cc
	$(CC) $(FLAGS) settings.cc

benchmark.o: benchmark.cc
	$(CC) $(FLAGS) benchmark.cc

main.o: main.cc
	$(CC) $(FLAGS) main.cc

clean:
	rm -f $(SOURCE) $(OUT)
	rm -f a.out
	rm -f settings