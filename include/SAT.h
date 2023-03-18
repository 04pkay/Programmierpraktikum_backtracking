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
    bool check_if_satisfied() const;
    bool check_if_still_satisfiable() const;
    u_long get_number_variables() const;
    u_long get_numberclauses() const;
private:
    std::vector<std::vector<std::tuple<int,bool,int>>> instanz;
     u_long variables;
     u_long clauses;
};

#endif //PROGRAMMIERPRAKTIKUM_SAT_H