//
// Created by Pascal Kessler on 30.05.23.
//

#include <slimSAT.h>
#include <vector>
#include <cmath>
#include <stack>
#include <algorithm>

std::pair<int,bool> choose_variable(SAT & instance, const std::vector<double> & SizeFactor) {
    std::vector<std::pair<double,double>> VariableOccurrences(int(instance.get_number_variables()), std::make_pair(0,0));   //to store the weighted occurrences of variables, first: positive occurrence, second: negative occurrence
    std::vector<std::pair<int,int>> VariablesInClausesLenTwo (int(instance.get_number_variables()+1),std::make_pair(0,0));  //to store how often a literal occurs in a clause of length 2, first: positive literal, second: negative literal
    for (const auto & clause : instance.get_clauses()) {
        const int clause_size = clause.size();
        if (clause_size == 2) {
            if (clause[0] > 0) {    //positive occurrence
                VariablesInClausesLenTwo[clause[0]].first += 1;
            }
            else {  //negative occurrence
                VariablesInClausesLenTwo[-clause[0]].second += 1;
            }
            if (clause[1] > 0) {    //positive occurrence
                VariablesInClausesLenTwo[clause[1]].first += 1;
            }
            else {  //negative occurrence
                VariablesInClausesLenTwo[-clause[1]].second += 1;
            }
        }
        for (const auto & literal : clause) {
            if (literal > 0) {   //positive occurrence
                VariableOccurrences[literal-1].first += SizeFactor[clause_size];
            }
            else {  //negative occurrence
                VariableOccurrences[-literal-1].second += SizeFactor[clause_size];
            }
        }
    }

    //we now iterate through the instance again and add to the weights, if a literal that occurred in a length 2 clause, occurs negated
    for (const auto & clause : instance.get_clauses()) {
        for (const auto & literal : clause) {
            if (literal > 0 and VariableOccurrences[literal - 1].first < 99999) {   // <99999 to prevent overflow
                VariableOccurrences[literal - 1].first += SizeFactor[clause.size()]*VariablesInClausesLenTwo[literal].second;
            }
            else if (literal < 0 and VariableOccurrences[-literal - 1].second < 99999){
                VariableOccurrences[-literal - 1].second += SizeFactor[clause.size()]*VariablesInClausesLenTwo[-literal].first;
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
    //we return the chosen variable and if the variable occurred more often positive than negative
    return std::make_pair(chosen_variable + 1, VariableOccurrences[chosen_variable].first >= VariableOccurrences[chosen_variable].second);  //we return the chosen variable and if there are more/equal positive occurrences than negative ones
}

bool unit_propagation(SAT & instance) {
    SAT ClonedInstance = instance;
    bool found_variable_fix = true;
    while (found_variable_fix) {    //we want to stop after one iteration without change
        found_variable_fix = false;
        for (auto & clause : ClonedInstance.get_clauses()) {
            if (clause.size() == 1 and clause[0] != 0 and clause[0] != ClonedInstance.get_number_variables()+1) {
                found_variable_fix = true;
                int literal_fix = clause[0];
                for (auto & setclause : ClonedInstance.get_clauses()) {
                   for (auto & literal : setclause) {
                       if (literal == literal_fix) {
                           literal = 0;     //this literal satisfies the clause
                       }
                       else if (-literal == literal_fix) {
                       literal = ClonedInstance.get_number_variables()+1;   //this literal does not
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
                    return false;
                }
                else {
                    ++clause_iterator;
                }
            }
        }

    }
    instance = std::move(ClonedInstance);  //we simplified the instance
    return true;
}

enum class LocalProcessing : int {backtrack = -1, not_simplified = 0, simplified = 1, solved = 4};

LocalProcessing local_processing(SAT & instance, const std::vector<double> & SizeFactor) {
    std::vector<std::pair<int,double>> SortedVariables;

    //we initialise the vector
    for(int variable = 1; variable <= instance.get_number_variables(); variable++) {
        SortedVariables.emplace_back(variable,0);
    }
    //we add to the weight of the variables
    for (auto & clause : instance.get_clauses()) {
        for (auto & literal : clause) {
            SortedVariables[std::abs(literal)-1].second += SizeFactor[clause.size()];
        }
    }
    std::sort(SortedVariables.begin(), SortedVariables.end(), [] (const std::pair<int,double> & a, const std::pair<int,double> & b) {return a.second > b.second;});
    //we now have the variables sorted

    //we remove variables already set
    auto remove_zeros = std::find_if(SortedVariables.begin(),SortedVariables.end(),[] (const std::pair<int,double> & pair) {return pair.second == 0;});
    SortedVariables.erase(remove_zeros,SortedVariables.end());

    for (int position = 0; position < 0.05*SortedVariables.size(); position++) {    //we only try for a portion of the variables
        SAT PositiveClonedInstance = instance;
        SAT NegativeClonedInstance = instance;
        NegativeClonedInstance.set_variable_false(SortedVariables[position].first);
        PositiveClonedInstance.set_variable_true(SortedVariables[position].first);
        bool result_negative_clone = unit_propagation(NegativeClonedInstance);  //remark: This may simplify the cloned instances
        bool result_positive_clone = unit_propagation(PositiveClonedInstance);

        if (!result_negative_clone and !result_positive_clone) {    //we found a contradiction
            return LocalProcessing::backtrack;
        }
        else if (result_negative_clone and !result_positive_clone) {    //we fix the variable on false
            instance = std::move(NegativeClonedInstance);
            return LocalProcessing::simplified;
        }
        else if (!result_negative_clone and result_positive_clone) {   //we fix the variable on true
            instance = std::move(PositiveClonedInstance);
            return LocalProcessing::simplified;
        }
        else if (NegativeClonedInstance.get_clauses().empty() or PositiveClonedInstance.get_clauses().empty()) {    //we solved one of the instances, so we are done
            return LocalProcessing::solved;
        }
    }
    return LocalProcessing::not_simplified; //didn't help, we want to do normal algorithm again
}

bool c_sat(SAT & instance) {
    int size_of_biggest_clause = (*std::max_element(instance.get_clauses().begin(), instance.get_clauses().end() , [] (const std::vector<int> & a, const std::vector<int> & b) {return a.size() < b.size();})).size();
    std::vector<double> SizeFactor {0};   //initialize with 0 so size and place in vector line up

    for (int size = 1; size <= size_of_biggest_clause; size++) {    //compute all the different weights that we could want to use
        SizeFactor.push_back(-log(1-1/pow((pow(2, size) -1),2)));
    }

    std::stack<SAT> DifferentPaths; //here we store the instances with different variable settings
    DifferentPaths.push(instance);
    bool tried_local_processing = false;

    //do we have an instance that makes sense?
    if (std::any_of(DifferentPaths.top().get_clauses().begin(), DifferentPaths.top().get_clauses().end(),[](const std::vector<int> &clause) { return clause.empty(); })) {
        return false;
    }

    while (not DifferentPaths.empty()) {
        SAT current_instance = std::move(DifferentPaths.top());
        DifferentPaths.pop();

        if (current_instance.get_number_clauses() == 0) {   //clauses get deleted, when satisfied
            return true;
        }

        else if (current_instance.get_number_assigned_variables() < (0.5)*instance.get_number_variables() or tried_local_processing) { //like a_sat but with refined chosen_variable
                tried_local_processing = false;
                std::pair chosen_variable = choose_variable(current_instance, SizeFactor);
                if (chosen_variable.second) {   //the variable occurs more often positive
                    SAT cloned_instance = current_instance;
                    if (cloned_instance.set_variable_false(chosen_variable.first)) {
                        DifferentPaths.push(cloned_instance);
                    }
                    if (current_instance.set_variable_true(chosen_variable.first)){
                        DifferentPaths.push(std::move(current_instance));
                    }
                }
                else {      //the variable occurs more often negative
                    SAT cloned_instance = current_instance;
                    if (cloned_instance.set_variable_true(chosen_variable.first)) {
                        DifferentPaths.push(cloned_instance);
                    }
                    if (current_instance.set_variable_false(chosen_variable.first)){
                        DifferentPaths.push(std::move(current_instance));
                    }
                }
            }

        else {  //local processing
            LocalProcessing result_local_processing = local_processing(current_instance, SizeFactor);
            if (result_local_processing == LocalProcessing::backtrack) {    //dead-end, we try another next path
                continue;
            }
            else {
                DifferentPaths.push(std::move(current_instance));
                if (result_local_processing == LocalProcessing::solved) {   //we are done
                    return true;
                }
                if (result_local_processing == LocalProcessing::not_simplified) {  //local processing didn't help
                    tried_local_processing = true;
                }
            }
        }
    }
    return false;   //we tried everything
}
