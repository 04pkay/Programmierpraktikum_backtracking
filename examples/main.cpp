#include <iostream>
#include "../include/SAT.h"

std::pair<bool,SAT> backtracking(SAT & instanz);
bool backtracking_non_recursive(SAT & instanz);

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
        if (std::get<1>(test).check_if_satisfied()) {
            std::cout << "JUHU!!!" << std::endl;
            std::get<1>(test).print();
        }
        else {
            std::cout << "OH NO?" << std::endl;
        }
        /*if (backtracking_non_recursive(g)) {
            std::cout << std::endl << "YIPPI SCHNIPPI" << std::endl;
        }
        else {
            std::cout << std::endl << "IS NICH DRIN MA SCHNIPPI" << std::endl;
        }*/
    }

    return 0;
}
