appname := hello_window

CXX := g++
CXXFLAGS := -std=c++11

srcfiles := $(shell find . -name "*.cpp" -not -path "./exercise/*") 
objects  := $(patsubst %.cpp, %.o, $(srcfiles))
LDFLAGS :=
LDLIBS := -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr

all: $(appname)
	./$(appname)
	
$(appname): $(objects)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(appname) $(objects) $(LDLIBS)

depend: .depend

.depend: $(srcfiles)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	rm -f $(objects)

dist-clean: clean
	rm -f *~ .depend

ex1: 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o ./exercise/ex1 exercise/ex1.cpp $(LDLIBS)
	./exercise/ex1

ex2: 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o ./exercise/ex2 exercise/ex2.cpp $(LDLIBS)
	./exercise/ex2

ex3: 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o ./exercise/ex3 exercise/ex3.cpp $(LDLIBS)
	./exercise/ex3

ex-clean:
	rm ./exercise/ex1
	rm ./exercise/ex2
	rm ./exercise/ex3

include .depend