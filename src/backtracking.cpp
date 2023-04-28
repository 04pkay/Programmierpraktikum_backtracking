//
// Created by Pascal Kessler on 18.03.23.
//

#include "../include/SAT.h"
#include <vector>

enum class VariableAssignment : int {assigned_false = -1, unassigned = 0, assigned_true = 1};

bool backtracking_recursion(int current_variable, SAT & instance) {
    instance.set_variable(current_variable, true);
    if (instance.check_if_satisfied()) {
        return true;
    }
    if (instance.check_if_still_satisfiable()) {     //if the instance is still satisfiable, we can set the next variable
        if (backtracking_recursion(current_variable + 1, instance)) {
            return true;
        }
    }

    instance.set_variable(current_variable, false);      //we came back because we made a mistake, maybe it will work with the variable set to false
    if (instance.check_if_satisfied()) {
        return true;
    }
    if (instance.check_if_still_satisfiable()) {         //if the instance is still satisfiable, we can set the next variable
        if (backtracking_recursion(current_variable + 1, instance)) {
            return true;
        }
    }

    instance.reset_variable(current_variable);       //this variable was not the mistake, we need to backtrack some more
    return false;       //we tried every combination, this instance is not satisfiable
}

std::pair<bool,SAT> backtracking(SAT & instance) {
    backtracking_recursion(1, instance);
    if (instance.check_if_satisfied()) {
        return std::make_pair (true,instance);
    }
    else {
        return std::make_pair (false,instance);
    }
}

bool backtracking_non_recursive(SAT & instance) {
    std::vector<VariableAssignment> VariableState(int(instance.get_number_variables()), VariableAssignment::unassigned);     //VariableState[i] := variable i+1 is not set (0), set to true (1), or false (-1)
    int current_variable = 1;
    while (current_variable > 0 and current_variable <= instance.get_number_variables()) {
        if (VariableState[current_variable - 1] == VariableAssignment::unassigned) {     //first we set the variable to true
            instance.set_variable(current_variable, true);
            VariableState[current_variable - 1] = VariableAssignment::assigned_true;
            if (instance.check_if_still_satisfiable()) {     //if the instance is still satisfiable we go back to the start for the next variable
                current_variable++;
            }
        }
        else if (VariableState[current_variable - 1] == VariableAssignment::assigned_true) { //we already tried setting this variable to true, but it didn't work, lets try setting it to false
                instance.set_variable(current_variable, false);
                VariableState[current_variable - 1] = VariableAssignment::assigned_false;
                if (instance.check_if_still_satisfiable()) {
                    current_variable++;
                }
        }
        else {      //we already tried both with this variable, we need to go back some more
            instance.reset_variable(current_variable);
            VariableState[current_variable - 1] = VariableAssignment::unassigned;
            current_variable--;
        }
    }
    return instance.check_if_satisfied(); //either nothing worked or we set every variable and are satisfied
}
