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
    while (FirstLetter == 'c') {        //skip comment lines
        std::getline(file, line);
        std::stringstream bb(line);
        bb >> FirstLetter;
    }

    std::stringstream tt(line);         //first line without 'c' in front
    tt >> FirstLetter;
    std::string InitialLineType;
    tt >> InitialLineType;
    if (FirstLetter != 'p' or InitialLineType != "cnf") {
        throw std::runtime_error("Wrong format.");
    }
    tt >> variables >> clauses;

    std::vector<std::tuple<int,bool,int>> clause;       //clause[0] := variable, clause[1] := not negated?, clause[2] := literal is not set(0) true(1) or false(-1)
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        int temp;
        ss >> temp;
        while (ss) {
            if (temp < 0) {     //negated literal
                const std::tuple<int,bool,int> Literal = std::make_tuple(-temp,false,0);
                clause.push_back(Literal);
            }
            if (temp > 0) {     //normal literal
                const std::tuple<int,bool,int> Literal = std::make_tuple(temp,true,0);
                clause.push_back(Literal);
            }
            if (temp == 0) {        //here we are at the end of the clause and clear it for the next to come
                instance.push_back(clause);
                clause.clear();
            }
            ss >> temp;
        }
    }
}

void SAT::print() const{
    for (auto & clause : instance) {
        for (auto & tuple : clause) {
            if (std::get<1>(tuple)) {
                std::cout << std::get<0>(tuple) << "," << std::get<2>(tuple) << " ; ";
            }
            else {
                std::cout << -std::get<0>(tuple) << "," << std::get<2>(tuple) << " ; ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void SAT::set_variable(int variable, bool setting) {
    for (auto & clause : instance) {
        for (auto & tuple : clause) {
            if (std::get<0>(tuple) == variable) {       //if we find a literal belonging to the variable..
                if (std::get<1>(tuple) == setting) {    //..we check if we should set it to true..
                    std::get<2>(tuple) = 1;
                }
                else {
                    std::get<2>(tuple) = -1;            //..or false
                }
            }
        }
    }
}

bool SAT::check_if_satisfied() const{
    for (auto & clause : instance) {
        bool satisfied = false;
        for (auto & tuple : clause) {
            if (std::get<2>(tuple) == 1) {      //if we find a literal set on true, the clause is satisfied
                satisfied = true;
            }
        }
        if (not satisfied) {
            return false;       //if one clause is not satisfied, the whole instance is not satisfied
        }
    }
    return true;
}

unsigned long SAT::get_number_variables() const {
    return variables;
}

unsigned long SAT::get_number_clauses() const {
    return clauses;
}

bool SAT::check_if_still_satisfiable() const {
    for (auto & clause : instance) {
        bool satisfiable = false;
        for (auto & tuple : clause) {
            if (std::get<2>(tuple) != -1) {     //if we find a literal not set on false, the clause can still be satisfied (or is satisfied already)
                satisfiable = true;
            }
        }
        if (not satisfiable) {
            return false;       //if one clause has all literals set on false we cannot still satisfy the clause
        }
    }
    return true;
}

void SAT::reset_variable(int variable) {
    for (auto & clause : instance) {
        for (auto & tuple : clause) {
            if (std::get<0>(tuple) == variable) {
                std::get<2>(tuple) = 0;
            }
        }
    }
}

/*std::vector<std::pair<int,int>> SAT::variable_scan() {
    std::vector<std::vector<std::pair<int,int>>> VariableLocations(int(get_number_variables()), std::vector<std::pair<int,int>>);
    int current_clause = 0;
    int current_tuple = 0;
    for (auto & clause : instance) {
        for (auto & tuple : clause) {
            VariableLocations
        }
        current_clause += 1;
        current_tuple = 0;
    }
}*/
void SAT::delete_clause(int clause) {
    clauses -= 1;
    instance.erase(instance.begin() +clause);
}

void SAT::delete_literal(int clause, int variable) {
    auto NoMoreLiteral = std::remove_if(begin(instance[clause-1]), end(instance[clause-1]),[variable](std::tuple<int,bool,int> tuple) { return (std::get<0>(tuple) == variable);});
    instance[clause-1].erase(NoMoreLiteral,instance[clause-1].end());
}

std::vector<std::vector<std::tuple<int, bool, int>>> SAT::get_clauses() {
    return instance;
}
