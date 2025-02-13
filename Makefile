.PHONY: test clang gcc tcc

FLAGS =

test: clang gcc

test-all: clang gcc tcc

clang: test.clang.out
	-./test.clang.out $(FLAGS)

gcc: test.gcc.out
	-./test.gcc.out $(FLAGS)

tcc: test.tcc.out
	-./test.tcc.out $(FLAGS)

test.clang.out: ft_test.h tests/*.c
	@clang -I. tests/*.c -Wall -Werror -Wextra -o $@

test.gcc.out: ft_test.h tests/*.c
	@gcc -I. tests/*.c -Wall -Werror -Wextra -o $@

test.tcc.out: ./tcc-compiler/tcc ft_test.h tests/*.c
	@./tcc-compiler/tcc -I./tcc-compiler/include -L./tcc-compiler tests/*.c -Wall -Werror -Wextra -DFT_ALLOW_WARNINGS -o $@


./tcc-compiler/tcc: ./tcc-compiler/configure
	cd ./tcc-compiler; ./configure
	cd ./tcc-compiler; make

./tcc-compiler/configure:
	mkdir -p ./tcc-compiler
	cd ./tcc-compiler; git init
	cd ./tcc-compiler; git remote add origin https://github.com/TinyCC/tinycc.git
	cd ./tcc-compiler; git fetch --depth 1 origin f6385c05308f715bdd2c06336801193a21d69b50
	cd ./tcc-compiler; git checkout FETCH_HEAD

