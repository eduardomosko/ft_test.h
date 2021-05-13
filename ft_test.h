/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_test.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emendes- <emendes-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 18:50:43 by emendes-          #+#    #+#             */
/*   Updated: 2021/05/13 19:57:48 by emendes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TEST_H
# define FT_TEST_H

# include <stdio.h>
# include <stdlib.h>

# ifndef FT_TEST_MAX_TESTS
#  define FT_TEST_MAX_TESTS 1024
# endif

# define FTT(x) __________ftt_##x

typedef void (*FTT(test_t))();

extern int FTT(test_failed);
extern FTT(test_t) FTT(tests)[];
extern const char *FTT(current_test);

# define FT_TEST(test_name) \
	void FTT(test_case__##test_name)();\
	void FTT(test_case__##test_name##_runner)();\
	void __attribute__((constructor)) FTT(register_##test_name)() {\
		int i;\
		for (i = 0; FTT(tests)[i] != 0; ++i);\
		FTT(tests)[i] = FTT(test_case__##test_name##_runner);\
	}\
	void FTT(test_case__##test_name##_runner)() {\
		FTT(current_test) = #test_name;\
		FTT(test_case__##test_name)();\
	}\
	void FTT(test_case__##test_name)()


# define FT_TRUE(condition)\
	do {\
		if (!(condition)) {\
			printf("Error on test %s: (%s) should be TRUE\n", FTT(current_test), #condition);\
			ftt__failed = 1;\
			return;\
		}\
	} while (0);


# define FT_FALSE(condition)\
	do {\
		if ((condition)) {\
			printf("Error on test %s: (%s) should be FALSE\n", FTT(current_test), #condition);\
			FTT(failed) = 1;\
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
			printf("Error on test %s: expected %s == %s, but ", FTT(current_test), #a, #b);\
			FTT(print_##type_name)((a));\
			printf(" != ");\
			FTT(print_##type_name)((b));\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);

# define FT_EQUALS(...) FT_EQ(__VA_ARGS__)

# ifdef FT_TEST_MAIN

int FTT(failed) = 0;
const char *FTT(current_test) = "None";
FTT(test_t) FTT(tests)[FT_TEST_MAX_TESTS] = {0};\

int main() {
	for (int i = 0; FTT(tests)[i] != 0; ++i) {\
		FTT(tests)[i]();
	}

	if (!FTT(test_failed))
		printf("OK\n");

	return FTT(test_failed);
}

# endif

#endif
