.PHONY: gcc_test clang_test test all

all: test

gcc_test:
	nix run nixpkgs#gcc13 -- test.cpp -std=c++20 -pedantic

clang_test:
	nix run nixpkgs#clang_12 -- test.cpp -std=c++20 -pedantic

test: clang_test gcc_test
	rm -f a.out


