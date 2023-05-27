//
// Created by Pascal Kessler on 18.03.23.
//

#ifndef PROGRAMMIERPRAKTIKUM_SAT_H
#define PROGRAMMIERPRAKTIKUM_SAT_H

#include <iostream>
#include <vector>

class SAT {
public:
    explicit SAT(char const* filename);
    void print() const;
    void set_variable(int variable, bool setting);
    void reset_variable(int variable);
    bool check_if_satisfied() const;
    bool check_if_still_satisfiable() const;
    unsigned long get_number_variables() const;
    unsigned long get_number_clauses() const;
    std::vector<std::pair<int,int>> variable_scan();
    void delete_clause(int clause);
    void delete_literal(int clause, int variable);
    std::vector<std::vector<std::tuple<int,bool,int>>> get_clauses();
private:
    std::vector<std::vector<std::tuple<int,bool,int>>> instance;
     unsigned long variables;
     unsigned long clauses;
};

#endif //PROGRAMMIERPRAKTIKUM_SAT_H
