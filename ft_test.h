/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_test.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emendes- <emendes-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 18:50:43 by emendes-          #+#    #+#             */
/*   Updated: 2021/05/13 20:31:06 by emendes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TEST_H
# define FT_TEST_H

# include <stdio.h>
# include <stdlib.h>

# define FTT(x) __________ftt_##x

typedef struct	FTT(test_s)
{
	struct FTT(test_s) *next;
	const char *name;
	void (*run)();
}				FTT(test_t);

extern int FTT(test_failed);
extern const char *FTT(current_test);

extern FTT(test_t) *FTT(tests);

void	FTT(register_test)(const char *name, void (*test)());

# define FT_TEST(test_name) \
	void FTT(test_case__##test_name)();\
	void __attribute__((constructor)) FTT(construct_##test_name)() {\
		FTT(test_register)(#test_name, FTT(test_case__##test_name));\
	}\
	void FTT(test_case__##test_name)()


# define FT_TRUE(condition)\
	do {\
		if (!(condition)) {\
			printf("Error on test %s: (%s) should be TRUE\n", FTT(tests)->name, #condition);\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);


# define FT_FALSE(condition)\
	do {\
		if ((condition)) {\
			printf("Error on test %s: (%s) should be FALSE\n", FTT(tests)->name, #condition);\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);


# define FT_OUTPUT(print, string_expect)


# define FT_TEST_REGISTER_TYPE_LAMBDA(type_name, type, printfn, compfn)\
	int FTT(comp_##type_name) compfn\
	void FTT(print_##type_name) printfn

# define FT_TEST_REGISTER_TYPE(type_name, type, printfn, compfn)\
	FT_TEST_REGISTER_TYPE_LAMBDA(type_name, type,\
			(type a)         { printfn(a);          },\
			(type a, type b) { return compfn(a, b); })\


# define FT_EQ(type_name, a, b)\
	do {\
		if (FTT(comp_##type_name)((a), (b)) != 0) {\
			printf("Error on test %s: expected %s == %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a));\
			printf(" != ");\
			FTT(print_##type_name)((b));\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_EQUALS(...) FT_EQ(__VA_ARGS__)

# define FT_LT(type_name, a, b)\
	do {\
		if (FTT(comp_##type_name)((a), (b)) < 0) {\
			printf("Error on test %s: expected %s < %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a));\
			printf(" >= ");\
			FTT(print_##type_name)((b));\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_LESS_THAN(...) FT_LT(__VA_ARGS__)

# define FT_LE(type_name, a, b)\
	do {\
		if (FTT(comp_##type_name)((a), (b)) <= 0) {\
			printf("Error on test %s: expected %s <= %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a));\
			printf(" > ");\
			FTT(print_##type_name)((b));\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_LESS_EQUAL(...) FT_LE(__VA_ARGS__)

# define FT_GT(type_name, a, b)\
	do {\
		if (FTT(comp_##type_name)((a), (b)) > 0) {\
			printf("Error on test %s: expected %s > %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a));\
			printf(" <= ");\
			FTT(print_##type_name)((b));\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_GREATER_THAN(...) FT_GT(__VA_ARGS__)

# define FT_GE(type_name, a, b)\
	do {\
		if (FTT(comp_##type_name)((a), (b)) >= 0) {\
			printf("Error on test %s: expected %s >= %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a));\
			printf(" < ");\
			FTT(print_##type_name)((b));\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_GREATER_EQUAL(...) FT_GT(__VA_ARGS__)

# ifdef FT_TEST_MAIN

int FTT(test_failed) = 0;
FTT(test_t) *FTT(tests) = 0;
FTT(test_t) **FTT(register_handle) = &FTT(tests);

FTT(test_t)	*FTT(test_new)(const char *name, void (*test)()) {
	FTT(test_t) *ret = malloc(sizeof(FTT(test_t)));
	if (ret == NULL)
		exit(-1);
	ret->next = NULL;
	ret->name = name;
	ret->run = test;
	return (ret);
}

void	FTT(test_register)(const char *name, void (*test)()) {
	*FTT(register_handle) = FTT(test_new)(name, test);
}

int main() {
	for (; FTT(tests) != 0; FTT(tests) = FTT(tests)->next) {
		FTT(tests)->run();
	}

	if (!FTT(test_failed))
		printf("OK\n");

	return FTT(test_failed);
}

# endif

#endif
