#!/usr/bin/env bash

g++ -std=gnu++17 -fdiagnostics-color=always test17.cpp -o test17 2>&1 |less
#-std=gnu++2a -Ofast -march=native -Wall -Wextra -fwhole-program -DRANGES_DISABLE_DEPRECATED_WARNINGS
g++ -std=gnu++2b -Ofast -march=native -Wall -Wextra -fwhole-program -fdiagnostics-color=always -lsnitch test.cpp -o test 2>&1 |less
./test 2>&1 |less

#g++ -std=gnu++23 -I. -Icomcon -fdiagnostics-color=always comcon/test.cpp -o comcon_test |less
#./concon_test

