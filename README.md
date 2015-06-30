# gbcEmulator

## Build and run native
make && ./build/gbcEmulator.out

## Build and run for web
make target=out.js CXX=em++ && cd build && python2 -m SimpleHTTPServer 8080& 
Now go to localhost:8080/out.html

**Remember to make clean between two target as the .o are not in the same format.**
