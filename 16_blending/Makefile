appname := $(shell basename `pwd`).out

CXX := g++
CXXFLAGS := -std=c++11

srcfiles := $(shell find . -name "*.cpp" -not -path "./exercise/*") 
objects  := $(patsubst %.cpp, %.o, $(srcfiles))
LDFLAGS := -L../lib/
LDLIBS := -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr -lassimp
INC := -I../include/

all: $(appname)
	$(shell LD_LIBRARY_PATH=../lib)
	export LD_LIBRARY_PATH=../lib/ ; ./$(appname) || true
	
$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(objects) $(LDFLAGS) $(LDLIBS) $(INC)

%.o: %.cpp
	$(CXX) -c  $(CXXFLAGS) $(INC) $< -o $@

depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) $(INC) -MM $^>>./.depend;

clean:
	rm -f $(objects) || true
	rm -f $(appname) || true
#in case we changed the base directory name - clean also all .out files
	rm -f *.out || true

dist-clean: clean
	rm -f *~ .depend

ex1: 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o ./exercise/ex1 exercise/ex1.cpp $(LDLIBS) $(INC)
	./exercise/ex1

ex2: 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o ./exercise/ex2 exercise/ex2.cpp $(LDLIBS) $(INC)
	./exercise/ex2

ex3: 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o ./exercise/ex3 exercise/ex3.cpp $(LDLIBS) $(INC)
	./exercise/ex3

ex4: 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o ./exercise/ex4 exercise/ex4.cpp $(LDLIBS) $(INC)
	./exercise/ex4

ex-clean:
	rm ./exercise/ex1 || true
	rm ./exercise/ex2 || true
	rm ./exercise/ex3 || true
	rm ./exercise/ex4 || true

include .depend