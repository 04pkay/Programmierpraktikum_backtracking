//
// Created by Pascal Kessler on 27.05.23.
//

#include <slimSAT.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>

int branching_rule(SAT & current_instance) {
    int HowShort = std::numeric_limits<int>::max();
    std::vector<int> VariableOccurrences(int(current_instance.get_number_variables()), 0);  //here we store how often variables occur in the shortest clauses

    for (auto & clause : current_instance.get_clauses()) {
        if (clause.size() < HowShort) {
            HowShort = clause.size();
            std::for_each(VariableOccurrences.begin(), VariableOccurrences.end(), [](int& n) {n=0;});
            for (auto & literal : clause) {
                VariableOccurrences[std::abs(literal)-1] += 1;
            }
        }
        else if (clause.size() == HowShort) {
            for (auto & literal : clause) {
                VariableOccurrences[std::abs(literal)-1] += 1;
            }
        }
    }
    int ChosenVariable = 0;
    for (int variable = 1; variable < VariableOccurrences.size(); variable++) {
        if (VariableOccurrences[ChosenVariable] < VariableOccurrences[variable]) {  //we determine the variable with the most occurrences
            ChosenVariable = variable;
        }
    }
    return ChosenVariable+1; //shift one up because how the variables are stored
}

bool a_sat(SAT & instance) {
    std::queue<SAT> DifferentPaths; //here we store the instances with different variable settings
    DifferentPaths.push(instance);
    while (not DifferentPaths.empty()) {
        SAT current_instance = std::move(DifferentPaths.front());
        DifferentPaths.pop();
        if (current_instance.get_number_clauses() == 0) {   //clauses get deleted, when satisfied
            return true;
        }
        else if (std::none_of(current_instance.get_clauses().begin(), current_instance.get_clauses().end(),[](const std::vector<int> &clause) { return clause.empty(); })) {
            int ChosenVariable = branching_rule(current_instance);
            SAT ClonedInstance = current_instance;
            ClonedInstance.set_variable_false(ChosenVariable);
            DifferentPaths.push(ClonedInstance);
            current_instance.set_variable_true(ChosenVariable);
            DifferentPaths.emplace(std::move(current_instance));
        }
    }
    return false;   //we tried everything :(
}