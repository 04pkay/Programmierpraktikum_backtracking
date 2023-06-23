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
    bool set_variable_false(const int & variable);
    bool set_variable_true(const int & variable);
    int get_number_variables() const;
    int get_number_assigned_variables() const;
    int& get_number_clauses();
    void delete_clause(const int & clause);
    void delete_literal(const int & clause,const int & variable);
    std::vector<std::vector<int>>::iterator erase_clause(const std::vector<std::vector<int>>::iterator & clause_iterator);
    std::vector<std::vector<int>>& get_clauses();
private:
     std::vector<std::vector<int>> instance;
     int variables;
     int clauses;
     int num_assigned_variables;
};

#endif //PROGRAMMIERPRAKTIKUM_SLIMSAT_H