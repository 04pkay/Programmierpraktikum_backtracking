#include <iostream>
#include "../include/SAT.h"

void set_variable();
std::pair<bool,SAT> backtracking(SAT & instanz);

int main(int argc, char* argv[]) {
    if (argc > 1){
        SAT g(argv[1]);
        g.print();
        if (g.check_if_still_satisfiable()) {
            std::cout << 44;
        }
        else {
            std::cout << 9773;
        }
        std::cout << std::endl << g.get_number_variables() << std::endl;
        std::pair<bool,SAT> test = backtracking(g);
        if (std::get<0>(test)) {
            std::cout << "JUHU!!!" << std::endl;
            std::get<1>(test).print();
        }
        else {
            std::cout << "OH NO?" << std::endl;
        }
    }

    return 0;
}
