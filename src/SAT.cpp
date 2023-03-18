//
// Created by Pascal Kessler on 18.03.23.
//

#include "../include/SAT.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

SAT::SAT(char const* filename) {
    std::ifstream file(filename);                             // open file
    if (not file) {
        throw std::runtime_error("Cannot open file.");
    }

    std::string line;
    std::getline(file, line);           // get first line of file
    std::stringstream ss(line);               // convert line to a stringstream
    char FirstLetter;
    ss >> FirstLetter;
    while (FirstLetter == 'c') {
        std::getline(file, line);
        std::stringstream bb(line);
        bb >> FirstLetter;
    }

    std::stringstream tt(line);
    tt >> FirstLetter;
    std::string InitialLineType;
    tt >> InitialLineType;
    if (FirstLetter != 'p' or InitialLineType != "cnf") {
        throw std::runtime_error("Wrong format.");
    }
    tt >> variables >> clauses;

    std::vector<std::tuple<int,bool,int>> clause;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        while (ss) {
            int temp;
            ss >> temp;
            if (temp < 0) {
                const std::tuple<int,bool,int> Literal = std::make_tuple(-temp,false,0);
                clause.push_back(Literal);
            }
            if (temp > 0) {
                const std::tuple<int,bool,int> Literal = std::make_tuple(temp,true,0);
                clause.push_back(Literal);
            }
            else {
                instanz.push_back(clause);
                clause.clear();
            }
        }
    }
}

void SAT::print() const{
    for (auto & clause : instanz) {
        for (auto & tuple : clause) {
            std::cout << std::get<0>(tuple) << "," << std::get<2>(tuple) << " ; ";
        }
    }
    std::cout << std::endl;
}

void SAT::set_variable(int variable, bool setting) {

}

bool SAT::check_if_satisfied() const{
    for (auto & clause : instanz) {
        bool satisfied = false;
        for (auto & tuple : clause) {
            if (std::get<2>(tuple) == 1) {
                satisfied = true;
            }
        }
        if (not satisfied) {
            return false;
        }
    }
    return true;
}

u_long SAT::get_number_variables() const {
    return variables;
}

u_long SAT::get_numberclauses() const {
    return clauses;
}

bool SAT::check_if_still_satisfiable() const {
    for (auto & clause : instanz) {
        bool satisfiable = false;
        for (auto & tuple : clause) {
            if (std::get<2>(tuple) != -1) {
                satisfiable = true;
            }
        }
        if (not satisfiable) {
            return false;
        }
    }
};