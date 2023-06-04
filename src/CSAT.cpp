//
// Created by Pascal Kessler on 30.05.23.
//

#include "../include/SAT.h"
#include <vector>
#include <cmath>
#include <queue>

std::pair<int,bool> choose_variable(SAT & instance) {
    std::vector<std::pair<double,double>> VariableOccurrences(int(instance.get_number_variables()), std::make_pair<double,double>(0,0));
    std::vector<std::vector<std::tuple<int,bool,int>>*> ClausesLenTwo;
    for (auto & clause : instance.get_clauses()) {
        if (clause.size() == 2) {
            ClausesLenTwo.push_back(&clause);
        }
        double SizeFactor = -log(1-1/pow((pow(2, clause.size()) -1),2));
        for (auto & tuple : clause) {
            if (std::get<2>(tuple) == 0) {
                if (std::get<1>(tuple)) {
                    VariableOccurrences[std::get<0>(tuple)-1].first += SizeFactor;
                }
                else {
                    VariableOccurrences[std::get<0>(tuple)-1].second += SizeFactor;
                }
            }
        }
    }

    for (auto & clausefix : ClausesLenTwo) {
        std::tuple<int,bool,int> First = std::make_tuple(std::get<0>((*clausefix)[0]),!std::get<1>((*clausefix)[0]),0);
        std::tuple<int,bool,int> Second = std::make_tuple(std::get<0>((*clausefix)[1]),!std::get<1>((*clausefix)[1]),0);
        if (std::get<1>(First) and std::get<1>(Second)) { //both variables negated
            for (auto & clause : instance.get_clauses()) {
                double SizeFactor = -log(1 - 1 / pow((pow(2, clause.size()) - 1), 2));
                for (auto &tuple: clause) {
                    if (tuple == First) {
                        VariableOccurrences[std::get<0>(Second) - 1].second += SizeFactor;
                    }
                    if (tuple == Second) {
                        VariableOccurrences[std::get<0>(First) - 1].second += SizeFactor;
                    }
                }
            }
        }
        else if (!std::get<1>(First) and std::get<1>(Second)) { //only second variable negated
            for (auto & clause : instance.get_clauses()) {
                double SizeFactor = -log(1 - 1 / pow((pow(2, clause.size()) - 1), 2));
                for (auto &tuple: clause) {
                    if (tuple == First) {
                        VariableOccurrences[std::get<0>(Second) - 1].second += SizeFactor;
                    }
                    if (tuple == Second) {
                        VariableOccurrences[std::get<0>(First) - 1].first += SizeFactor;
                    }
                }
            }
        }
        else if (std::get<1>(First) and !std::get<1>(Second)) { //only first variable negated
            for (auto & clause : instance.get_clauses()) {
                double SizeFactor = -log(1 - 1 / pow((pow(2, clause.size()) - 1), 2));
                for (auto &tuple: clause) {
                    if (tuple == First) {
                        VariableOccurrences[std::get<0>(Second) - 1].first += SizeFactor;
                    }
                    if (tuple == Second) {
                        VariableOccurrences[std::get<0>(First) - 1].second += SizeFactor;
                    }
                }
            }
        }
        else { //both normal
            for (auto & clause : instance.get_clauses()) {
                double SizeFactor = -log(1 - 1 / pow((pow(2, clause.size()) - 1), 2));
                for (auto &tuple: clause) {
                    if (tuple == First) {
                        VariableOccurrences[std::get<0>(Second) - 1].first += SizeFactor;
                    }
                    if (tuple == Second) {
                        VariableOccurrences[std::get<0>(First) - 1].first += SizeFactor;
                    }
                }
            }
        }
    }

    int chosen_variable = -1;
    double maximum_value = 0;
    for (int variable = 0; variable < VariableOccurrences.size(); variable++) {
        double const current_value = VariableOccurrences[variable].first + VariableOccurrences[variable].second + 1.5 * std::min(VariableOccurrences[variable].first, VariableOccurrences[variable].second);
        if (current_value > maximum_value) {
            chosen_variable = variable;
            maximum_value = current_value;
        }
    }
    return std::make_pair(chosen_variable, VariableOccurrences[chosen_variable].first >= VariableOccurrences[chosen_variable].second);
}

