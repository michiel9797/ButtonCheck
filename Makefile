OBJS		= settings.o benchmark.o main.o
SOURCE		= settings.cc benchmark.cc main.cc
OUT		= ButtonCheck
CC		= c++
FLAGS		= -Wall -Weffc++ -std=c++17 -O2 -Iinclude
LDLFLAGS 	= libtuntap/build/lib/libtuntap++.a \
		  libtuntap/build/lib/libtuntap.a
INCLUDES	= -Ilibtuntap/build


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LDLFLAGS)

%.o: %.cc
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUT)
	rm -f a.out
	rm -f settings