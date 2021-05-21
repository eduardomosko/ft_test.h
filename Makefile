.PHONY: test

test: test.clang test.gcc
	./test.clang -vva
	./test.gcc -vva

test.clang: ft_test.h tests/*.c
	@clang tests/*.c -o test.clang

test.gcc: ft_test.h tests/*.c
	@gcc tests/*.c -o test.gcc
