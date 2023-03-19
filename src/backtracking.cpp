//
// Created by Pascal Kessler on 18.03.23.
//

#include "../include/SAT.h"
#include <iostream>
#include <vector>

bool backtracking_recursion(int temp, SAT & instanz, std::vector<int> & VariableState) {
    VariableState[temp - 1] = 1;
    instanz.set_variable(temp, true);
    if (instanz.check_if_satisfied()) {
        return true;
    }
    if (backtracking_recursion(temp+1, instanz, VariableState) and temp < instanz.get_number_variables()) {
        return true;
    }
    VariableState[temp - 1] = -1;
    instanz.set_variable(temp, false);
    if (instanz.check_if_satisfied()) {
        return true;
    }
    if (backtracking_recursion(temp+1, instanz, VariableState) and temp < instanz.get_number_variables()) {
        return true;
    }
    VariableState[temp - 1] = 0;
    instanz.reset_variable(temp);
    return false;
}

std::pair<bool,SAT> backtracking(SAT & instanz) {
    u_long NumVariables = instanz.get_number_variables();
    std::vector<int> VariableState;
    for (int i = NumVariables; i>0; i -= 1) {
        VariableState.push_back(0);
    }
    backtracking_recursion(1, instanz, VariableState);
    if (instanz.check_if_satisfied()) {
        return std::make_pair (true,instanz);
    }
    else {
        return std::make_pair (false,instanz);
    }
}
