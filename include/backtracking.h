//
// Created by Pascal Kessler on 28.04.23.
//

#ifndef PROGRAMMIERPRAKTIKUM_BACKTRACKING_H
#define PROGRAMMIERPRAKTIKUM_BACKTRACKING_H

#include "SAT.h"

std::pair<bool,SAT> backtracking(SAT & instance);
bool backtracking_non_recursive(SAT & instance);

#endif //PROGRAMMIERPRAKTIKUM_BACKTRACKING_H
