//
// Created by Pascal Kessler on 30.05.23.
//

#include <SAT.h>
#include <vector>
#include <cmath>
#include <queue>

std::pair<int,bool> choose_variable(SAT & instance, const std::vector<const double> & SizeFactor) {
    std::vector<std::pair<double,double>> VariableOccurrences(int(instance.get_number_variables()), std::make_pair<double,double>(0,0));
    std::vector<std::vector<std::vector<std::tuple<int,bool,int>>>::iterator> ClausesLenTwo;
    for (auto clause = instance.get_clause_iterator(); clause != instance.get_clause_iterator_end(); clause++) {
        if ((*clause).size() == 2) {
            ClausesLenTwo.push_back(clause);
        }
        for (auto & tuple : *clause) {
            if (std::get<1>(tuple)) {   //positive occurrence
                VariableOccurrences[std::get<0>(tuple)-1].first += SizeFactor[(*clause).size()];
            }
            else {  //negative occurrence
                VariableOccurrences[std::get<0>(tuple)-1].second += SizeFactor[(*clause).size()];
            }
        }
    }

    for (auto & clausefix : ClausesLenTwo) {
        std::tuple<int,bool,int> First = std::make_tuple(std::get<0>((*clausefix)[0]),!std::get<1>((*clausefix)[0]),0);
        std::tuple<int,bool,int> Second = std::make_tuple(std::get<0>((*clausefix)[1]),!std::get<1>((*clausefix)[1]),0);
        if (std::get<1>(First) and std::get<1>(Second)) { //both variables negated
            for (auto & clause : instance.get_clauses()) {
                for (auto &tuple: clause) {
                    if (tuple == First) {
                        VariableOccurrences[std::get<0>(Second) - 1].second += SizeFactor[clause.size()];
                    }
                    if (tuple == Second) {
                        VariableOccurrences[std::get<0>(First) - 1].second += SizeFactor[clause.size()];
                    }
                }
            }
        }
        else if (!std::get<1>(First) and std::get<1>(Second)) { //only second variable negated
            for (auto & clause : instance.get_clauses()) {
                for (auto &tuple: clause) {
                    if (tuple == First) {
                        VariableOccurrences[std::get<0>(Second) - 1].second += SizeFactor[clause.size()];
                    }
                    if (tuple == Second) {
                        VariableOccurrences[std::get<0>(First) - 1].first += SizeFactor[clause.size()];
                    }
                }
            }
        }
        else if (std::get<1>(First) and !std::get<1>(Second)) { //only first variable negated
            for (auto & clause : instance.get_clauses()) {
                for (auto &tuple: clause) {
                    if (tuple == First) {
                        VariableOccurrences[std::get<0>(Second) - 1].first += SizeFactor[clause.size()];
                    }
                    if (tuple == Second) {
                        VariableOccurrences[std::get<0>(First) - 1].second += SizeFactor[clause.size()];
                    }
                }
            }
        }
        else { //both positive
            for (auto & clause : instance.get_clauses()) {
                for (auto &tuple: clause) {
                    if (tuple == First) {
                        VariableOccurrences[std::get<0>(Second) - 1].first += SizeFactor[clause.size()];
                    }
                    if (tuple == Second) {
                        VariableOccurrences[std::get<0>(First) - 1].first += SizeFactor[clause.size()];
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

bool unit_propagation(SAT & instance) {
    SAT ClonedInstance = instance;
    bool found_variable_fix = true;
    while (found_variable_fix) {    //we want to stop after one iteration without change
        found_variable_fix = false;
        for (auto & clause : ClonedInstance.get_clauses()) {
            if (clause.size() == 1) {
                found_variable_fix = true;
                if (std::get<2>(clause[0]) == 0) {  //if not yet set we set the variable such that it satisfies the clause
                    ClonedInstance.set_variable(std::get<0>(clause[0]), std::get<1>(clause[0]));
                }
                else if (std::get<2>(clause[0]) == -1) {    //this is a contradiction, the clause is not satisfiable anymore
                    return false;
                }
            }
            else if (clause.empty()) {
                return false;
            }
        }
        auto clause_iterator = ClonedInstance.get_clauses().begin();
        while (clause_iterator != ClonedInstance.get_clauses().end()) {
            auto literal_iterator = clause_iterator->begin();
            while (clause_iterator != ClonedInstance.get_clauses().end() and literal_iterator != clause_iterator->end()) {
                if (std::get<2>(*literal_iterator) == 1) {  //we satisfy the clause, thus it gets erased
                    std::destroy_at(&literal_iterator);
                    clause_iterator = ClonedInstance.erase_clause(clause_iterator);
                    if (clause_iterator != ClonedInstance.get_clauses().end()) {
                        literal_iterator = clause_iterator->begin();
                    }
                }
                else if (std::get<2>(*literal_iterator) == -1) {    //we delete the literal from the clause
                    literal_iterator = clause_iterator->erase(literal_iterator);
                }
                else {
                    ++literal_iterator;
                }
            }
            if (clause_iterator != ClonedInstance.get_clauses().end()) {
                if (clause_iterator->empty()) { //this clause is now empty, that's a contradiction
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

LocalProcessing local_processing(SAT & instance, const std::vector<const double> & SizeFactor) {
    std::vector<std::pair<int,double>> SortedVariables;
    for(int variable = 1; variable <= instance.get_number_variables(); variable++) {
        SortedVariables.push_back(std::make_pair(variable,0));
    }
    for (auto & clause : instance.get_clauses()) {
        for (auto & tuple : clause) {
            SortedVariables[std::get<0>(tuple)-1].second += SizeFactor[clause.size()];
        }
    }
    std::sort(SortedVariables.begin(), SortedVariables.end(), [] (const std::pair<int,double> & a, const std::pair<int,double> & b) {return a.second > b.second;});
    //we now have the variables sorted
    for (int variable = 0; variable < 0.05*SortedVariables.size(); variable++) {
        SAT PositiveClonedInstance = instance;
        SAT NegativeClonedInstance = instance;
        NegativeClonedInstance.set_variable_false(SortedVariables[variable].first);
        PositiveClonedInstance.set_variable_true(SortedVariables[variable].first);
        bool result_negative_clone = unit_propagation(NegativeClonedInstance);  //remark: This may simplify the cloned instances
        bool result_positive_clone = unit_propagation(PositiveClonedInstance);
        if (!result_negative_clone and !result_positive_clone) {    //we found a contradiction
            return LocalProcessing::backtrack;
        }
        else if (result_negative_clone and !result_positive_clone) {    //we fix the variable on false
            instance = NegativeClonedInstance;
            return LocalProcessing::simplified;
        }
        else if (!result_negative_clone and result_positive_clone) {    //hier sagt es mir die Bedingung wäre immer erfüllt, falls wir hier hin kommen??
            instance = PositiveClonedInstance;
            return LocalProcessing::simplified;
        }
        else if (NegativeClonedInstance.get_clauses().empty() or PositiveClonedInstance.get_clauses().empty()) {
            return LocalProcessing::solved;
        }
    }
    return LocalProcessing::not_simplified; //didn't help, we want to do normal algorithm again
}

bool c_sat(SAT & instance) {
    int size_of_biggest_clause = (*std::max_element(instance.get_clause_iterator(), instance.get_clause_iterator_end() , [] (const std::vector<std::tuple<int,bool,int>> & a, const std::vector<std::tuple<int,bool,int>> & b) {return a.size() < b.size();})).size(); //ich wollte es mit std::max machen, hab es aber nicht hinbekommen
    std::vector<const double> SizeFactor {0};   //initialize with 0 so variable and place in vector line up
    for (int size = 1; size <= size_of_biggest_clause; size++) {
        SizeFactor.push_back(-log(1-1/pow((pow(2, size) -1),2)));
    }
    std::queue<SAT> DifferentPaths; //here we store the instances with different variable settings
    DifferentPaths.push(instance);
    bool tried_local_processing = false;
    while (not DifferentPaths.empty()) {
        SAT current_instance = std::move(DifferentPaths.front());
        DifferentPaths.pop();
        if (current_instance.get_number_clauses() == 0) {   //clauses get deleted, when satisfied
            return true;
        }
        else if (std::none_of(current_instance.get_clause_iterator(), current_instance.get_clause_iterator_end(),[](const std::vector<std::tuple<int, bool, int>> &clause) { return clause.empty(); })) {
            if (current_instance.get_number_assigned_variables() < 0.5*instance.get_number_variables() or tried_local_processing) { //like a_sat but with refined chosen_variable
                tried_local_processing = false;
                std::pair chosen_variable = choose_variable(current_instance, SizeFactor);
                if (std::get<1>(chosen_variable)) {
                    SAT cloned_instance = current_instance;
                    cloned_instance.set_variable_false(std::get<0>(chosen_variable));
                    DifferentPaths.push(cloned_instance);
                    current_instance.set_variable_true(std::get<0>(chosen_variable));
                    DifferentPaths.emplace(std::move(current_instance));
                }
                else {
                    SAT cloned_instance = current_instance;
                    cloned_instance.set_variable_true(std::get<0>(chosen_variable));
                    DifferentPaths.push(cloned_instance);
                    current_instance.set_variable_false(std::get<0>(chosen_variable));
                    DifferentPaths.emplace(std::move(current_instance));
                }
            }
            else {  //local processing
                LocalProcessing result_local_processing = local_processing(current_instance, SizeFactor);
                if (result_local_processing != LocalProcessing::backtrack) {    //dead-end, we try another next path
                    DifferentPaths.emplace(std::move(current_instance));
                    if (result_local_processing == LocalProcessing::solved) {
                        return true;
                    }
                    if (result_local_processing == LocalProcessing::not_simplified) {  //local processing didn't help
                        tried_local_processing = true;
                    }
                }
            }
        }
    }
    return false;   //tried everything
}
