//
// Created by Pascal Kessler on 18.03.23.
//

#include "../include/SAT.h"
#include <iostream>
#include <vector>
#include <stack>

void set_variable(int temp, SAT & instanz, std::vector<int> & VariableState) {
    if (VariableState[temp-1] > 0) {
        VariableState[temp-1] = -1;
        instanz.set_variable(temp, false);
    }
    if (VariableState[temp-1] < 0) {
        VariableState[temp-1] = 0;
        instanz.reset_variable(temp);
    }
}

std::pair<bool,SAT> backtracking(SAT & instanz) {
    std::pair<bool,SAT> final = std::make_pair (false,instanz);
    u_long NumVariables = instanz.get_number_variables();
    std::stack<int> EachVariable;
    std::stack<int> BackTrack;
    std::vector<int> VariableState;
    for (int i = NumVariables; i>0; i -= 1) {
        EachVariable.push(i);
        VariableState.push_back(0);
    }
    int temp = EachVariable.top();
    EachVariable.pop();
    BackTrack.push(temp);
    VariableState[temp-1] = 1;
    instanz.set_variable(temp, true);
    if (not instanz.check_if_still_satisfiable()) {
        VariableState[temp - 1] = -1;
        instanz.set_variable(temp, false);
    }
    temp = EachVariable.top();
    EachVariable.pop();
    BackTrack.push(temp);
    while (VariableState[0] != 0) {
        if (instanz.check_if_satisfied()) {
            return std::make_pair(true, instanz);
        }
        if (instanz.check_if_still_satisfiable()) {
            VariableState[temp - 1] = 1;
            instanz.set_variable(temp, true);
            temp = EachVariable.top();
            EachVariable.pop();
            BackTrack.push(temp);
        }
        else {
            if (VariableState[temp - 1] > 0) {
                VariableState[temp - 1] = -1;
                instanz.set_variable(temp, false);
            }
            else {
                VariableState[temp-1] = 0;
                instanz.reset_variable(temp);
                BackTrack.pop();
                EachVariable.push(temp);
                try {temp = BackTrack.top();}
                catch (...) {
                    return final;
                }
            }
        }
    }
    return final;
}
