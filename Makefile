CXX = g++
CXXFLAGS = -std=c++11 -Wall -I. -I./CL
LDFLAGS = /usr/lib/x86_64-linux-gnu/libOpenCL.so.1

SRC = test.cpp
OBJS = $(SRC:.cpp=.o)
EXEC = main

all: $(EXEC)
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)