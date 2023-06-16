#include <iostream>
#include <slimSAT.h>
#include <CSAT.h>

int main(int argc, char* argv[]) {
    if (argc > 1){
        SAT g(argv[1]);
        g.print();
        if (c_sat(g)) {
            std::cout << std::endl << "YIPPI" << std::endl;
        }
        else {
            std::cout << std::endl << "NEE" << std::endl;
        }
    }

    return 0;
}
