.PHONY: test

test: test.clang.out test.gcc.out
	./test.clang.out -vva
	./test.gcc.out -vva

test.clang.out: ft_test.h tests/*.c
	@clang -I. tests/*.c -Wall -Werror -Wextra -o $@

test.gcc.out: ft_test.h tests/*.c
	@gcc -I. tests/*.c -Wall -Werror -Wextra -o $@
