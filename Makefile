CXX := g++
CXXFLAGS := -std=c++17 -O2 -pthread

BIN := bin/mandelbrot
SRC := src/mandelbrot.cpp src/bitmap.cpp src/multithread.cpp 

all: $(BIN)


$(BIN): $(SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@

e0.bmp: $(BIN)
	./$(BIN) -2 1 -1.5 1.5 bin/e0.bmp

e1.bmp: $(BIN)
	./$(BIN) -0.5 0 0.3 1.2 e1.bmp

e2.bmp: $(BIN)
	./$(BIN) 0.3 0.4 0.6 0.7 bin/e2.bmp

e3.bmp: $(BIN)
	./$(BIN) -0.2 0.0 -1.0 -0.9 bin/e3.bmp

e4.bmp: $(BIN)
	./$(BIN) -0.05 -0.01 -1.01 -0.97 bin/e4.bmp

clean:
	rm -rf bin doc DoxygenOutput *.bmp

doc:
	doxygen Doxyfile