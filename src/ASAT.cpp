//
// Created by Pascal Kessler on 27.05.23.
//

#include <slimSAT.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>
#include <unordered_map>

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

SAT initialize_instance(SAT instance, const std::vector<int> & VariableSetting) {
    for (int clause = 0; clause < instance.get_number_clauses(); clause++) {
        bool occurred = false;
        bool satisfied = false;
        for (int & literal : instance.get_clauses()[clause]) {
            if (VariableSetting[std::abs(literal)] != 0) {
                if (literal == VariableSetting[std::abs(literal)]) {
                    satisfied = true;
                }
                else {
                    occurred = true;
                    literal = 0;
                }
            }
        }
        if (satisfied) {
            instance.get_clauses()[clause].clear();
            instance.get_number_clauses() -= 1;
        }
        else if (occurred) {
            instance.delete_literal(clause,0);
        }
    }
    auto NoMoreEmptyClauses = std::remove_if(instance.get_clauses().begin(), instance.get_clauses().end(),[](const std::vector<int> &clause) { return clause.empty(); });
    instance.get_clauses().erase(NoMoreEmptyClauses,instance.get_clauses().end());
    return instance;
}

bool a_sat(SAT & instance) {
    if (std::any_of(instance.get_clauses().begin(), instance.get_clauses().end(),[](const std::vector<int> &clause) { return clause.empty(); })) {
        return false;
    }
    std::queue<std::vector<int>> DifferentPaths; //here we store the different variable settings
    std::vector<int> InitialSetting (int(instance.get_number_variables()+1), 0);
    int InitialChosenVariable = branching_rule(instance);
    InitialSetting[InitialChosenVariable] = InitialChosenVariable;
    DifferentPaths.push(InitialSetting);
    InitialSetting[InitialChosenVariable] = -InitialChosenVariable;
    DifferentPaths.push(InitialSetting);
    while (not DifferentPaths.empty()) {
        std::vector<int> current_setting = std::move(DifferentPaths.front());
        SAT current_instance = initialize_instance(instance,current_setting);
        DifferentPaths.pop();
        if (current_instance.get_number_clauses() == 0) {   //clauses get deleted, when satisfied
            return true;
        }
        int ChosenVariable = branching_rule(current_instance);
        SAT ClonedInstance = current_instance;
        std::vector<int> cloned_setting = current_setting;
        if (ClonedInstance.set_variable_false(ChosenVariable)) {
            cloned_setting[ChosenVariable] = -ChosenVariable;
            DifferentPaths.push(cloned_setting);
        }
        if (current_instance.set_variable_true(ChosenVariable)) {
            current_setting[ChosenVariable] = ChosenVariable;
            DifferentPaths.push(std::move(current_setting));
        }
    }
    return false;   //we tried everything :(
}