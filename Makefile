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
	make clean && make target=out.js CXX=em++

serve:
	xdg-open http://localhost:8080/build/out2.html && python2 -m SimpleHTTPServer 8080

stopServe:
	killall python2

native:
	make clean && make && ./build/gbcEmulator.out
