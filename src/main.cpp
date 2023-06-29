#include <iostream>
#include <slimSAT.h>
#include <CSAT.h>   //change to ASAT.h if wanted

int main(int argc, char* argv[]) {
    if (argc > 1){
        SAT g(argv[1]);
        //g.print();

        if (c_sat(g)) { //change to a_sat(g) if wanted
            std::cout << std::endl << "SATISFIABLE" << std::endl;
        }
        else {
            std::cout << std::endl << "NOT SATISFIABLE" << std::endl;
        }
    }
    else {
        std::cout << "Please provide a file with an instance" << std::endl;
    }
    return 0;
}
