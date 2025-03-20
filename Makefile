OBJS	= main.cc
SOURCE	= main.o
HEADER	=
OUT	= ButtonCheck
CC	 = c++
FLAGS	 = -Wall -Weffc++ -std=c++17 -O3 -Iinclude

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cc
	$(CC) $(FLAGS) main.cc

clean:
	rm -f $(SOURCE) $(OUT)
	rm -f a.out