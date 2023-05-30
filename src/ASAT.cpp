//
// Created by Pascal Kessler on 27.05.23.
//

#include "../include/SAT.h"
#include <vector>
#include <queue>

int branching_rule(SAT & current_instance) {
    std::vector<std::vector<std::tuple<int,bool,int>>> ShortestClauses;
    int HowShort = std::round_toward_infinity;
    for (auto & clause : current_instance.get_clauses()) {      //ist es hier vlt schlauer mit iteratoren 2-mal über die Klauseln zu gehen, einmal um HowShort zu bestimmen und dann um die kurzen Klauseln hinzuzufügen?
        if (clause.size() < HowShort) { //we found a shorter clause
            HowShort = clause.size();
            ShortestClauses.clear();
            ShortestClauses.push_back(clause);
        }
        else if (clause.size() == HowShort) { //we add this clause
            ShortestClauses.push_back(clause);
        }
    }
    std::vector<int> VariableOccurrences(int(current_instance.get_number_variables()), 0);  //here we store how often variables occur in the shortest clauses
    for (auto & clause : current_instance.get_clauses()) {
        for (auto & tuple : clause) {
            VariableOccurrences[std::get<0>(tuple)-1] += 1;
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
    std::queue<SAT> DifferentPaths; //here we store the instances, where we put the selected variable to false instead of true
    DifferentPaths.push(instance);
    while (not DifferentPaths.empty()) {
        SAT current_instance = DifferentPaths.front();
        if (current_instance.get_number_clauses() == 0) {   //clauses get deleted, when satisfied
            return true;
        } else if (std::count_if(begin(current_instance.get_clauses()), end(current_instance.get_clauses()),
                                 [](const std::vector<std::tuple<int, bool, int>> &clause) { return clause.empty(); }) >
                   0) {
            DifferentPaths.pop();   //instance not satisfiable anymore, we found a clause which can not be satisfied anymore
        } else {
            int ChosenVariable = branching_rule(current_instance);
            SAT ClonedInstance = current_instance;
            for (int clause = 0; clause < ClonedInstance.get_clauses().size(); clause++) {  //we set the variable to false
                bool satisfied = false;
                bool occurred = false;
                for (int tuple = 0; tuple < ClonedInstance.get_clauses()[clause].size(); tuple++) {
                    if (std::get<0>(ClonedInstance.get_clauses()[clause][tuple]) == ChosenVariable) {
                        occurred = true;    //the variable occurs in this clause..
                        if (std::get<1>(ClonedInstance.get_clauses()[clause][tuple]) == false) {
                            satisfied = true;    //..and also satisfies the clause
                        }
                    }
                }
                if (occurred and satisfied) {
                    ClonedInstance.delete_clause(clause);
                }
                else if (occurred) {
                    ClonedInstance.delete_literal(clause, ChosenVariable);
                }
            }
            DifferentPaths.push(ClonedInstance);
            for (int clause = 0; clause < current_instance.get_clauses().size(); clause++) {    //same but set variable to true
                bool satisfied = false;
                bool occurred = false;
                for (int tuple = 0; tuple < current_instance.get_clauses()[clause].size(); tuple++) {
                    if (std::get<0>(current_instance.get_clauses()[clause][tuple]) == ChosenVariable) {
                        occurred = true;
                        if (std::get<1>(current_instance.get_clauses()[clause][tuple])) {
                            satisfied = true;
                        }
                    }
                }
                if (occurred and satisfied) {
                    current_instance.delete_clause(clause);
                }
                else if (occurred) {
                    current_instance.delete_literal(clause, ChosenVariable);
                }
            }
        }
        return false;   //we tried everything :(
    }
}