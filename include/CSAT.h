//
// Created by Pascal Kessler on 30.05.23.
//

#ifndef PROGRAMMIERPRAKTIKUM_CSAT_H
#define PROGRAMMIERPRAKTIKUM_CSAT_H

enum class LocalProcessing;

std::pair<int,bool> choose_variable(SAT & instance, const std::vector<const double> & SizeFactor);
bool unit_propagation(SAT & instance);
LocalProcessing local_processing(SAT & instance, const std::vector<const double> & SizeFactor);
bool c_sat(SAT & instance);

#endif //PROGRAMMIERPRAKTIKUM_CSAT_H
