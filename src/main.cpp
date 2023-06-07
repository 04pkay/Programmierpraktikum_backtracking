#include <iostream>
#include "../include/SAT.h"
#include "../include/backtracking.h"
#include "../include/ASAT.h"
#include "../include/CSAT.h"

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
        /*std::pair<bool,SAT> test = backtracking(g);
        if (std::get<1>(test).check_if_satisfied()) {
            std::cout << "JUHU!!!" << std::endl;
            std::get<1>(test).print();
        }
        else {
            std::cout << "OH NO?" << std::endl;
        }*/
        if (c_sat(g)) {
            std::cout << std::endl << "YIPPI" << std::endl;
        }
        else {
            std::cout << std::endl << "NEE" << std::endl;
        }
    }

    return 0;
}
