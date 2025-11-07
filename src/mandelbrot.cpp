#include <bits/stdc++.h>

struct Rectangle { long double x_min, x_max, y_min, y_max; };

static const int WIDTH = 1500;
int HEIGHT; // This will be determined by the number of points on the y axis.


int main(int argc, char** argv) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if(argc != 6) {
        std::cerr << "Usage: " << argv[0] << " x_min x_max y_min y_max max_iterations\n";
        return 1;
    }

    Rectangle rect;
    try{
        rect.x_min = std::stold(argv[1]);
        rect.x_max = std::stold(argv[2]);
        rect.y_min = std::stold(argv[3]);
        rect.y_max = std::stold(argv[4]);
    } catch(...) {
        std::cerr << "Error: Invalid rectangle coordinates.\n";
        return 1;
    }
    if(!(rect.x_min < rect.x_max && rect.y_min < rect.y_max)) {
        std::cerr << "Error: Invalid rectangle coordinates.\n";
        return 1;
    }
    
}