CXX = clang++
CXXWEB = em++
CXXFLAGS = -g -Wall
LDFLAGS = -lSDL

src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)
target = gbcEmulator.out
outDir = ./build
outObj = $(addprefix $(outDir)/,$(obj))




$(target): $(outObj)
	$(CXX) -o ./build/$@ $^ $(LDFLAGS)

$(outDir)/%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(outDir)/*.o

web:
	make clean && make target=out.js CXX=em++ && python2 -m SimpleHTTPServer 8080&
