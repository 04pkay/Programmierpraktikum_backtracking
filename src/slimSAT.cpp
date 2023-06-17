//
// Created by Pascal Kessler on 18.03.23.
//

#include <slimSAT.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

SAT::SAT(char const* filename) {
    num_assigned_variables = 0;
    variables = 0;
    clauses = 0;
    std::ifstream file(filename);       // open file
    if (not file) {
        throw std::runtime_error("Cannot open file.");
    }

    std::string line;
    std::getline(file, line);           // get first line of file
    std::stringstream ss(line);               // convert line to a string stream
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

    std::vector<int> clause;       // -variable for negated, variable for normal
    while (std::getline(file, line)) {
        std::stringstream pp(line);
        int temp;
        pp >> temp;
        while (pp) {
            if (temp != 0) {
                clause.push_back(temp);
            }
            else {        //here we are at the end of the clause and clear it for the next to come
                instance.push_back(clause);
                clause.clear();
            }
            pp >> temp;
        }
    }
}

void SAT::print() const{
    for (auto & clause : instance) {
        for (auto & literal : clause) {
                std::cout << literal << " ; ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int SAT::get_number_variables() const {
    return variables;
}

int SAT::get_number_clauses() const {
    return clauses;
}

void SAT::delete_clause(const int & clause) {
    clauses -= 1;
    instance.erase(instance.begin() +clause);
}

void SAT::delete_literal(const int & clause, const int & variable) {
    auto NoMoreLiteral = std::remove_if(begin(instance[clause]), end(instance[clause]),[variable](int literal) { return (literal == variable or -literal == variable);});
    instance[clause].erase(NoMoreLiteral,instance[clause].end());
}

std::vector<std::vector<int>>& SAT::get_clauses() {
    return instance;
}

void SAT::set_variable_false(const int & variable) {
    num_assigned_variables += 1;
    for (int clause = 0; clause < instance.size(); clause++) {  //we set the variable to false
        bool satisfied = false;
        bool occurred = false;
        for (const int & literal : instance[clause]) {
            if (literal == variable or -literal == variable) {
                occurred = true;    //the variable occurs in this clause...
                if (literal < 0) {
                    satisfied = true;    //...and also satisfies the clause
                }
            }
        }
        if (occurred and satisfied) {
            delete_clause(clause);
            clause -= 1;
        }
        else if (occurred) {
            delete_literal(clause, variable);
        }
    }
}

void SAT::set_variable_true(const int & variable) {
    num_assigned_variables += 1;
    for (int clause = 0; clause < instance.size(); clause++) {  //we set the variable to true
        bool satisfied = false;
        bool occurred = false;
        for (const int & literal : instance[clause]) {
            if (literal == variable or -literal == variable) {
                occurred = true;    //the variable occurs in this clause...
                if (literal > 0) {
                    satisfied = true;    //...and also satisfies the clause
                }
            }
        }
        if (occurred and satisfied) {
            delete_clause(clause);
            clause -= 1;
        }
        else if (occurred) {
            delete_literal(clause, variable);
        }
    }
}

int SAT::get_number_assigned_variables() const {
    return num_assigned_variables;
}

std::vector<std::vector<int>>::iterator SAT::erase_clause(const std::vector<std::vector<int>>::iterator & clause_iterator) {
    clauses -= 1;
    return instance.erase(clause_iterator);
}
