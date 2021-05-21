.PHONY: test

test: test.clang test.gcc
	./test.clang -vva
	./test.gcc -vva

test.clang: ft_test.h tests/*.c
	@clang -I. tests/*.c -o test.clang

test.gcc: ft_test.h tests/*.c
	@gcc -I. tests/*.c -o test.gcc
