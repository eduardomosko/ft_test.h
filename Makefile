.PHONY: test

test: test.out
	@./test.out

test.out: ft_test.h tests/*.c
	@gcc tests/*.c -o test.out
