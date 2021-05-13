/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_test.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emendes- <emendes-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 18:50:43 by emendes-          #+#    #+#             */
/*   Updated: 2021/05/12 21:36:58 by emendes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TEST_H
# define FT_TEST_H

# include <stdio.h>
# include <stdlib.h>

# ifndef FT_TEST_MAX_TESTS
#  define FT_TEST_MAX_TESTS 1024
# endif

typedef void (*ftt_test_t)();

extern int ftt__failed;
extern ftt_test_t ftt__tests[];
extern const char *ftt__current_test;

# define FT_TEST(test_name) \
	void ftt__##test_name();\
	void ftt__##test_name##_runner();\
	void __attribute__((constructor)) ftt__register_##test_name() {\
		int i;\
		for (i = 0; ftt__tests[i] != 0; ++i);\
		ftt__tests[i] = ftt__##test_name##_runner;\
	}\
	void ftt__##test_name##_runner() {\
		ftt__current_test = #test_name;\
		ftt__##test_name();\
	}\
	void ftt__##test_name()

# define FT_TEST_MAIN \
	int ftt__failed = 0;\
	const char *ftt__current_test = "None";\
	ftt_test_t ftt__tests[FT_TEST_MAX_TESTS] = {0};\
	int main() {\
		for (int i = 0; ftt__tests[i] != 0; ++i) {\
			ftt__tests[i]();\
		}\
		if (!ftt__failed)\
			printf("OK\n");\
		return ftt__failed;\
	}

# define FT_TRUE(condition)\
	do {\
		if (!(condition)) {\
			printf("Error on test %s: (%s) should be TRUE\n", ftt__current_test, #condition);\
			ftt__failed = 1;\
			return;\
		}\
	} while (0);


# define FT_FALSE(condition)\
	do {\
		if ((condition)) {\
			printf("Error on test %s: (%s) should be FALSE\n", ftt__current_test, #condition);\
			ftt__failed = 1;\
			return;\
		}\
	} while (0);


# define FT_OUTPUT(print, string_expect)


# define FT_TEST_REGISTER_TYPE_LAMBDA(type_name, type, printfn, compfn)\
	int ftt__comp_##type_name compfn\
	void ftt__print_##type_name printfn

# define FT_TEST_REGISTER_TYPE(type_name, type, printfn, compfn)\
	FT_TEST_REGISTER_TYPE_LAMBDA(type_name, type,\
			(type a)         { printfn(a);          },\
			(type a, type b) { return compfn(a, b); })\


# define FT_EQ(type_name, a, b)\
	do {\
		if (ftt__comp_##type_name((a), (b)) != 0) {\
			printf("Error on test %s: expected %s == %s, but ", ftt__current_test, #a, #b);\
			ftt__print_##type_name((a));\
			printf(" != ");\
			ftt__print_##type_name((b));\
			printf("\n");\
			ftt__failed = 1;\
			return;\
		}\
	} while (0);

# define FT_EQUALS(...) FT_EQ(__VA_ARGS__)

#endif
