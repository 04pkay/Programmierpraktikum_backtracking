#include <iostream>
#include "../include/SAT.h"

int main(int argc, char* argv[]) {
    if (argc > 1){
        SAT g(argv[1]);
        g.print();
        if (g.check_if_satisfied()) {
            std::cout << 44;
        }
        else {
            std::cout << 9773;
        }
        std::cout << std::endl << g.get_number_variables();
    }

    return 0;
}
