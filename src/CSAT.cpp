//
// Created by Pascal Kessler on 30.05.23.
//

#include <slimSAT.h>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>
#include <stack>



SAT initialize_instance(SAT instance, const std::vector<int> & VariableSetting) {
    for (int clause = 0; clause < instance.get_clauses().size(); clause++) {
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

std::pair<int,bool> choose_variable(SAT & instance, const std::vector<const double> & SizeFactor) {
    std::vector<std::pair<double,double>> VariableOccurrences(int(instance.get_number_variables()), std::make_pair(0,0));
    std::vector<std::vector<std::vector<int>>::iterator> ClausesLenTwo;
    for (auto clause = instance.get_clauses().begin(); clause != instance.get_clauses().end(); clause++) {
        if ((*clause).size() == 2) {
            ClausesLenTwo.push_back(clause);
        }
        for (auto & literal : *clause) {
            if (literal > 0) {   //positive occurrence
                VariableOccurrences[std::abs(literal)-1].first += SizeFactor[(*clause).size()];
            }
            else {  //negative occurrence
                VariableOccurrences[std::abs(literal)-1].second += SizeFactor[(*clause).size()];
            }
        }
    }

    for (auto & clausefix : ClausesLenTwo) {
        if ((*clausefix)[0] < 0 and (*clausefix)[1] < 0) { //both variables negated
            for (auto & clause : instance.get_clauses()) {
                for (auto & literal : clause) {
                    if (-literal == (*clausefix)[0]) {
                        VariableOccurrences[-(*clausefix)[1] - 1].second += SizeFactor[clause.size()];
                    }
                    if (-literal == (*clausefix)[1]) {
                        VariableOccurrences[-(*clausefix)[0] - 1].second += SizeFactor[clause.size()];
                    }
                }
            }
        }
        else if ((*clausefix)[0] > 0 and (*clausefix)[1] < 0) { //only second variable negated
            for (auto & clause : instance.get_clauses()) {
                for (auto & literal: clause) {
                    if (-literal == (*clausefix)[0]) {
                        VariableOccurrences[-(*clausefix)[1] - 1].second += SizeFactor[clause.size()];
                    }
                    if (-literal == (*clausefix)[1]) {
                        VariableOccurrences[(*clausefix)[0] - 1].first += SizeFactor[clause.size()];
                    }
                }
            }
        }
        else if ((*clausefix)[0] < 0 and (*clausefix)[1] > 0) { //only first variable negated
            for (auto & clause : instance.get_clauses()) {
                for (auto & literal: clause) {
                    if (-literal == (*clausefix)[0]) {
                        VariableOccurrences[(*clausefix)[1] - 1].first += SizeFactor[clause.size()];
                    }
                    if (-literal == (*clausefix)[1]) {
                        VariableOccurrences[-(*clausefix)[0] - 1].second += SizeFactor[clause.size()];
                    }
                }
            }
        }
        else { //both positive
            for (auto & clause : instance.get_clauses()) {
                for (auto & literal: clause) {
                    if (-literal == (*clausefix)[0]) {
                        VariableOccurrences[(*clausefix)[1] - 1].first += SizeFactor[clause.size()];
                    }
                    if (-literal == (*clausefix)[1]) {
                        VariableOccurrences[(*clausefix)[0] - 1].first += SizeFactor[clause.size()];
                    }
                }
            }
        }
    }

    int chosen_variable = -1;
    double maximum_value = 0;
    for (int variable = 0; variable < VariableOccurrences.size(); variable++) { //we want select the variable with the most weight
        double const current_value = VariableOccurrences[variable].first + VariableOccurrences[variable].second + 1.5 * std::min(VariableOccurrences[variable].first, VariableOccurrences[variable].second);
        if (current_value > maximum_value) {
            chosen_variable = variable;
            maximum_value = current_value;
        }
    }
    return std::make_pair(chosen_variable + 1, VariableOccurrences[chosen_variable].first >= VariableOccurrences[chosen_variable].second);  //we return the chosen variable and if there are more/equal positive occurrences than negative ones
}

bool unit_propagation(SAT & instance, std::vector<int> & SetVariables) {
    SAT ClonedInstance = instance;
    bool found_variable_fix = true;
    while (found_variable_fix) {    //we want to stop after one iteration without change
        found_variable_fix = false;
        for (auto & clause : ClonedInstance.get_clauses()) {
            if (clause.size() == 1 and clause[0] != 0 and clause[0] != ClonedInstance.get_number_variables()+1) {
                found_variable_fix = true;
                int literal_fix = clause[0];
                SetVariables.push_back(literal_fix);
                for (auto & setclause : ClonedInstance.get_clauses()) {
                   for (auto & literal : setclause) {
                       if (literal == literal_fix) {
                           literal = 0;
                       }
                       else if (-literal == literal_fix) {
                       literal = ClonedInstance.get_number_variables()+1;
                       }
                   }
                }
            }
        }
        auto clause_iterator = ClonedInstance.get_clauses().begin();
        while (clause_iterator != ClonedInstance.get_clauses().end()) {
            auto literal_iterator = clause_iterator->begin();
            while (clause_iterator != ClonedInstance.get_clauses().end() and literal_iterator != clause_iterator->end()) {
                if (*literal_iterator == 0) {  //we satisfy the clause, thus it gets erased
                    std::destroy_at(&literal_iterator);
                    clause_iterator = ClonedInstance.erase_clause(clause_iterator);
                    if (clause_iterator != ClonedInstance.get_clauses().end()) {
                        literal_iterator = clause_iterator->begin();
                    }
                }
                else if (*literal_iterator == ClonedInstance.get_number_variables()+1) {    //we delete the literal from the clause
                    literal_iterator = clause_iterator->erase(literal_iterator);
                }
                else {
                    ++literal_iterator;
                }
            }
            if (clause_iterator != ClonedInstance.get_clauses().end()) {
                if (clause_iterator->empty()) { //this clause is now empty, that's a contradiction
                    SetVariables.clear();
                    return false;
                }
                else {
                    ++clause_iterator;
                }
            }
        }

    }
    instance = ClonedInstance;  //we simplified the instance
    return true;
}

enum class LocalProcessing : int {backtrack = -1, not_simplified = 0, simplified = 1, solved = 4};

std::pair<LocalProcessing,std::vector<int>> local_processing(SAT & instance, const std::vector<const double> & SizeFactor) {
    std::vector<int> NoInformation {-1};
    std::vector<std::pair<int,double>> SortedVariables;
    for(int variable = 1; variable <= instance.get_number_variables(); variable++) {
        SortedVariables.emplace_back(variable,0);
    }
    for (auto & clause : instance.get_clauses()) {
        for (auto & literal : clause) {
            SortedVariables[std::abs(literal)-1].second += SizeFactor[clause.size()];
        }
    }
    std::sort(SortedVariables.begin(), SortedVariables.end(), [] (const std::pair<int,double> & a, const std::pair<int,double> & b) {return a.second > b.second;});
    //we now have the variables sorted
    for (int variable = 0; variable < 0.05*SortedVariables.size(); variable++) {
        SAT positive_cloned_instance = instance;
        SAT negative_cloned_instance = instance;
        std::vector<int> SetVariablesNegative {-SortedVariables[variable].first};
        std::vector<int> SetVariablesPositive {SortedVariables[variable].first};
        negative_cloned_instance.set_variable_false(SortedVariables[variable].first);
        positive_cloned_instance.set_variable_true(SortedVariables[variable].first);
        bool result_negative_clone = unit_propagation(negative_cloned_instance, SetVariablesNegative);  //remark: This may simplify the cloned instances
        bool result_positive_clone = unit_propagation(positive_cloned_instance, SetVariablesPositive);
        if (!result_negative_clone and !result_positive_clone) {    //we found a contradiction
            return std::make_pair(LocalProcessing::backtrack,NoInformation);
        }
        else if (result_negative_clone and !result_positive_clone) {    //we fix the variable on false
            instance = std::move(negative_cloned_instance);
            return std::make_pair(LocalProcessing::simplified,SetVariablesNegative);
        }
        else if (!result_negative_clone and result_positive_clone) {   //we fix the variable on true
            instance = std::move(positive_cloned_instance);
            return std::make_pair(LocalProcessing::simplified,SetVariablesPositive);
        }
        else if (negative_cloned_instance.get_clauses().empty() or positive_cloned_instance.get_clauses().empty()) {    //we solved one of the instances, so we are done
            return std::make_pair(LocalProcessing::solved,NoInformation);
        }
    }
    return std::make_pair(LocalProcessing::not_simplified,NoInformation); //didn't help, we want to do normal algorithm again
}

enum class SettingInformation : int {pop = 1, flip = 0,};

void backtrack(std::stack<std::pair<int,SettingInformation>> & DifferentPaths, std::vector<int> & VariableSetting) {
    while (!DifferentPaths.empty() and DifferentPaths.top().second == SettingInformation::pop) {
        VariableSetting[DifferentPaths.top().first] = 0;
        DifferentPaths.pop();
    }
    if (!DifferentPaths.empty()) {
        VariableSetting[DifferentPaths.top().first] = -VariableSetting[DifferentPaths.top().first];
        DifferentPaths.top().second = SettingInformation::pop;
    }
}

bool c_sat(SAT & instance) {
    int size_of_biggest_clause = (*std::max_element(instance.get_clauses().begin(), instance.get_clauses().end(),[](const std::vector<int> &a, const std::vector<int> &b) {return a.size() < b.size();})).size();
    std::vector<const double> SizeFactor{0};   //initialize with 0 so size and place in vector line up
    for (int size = 1; size <= size_of_biggest_clause; size++) {
        SizeFactor.push_back(-log(1 - 1 / pow((pow(2, size) - 1), 2)));
    }
    if (instance.get_clauses().empty()) {
        return true;
    }
    std::stack<std::pair<int, SettingInformation>> DifferentPaths; //here we store the variables already set
    std::vector<int> VariableSetting(int(instance.get_number_variables() + 1), 0);
    std::pair initial_chosen_variable = choose_variable(instance, SizeFactor);
    DifferentPaths.emplace(initial_chosen_variable.first, SettingInformation::flip);
    if (initial_chosen_variable.second) {
        VariableSetting[initial_chosen_variable.first] = initial_chosen_variable.first;
    }
    else {
        VariableSetting[initial_chosen_variable.first] = -initial_chosen_variable.first;
    }
    bool tried_local_processing = false;
    if (std::any_of(instance.get_clauses().begin(), instance.get_clauses().end(),[](const std::vector<int> &clause) { return clause.empty(); })) {
        return false;
    }
    bool initialize = false;
    SAT current_instance = initialize_instance(instance, VariableSetting);
    while (not DifferentPaths.empty()) {
        if (initialize) {
            current_instance = initialize_instance(instance, VariableSetting);
            initialize = false;
        }
        if (current_instance.get_clauses().empty()) {   //clauses get deleted, when satisfied
            return true;
        }
        else if (DifferentPaths.size() < (1 - 0.05 * 3) * instance.get_number_variables() or tried_local_processing) { //like a_sat but with refined chosen_variable
            tried_local_processing = false;
            std::pair chosen_variable = choose_variable(current_instance, SizeFactor);
            if (chosen_variable.second) {
                SAT cloned_instance = current_instance;
                if (current_instance.set_variable_false(chosen_variable.first)) {
                    VariableSetting[chosen_variable.first] = -chosen_variable.first;
                    if (cloned_instance.set_variable_true(chosen_variable.first)) {
                        DifferentPaths.emplace(chosen_variable.first, SettingInformation::flip);
                    }
                    else {
                        DifferentPaths.emplace(chosen_variable.first,SettingInformation::pop);
                    }
                }
                else if (cloned_instance.set_variable_true(chosen_variable.first)) {
                    current_instance = std::move(cloned_instance);
                    VariableSetting[chosen_variable.first] = chosen_variable.first;
                    DifferentPaths.emplace(chosen_variable.first, SettingInformation::pop);
                }
                else {
                    backtrack(DifferentPaths, VariableSetting);
                    initialize = true;
                }
            }
            else {
                SAT cloned_instance = current_instance;
                if (current_instance.set_variable_true(chosen_variable.first)) {
                    VariableSetting[chosen_variable.first] = chosen_variable.first;
                    if (cloned_instance.set_variable_false(chosen_variable.first)) {
                        DifferentPaths.emplace(chosen_variable.first, SettingInformation::flip);
                    }
                    else {
                        DifferentPaths.emplace(chosen_variable.first, SettingInformation::pop);
                    }
                }
                else if (cloned_instance.set_variable_false(chosen_variable.first)) {
                    current_instance = std::move(cloned_instance);
                    DifferentPaths.emplace(chosen_variable.first, SettingInformation::pop);
                    VariableSetting[chosen_variable.first] = -chosen_variable.first;
                }
                else {
                    backtrack(DifferentPaths, VariableSetting);
                    initialize = true;
                }
            }
        }
        else {  //local processing
            std::pair<LocalProcessing, std::vector<int>> result_local_processing = local_processing(current_instance, SizeFactor);
            if (result_local_processing.first != LocalProcessing::backtrack) {    //dead-end, we try another next path
                if (result_local_processing.first == LocalProcessing::solved) {
                    return true;
                }
                else if (result_local_processing.first == LocalProcessing::not_simplified) {  //local processing didn't help
                    tried_local_processing = true;
                }
                else {
                    for (const auto & literal: result_local_processing.second) {
                        DifferentPaths.emplace(std::abs(literal), SettingInformation::pop);
                        VariableSetting[std::abs(literal)] = literal;
                    }
                }
            }
            else {
                backtrack(DifferentPaths, VariableSetting);
                initialize = true;
            }
        }
    }
    return false;   //tried everything
}
