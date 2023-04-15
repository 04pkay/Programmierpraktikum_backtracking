//
// Created by Pascal Kessler on 18.03.23.
//

#include "../include/SAT.h"
#include <iostream>
#include <vector>

bool backtracking_recursion(int temp, SAT & instanz) {
    instanz.set_variable(temp, true);
    if (instanz.check_if_satisfied()) {
        return true;
    }
    if (instanz.check_if_still_satisfiable()) {     //if the instanz is still satisfiable, we can set the next variable
        if (backtracking_recursion(temp + 1, instanz)) {
            return true;
        }
    }

    instanz.set_variable(temp, false);      //we came back because we made a mistake, maybe it will work with the variable set to false
    if (instanz.check_if_satisfied()) {
        return true;
    }
    if (instanz.check_if_still_satisfiable()) {         //if the instanz is still satisfiable, we can set the next variable
        if (backtracking_recursion(temp + 1, instanz)) {
            return true;
        }
    }

    instanz.reset_variable(temp);       //this variable was not the mistake, we need to backtrack some more
    return false;       //we tried every combination, this instanz is not satisfiable
}

/*std::pair<bool,SAT> backtracking(SAT & instanz) {
    backtracking_recursion(1, instanz);
    if (instanz.check_if_satisfied()) {
        return std::make_pair (true,instanz);
    }
    else {
        return std::make_pair (false,instanz);
    }
}*/

bool backtracking_non_recursive(SAT & instanz) {
    std::vector<int> VariableState;     //VariableState[i] := variable i+1 is not set (0), set to true (1), or false (-1)
    for (int i=0; i<instanz.get_number_variables(); i++) {
        VariableState.push_back(0);
    }
    int temp = 1;
    while (temp > 0) {
        if (VariableState[temp - 1] == 0) {     //first we set the variable to true
            instanz.set_variable(temp, true);
            if (instanz.check_if_satisfied()) {
                return true;
            }
            if (instanz.check_if_still_satisfiable()) {     //if the instanz is still satisfiable we go back to the start for the next variable
                temp++;
            } else {
                instanz.set_variable(temp, false);      //otherwise we try setting the variable to false
                if (instanz.check_if_satisfied()) {
                    return true;
                }
                if (instanz.check_if_still_satisfiable()) { //if the instanz is still satisfiable we go back to the start for the next variable
                    temp++;
                } else {        //we hit a dead end, reset the variable and go back one step
                    instanz.reset_variable(temp);
                    temp--;
                }
            }
        }
        else {
            if (VariableState[temp - 1] == 1) {     //we already tried setting this variable to true, but it didn't work, lets try setting it to false
                instanz.set_variable(temp, false);
                if (instanz.check_if_satisfied()) {
                    return true;
                }
                if (instanz.check_if_still_satisfiable()) {
                    temp++;
                } else {    //it didn't work, we reset the variable and go back a step
                    instanz.reset_variable(temp);
                    temp--;
                }
            }
            else {      //we already tried both with this variable, we need to go back some more
                instanz.reset_variable(temp);
                temp--;
            }
        }
    }
    return false; //nothing worked, the instanz is not satisfiable
}
