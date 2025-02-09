.PHONY: test clang gcc

FLAGS =

test: clang gcc

clang: test.clang.out
	-./test.clang.out $(FLAGS)

gcc: test.gcc.out
	-./test.gcc.out $(FLAGS)

test.clang.out: ft_test.h tests/*.c
	@clang -I. tests/*.c -Wall -Werror -Wextra -o $@

test.gcc.out: ft_test.h tests/*.c
	@gcc -I. tests/*.c -Wall -Werror -Wextra -o $@