bool unit_propagation(SAT & instance) {
    SAT ClonedInstance = instance;
    bool found_variable_fix = true;
    while (found_variable_fix) {
        found_variable_fix = false;
        for (auto & clause : ClonedInstance.get_clauses()) {
            if (clause.size() == 1) {
                found_variable_fix = true;
                ClonedInstance.set_variable(std::get<0>(clause[0]),std::get<1>(clause[0]));
            }
        }
        auto clause_iterator = ClonedInstance.get_clauses().begin(); //zeigt der iterator hier auf die clauses von der instanz oder zeigt der jetzt ins nichts?
        while (clause_iterator != ClonedInstance.get_clauses().begin()) {
            auto literal_iterator = clause_iterator->begin();
            while (literal_iterator != clause_iterator->end()) {
                if (std::get<2>(*literal_iterator) == 1) {
                    clause_iterator = ClonedInstance.get_clauses().erase(clause_iterator);
                    literal_iterator = clause_iterator->begin();
                }
                else if (std::get<2>(*literal_iterator) == -1) {
                    literal_iterator = clause_iterator->erase(literal_iterator);
                }
                else {
                    ++ literal_iterator;
                }
            }
            if (clause_iterator->empty()) {
                return false;
            }
            else {
                ++ clause_iterator;
            }
        }

    }
    instance = ClonedInstance;
    return true;
}

enum class LocalProcessing : int {backtrack = -1, not_simplified = 0, simplified = 1};

LocalProcessing local_processing(std::vector<int> & SortedVariables, SAT & instance) {
    for (int variable = 0; variable < 0.05*SortedVariables.size(); variable++) {
        SAT PositiveClonedInstance = instance;
        SAT NegativeClonedInstance = instance;
        for (int clause = 0; clause < NegativeClonedInstance.get_clauses().size(); clause++) {  //we set the variable to false
            bool satisfied = false;
            bool occurred = false;
            for (int tuple = 0; tuple < NegativeClonedInstance.get_clauses()[clause].size(); tuple++) {
                if (std::get<0>(NegativeClonedInstance.get_clauses()[clause][tuple]) == SortedVariables[variable]) {
                    occurred = true;    //the variable occurs in this clause..
                    if (not std::get<1>(NegativeClonedInstance.get_clauses()[clause][tuple])) {
                        satisfied = true;    //..and also satisfies the clause
                    }
                }
            }
            if (occurred and satisfied) {
                NegativeClonedInstance.delete_clause(clause);
                clause -= 1;
            }
            else if (occurred) {
                NegativeClonedInstance.delete_literal(clause, SortedVariables[variable]);
            }
        }
        for (int clause = 0; clause < PositiveClonedInstance.get_clauses().size(); clause++) {    //same but set variable to true
            bool satisfied = false;
            bool occurred = false;
            for (int tuple = 0; tuple < PositiveClonedInstance.get_clauses()[clause].size(); tuple++) {
                if (std::get<0>(PositiveClonedInstance.get_clauses()[clause][tuple]) == SortedVariables[variable]) {
                    occurred = true;
                    if (std::get<1>(PositiveClonedInstance.get_clauses()[clause][tuple])) {
                        satisfied = true;
                    }
                }
            }
            if (occurred and satisfied) {
                PositiveClonedInstance.delete_clause(clause);
                clause -= 1;
            }
            else if (occurred) {
                PositiveClonedInstance.delete_literal(clause, SortedVariables[variable]);
            }
        }
        bool result_negative_clone = unit_propagation(NegativeClonedInstance);  //remark: This may simplify the cloned instances
        bool result_positive_clone = unit_propagation(PositiveClonedInstance);
        if (!result_negative_clone and !result_positive_clone) {
            return LocalProcessing::backtrack;
        }
        else if (result_negative_clone and !result_positive_clone) {
            instance = NegativeClonedInstance;
            return LocalProcessing::simplified;
        }
        else if (!result_negative_clone and result_positive_clone) {
            instance = PositiveClonedInstance;
            return LocalProcessing::simplified;
        }
    }
    return LocalProcessing::not_simplified;
}

bool c_sat(SAT instance) {

}


/*
  bool unit_propagation(SAT & instance) {
    SAT ClonedInstance = instance;
    int VariableFix;
    for (auto & clausefix : ClonedInstance.get_clauses()) {
        if (clausefix.size() == 1) {
            for (int clause = 0; clause < ClonedInstance.get_clauses().size(); clause++) {
                if (ClonedInstance.get_clauses()[clause].size() == 1) {
                    if (std::get<1>(ClonedInstance.get_clauses()[clause][0]) != std::get<1>(clausefix[0])) {
                        return false;
                    }
                    else {
                        ClonedInstance.delete_clause(clause);
                    }
                }
                else {
                    bool satisfied = false;
                    bool occurred = false;
                    for (int tuple = 0; tuple < ClonedInstance.get_clauses()[clause].size(); tuple++) {
                        if (std::get<0>(ClonedInstance.get_clauses()[clause][tuple]) == std::get<0>(clausefix[0])) {
                            occurred = true;
                            if (!std::get<1>(ClonedInstance.get_clauses()[clause][tuple])) {
                                satisfied = true;
                            }
                        }
                    }
                    if (occurred and satisfied) {
                        ClonedInstance.delete_clause(clause);
                    }
                    else if (occurred) {
                        ClonedInstance.delete_literal(clause, std::get<0>(clausefix[0]));
                    }
                }
            }
        }
    }
    instance = ClonedInstance;
    return true;
}
 */
