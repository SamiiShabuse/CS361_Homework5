CXX := g++
CXXFLAGS := -std=c++17 -O2 -pthread

all: mandelbrot

mandelbrot: mandelbrot.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

e0.bmp: mandelbrot
	./mandelbrot -2 1 -1.5 1.5 e0.bmp

e1.bmp: mandelbrot
	./mandelbrot -0.5 0 0.3 1.2 e1.bmp

e2.bmp: mandelbrot
	./mandelbrot 0.3 0.4 0.6 0.7 e2.bmp

e3.bmp: mandelbrot
	./mandelbrot -0.2 0.0 -1.0 -0.9 e3.bmp

e4.bmp: mandelbrot
	./mandelbrot -0.05 -0.01 -1.01 -0.97 e4.bmp

clean:
	rm -f mandelbrot *.bmp
