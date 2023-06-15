//
// Created by Pascal Kessler on 18.03.23.
//

#ifndef PROGRAMMIERPRAKTIKUM_SLIMSAT_H
#define PROGRAMMIERPRAKTIKUM_SLIMSAT_H

#include <iostream>
#include <vector>

class SAT {
public:
    explicit SAT(char const* filename);
    void print() const;
    void set_variable_false(int variable);
    void set_variable_true(int variable);
    const int get_number_variables() const;
    const int get_number_assigned_variables() const;
    const int get_number_clauses() const;
    void delete_clause(int clause);
    void delete_literal(int clause, int variable);
    std::vector<std::vector<int>>::iterator erase_clause(std::vector<std::vector<int>>::iterator clause_iterator);
    std::vector<std::vector<int>>& get_clauses();
private:
     std::vector<std::vector<int>> instance;
     int variables;
     int clauses;
     int num_assigned_variables;
};

#endif //PROGRAMMIERPRAKTIKUM_SLIMSAT_H