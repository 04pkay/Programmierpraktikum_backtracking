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
    void set_variable_false(int variable);
    void set_variable_true(int variable);
    void reset_variable(int variable);
    bool check_if_satisfied() const;
    bool check_if_still_satisfiable() const;
    unsigned long get_number_variables() const;
    unsigned long get_number_assigned_variables() const;
    unsigned long get_number_clauses() const;
    void delete_clause(int clause);
    void delete_literal(int clause, int variable);
    std::vector<std::vector<std::tuple<int,bool,int>>> get_clauses();
    std::vector<std::vector<std::tuple<int,bool,int>>>::iterator get_clause_iterator();
    std::vector<std::vector<std::tuple<int,bool,int>>>::iterator get_clause_iterator_end();
private:
    std::vector<std::vector<std::tuple<int,bool,int>>> instance;
     unsigned long variables;
     unsigned long clauses;
     unsigned long num_assigned_variables;
};

#endif //PROGRAMMIERPRAKTIKUM_SAT_H