# Programmierpraktikum_backtracking
Die finale Version hat die Commit Message "final version" und ist der neueste Commit auf dem Branch "store_whole_instances_version".
Auf dem Branch "Different_DifferenntPaths" ist eine alternative A-SAT Version, die etwas speichereffizienter ist, jedoch nicht schneller, da sie immer noch zu viel Speicher braucht.
Ich habe es jetzt aber dabei belassen, da wir ja Breitensuche machen wollen, mit Tiefensuche wie bei C-SAT, wäre der Algorithmus sicher schneller.
Bei C-SAT kann man die Bedingung wann wir "choose_variable" aufrufen und wann "local_processing" ändern, siehe Paper.

Die Compile-Befehle die ich benutze:
clang++ -std=c++20 -I./include src/slimSAT.cpp src/main.cpp src/CSAT.cpp -o csat -Wall -Werror -O3

clang++ -std=c++20 -I./include src/slimSAT.cpp src/main.cpp src/ASAT.cpp -o asat -Wall -Werror -O3
hier muss man aber auch die main.cpp ändern.
