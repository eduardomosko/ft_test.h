/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_test.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emendes- <emendes-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 18:50:43 by emendes-          #+#    #+#             */
/*   Updated: 2021/05/15 13:22:53 by emendes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TEST_H
# define FT_TEST_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>

# define FT_TEST_DEBUG
# ifndef FT_TEST_DEBUG
#  define FTT(x) __________ftt_##x
# else
#  define FTT(x) ftt_##x
# endif

typedef struct	FTT(test_s)
{
		struct FTT(test_s) *next;
	const char *name;
	void (*run)();
}				FTT(test_t);

typedef struct	FTT(options_s)
{
	const char	*program_name;

	int			verbose;
	int			help_only;
	int			exit_first;
	int			run_all;

}				FTT(options_t);

extern int FTT(test_failed);
extern const char *FTT(current_test);

extern FTT(test_t) *FTT(tests);
extern FTT(options_t) FTT(options);

void	FTT(test_register)(const char *name, void (*test)());

/*
 *
 *   TESTS
 *
 */

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

# define FT_EQ(type_name, a, b, ...)\
	do {\
		if (FTT(comp_##type_name)((a), (b), ##__VA_ARGS__) != 0) {\
			printf("Error on test %s: expected %s == %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a), ##__VA_ARGS__);\
			printf(" != ");\
			FTT(print_##type_name)((b), ##__VA_ARGS__);\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_EQUALS(...) FT_EQ(__VA_ARGS__)

# define FT_NEQ(type_name, a, b, ...)\
	do {\
		if (FTT(comp_##type_name)((a), (b), ##__VA_ARGS__) == 0) {\
			printf("Error on test %s: expected %s != %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a), ##__VA_ARGS__);\
			printf(" == ");\
			FTT(print_##type_name)((b), ##__VA_ARGS__);\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_NOT_EQUALS(...) FT_NEQ(__VA_ARGS__)

# define FT_LT(type_name, a, b, ...)\
	do {\
		if (FTT(comp_##type_name)((a), (b), ##__VA_ARGS__) < 0) {\
			printf("Error on test %s: expected %s < %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a), ##__VA_ARGS__);\
			printf(" >= ");\
			FTT(print_##type_name)((b), ##__VA_ARGS__);\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_LESS_THAN(...) FT_LT(__VA_ARGS__)

# define FT_LE(type_name, a, b, ...)\
	do {\
		if (FTT(comp_##type_name)((a), (b), ##__VA_ARGS__) <= 0) {\
			printf("Error on test %s: expected %s <= %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a), ##__VA_ARGS__);\
			printf(" > ");\
			FTT(print_##type_name)((b), ##__VA_ARGS__);\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_LESS_EQUAL(...) FT_LE(__VA_ARGS__)

# define FT_GT(type_name, a, b, ...)\
	do {\
		if (FTT(comp_##type_name)((a), (b), ##__VA_ARGS__) > 0) {\
			printf("Error on test %s: expected %s > %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a), ##__VA_ARGS__);\
			printf(" <= ");\
			FTT(print_##type_name)((b), ##__VA_ARGS__);\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_GREATER_THAN(...) FT_GT(__VA_ARGS__)

# define FT_GE(type_name, a, b, ...)\
	do {\
		if (FTT(comp_##type_name)((a), (b), ##__VA_ARGS__) >= 0) {\
			printf("Error on test %s: expected %s >= %s, but ", FTT(tests)->name, #a, #b);\
			FTT(print_##type_name)((a), ##__VA_ARGS__);\
			printf(" < ");\
			FTT(print_##type_name)((b), ##__VA_ARGS__);\
			printf("\n");\
			FTT(test_failed) = 1;\
			return;\
		}\
	} while (0);
# define FT_GREATER_EQUAL(...) FT_GT(__VA_ARGS__)

/*
 *
 *   TYPES
 *
 */

# define FT_TEST_REGISTER_TYPE_LAMBDA(type_name, type, printfn, compfn)\
	void FTT(print_##type_name) printfn;\
	int FTT(comp_##type_name) compfn

# define FT_TEST_REGISTER_TYPE_LMBD(type_name, type, printfn, compfn)\
	FT_TEST_REGISTER_TYPE_LAMBDA(type_name, type,\
			(type type_name) printfn,\
			(type type_name##1, type type_name##2 ) compfn)

# define FT_TEST_REGISTER_TYPE_LMBD_(type_name, type, printfn, compfn)\
	FT_TEST_REGISTER_TYPE_LAMBDA(type_name, type,\
			(type type_name##_) printfn,\
			(type type_name##1, type type_name##2 ) compfn)

# define FT_TEST_REGISTER_TYPE(type_name, type, printfn, compfn)\
	FT_TEST_REGISTER_TYPE_LAMBDA(type_name, type,\
			(type a)         { printfn(a);          },\
			(type a, type b) { return compfn(a, b); })

#define FT_TEST_TYPE_ALIAS(type_name, type_target, type)\
	FT_TEST_REGISTER_TYPE(type_name, type, print_##type_target, comp_##type_target);

#define FT_TYPE(type_name)\
	int FTT(comp_##type_name)();\
	void FTT(print_##type_name)()

FT_TYPE(int);
FT_TYPE(long);
FT_TYPE(ptr);
FT_TYPE(uint);
FT_TYPE(ulong);
FT_TYPE(str);
FT_TYPE(buffer);

# ifdef FT_TEST_MAIN

int FTT(test_failed) = 0;
FTT(test_t) *FTT(tests) = 0;
FTT(test_t) **FTT(register_handle) = &FTT(tests);
FTT(options_t) FTT(options) = {0};

void FTT(argparser)(int argc, char **argv)
{
	FTT(options).program_name = *argv;

	// Check every argument
	for (int i = 1; i < argc; ++i)
	{
		char *opt = argv[i];

		if (*opt == '-')
		{
			++opt;
			if (*opt != '-')
			{
				for (; *opt; ++opt)
				{
					switch (*opt) {
						case 'v':
							FTT(options).verbose += 1;
							break;
						case 'x':
							FTT(options).exit_first = 1;
							FTT(options).run_all = 0;
							break;
						case 'a':
							FTT(options).exit_first = 0;
							FTT(options).run_all = 1;
							break;
						case 'h':
							FTT(options).help_only = 1;
							break;
					}
				}
			}
			else
			{
				++opt;
				if (strcmp(opt, "verbose") == 0)
				{
					FTT(options).verbose += 1;
				}
				else if (strcmp(opt, "exitfirst") == 0)
				{
					FTT(options).exit_first = 1;
					FTT(options).run_all = 0;
				}
				else if (strcmp(opt, "all") == 0)
				{
					FTT(options).exit_first = 0;
					FTT(options).run_all = 1;
				}
				else if (strcmp(opt, "help") == 0)
				{
					FTT(options).help_only = 1;
				}
			}
		}
	}
}

void FTT(show_help)()
{
	char *message = "Usage";
	puts(message);
}

int main(int argc, char **argv) {
	FTT(argparser)(argc, argv);

	if (FTT(options).help_only)
	{
		FTT(show_help)();
	}
	else
	{
		for (; FTT(tests) != 0; FTT(tests) = FTT(tests)->next)
		{
			FTT(tests)->run();
		}

		if (!FTT(test_failed))
			printf("OK\n");
	}
	return FTT(test_failed);
}


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
	FTT(register_handle) = &(*FTT(register_handle))->next;
}

FT_TEST_REGISTER_TYPE_LMBD_(int , int  , { printf("%i" ,  int_); }, { return  int1 -  int2; });
FT_TEST_REGISTER_TYPE_LMBD_(long, long , { printf("%li", long_); }, { return long1 - long2; });
FT_TEST_REGISTER_TYPE_LMBD (ptr , void*, { printf("%p" ,  ptr ); }, { return  ptr1 -  ptr2; });

FT_TEST_REGISTER_TYPE_LMBD (uint , unsigned  int, { printf( "%u", uint ); }, { return  uint1 -  uint2; });
FT_TEST_REGISTER_TYPE_LMBD (ulong, unsigned long, { printf("%lu", ulong); }, { return ulong1 - ulong2; });

FT_TEST_REGISTER_TYPE_LMBD (str, char*, { printf("\"%s\"", str); }, { return strcmp(str1, str2); });

FT_TEST_REGISTER_TYPE_LAMBDA(buffer, void*,
		(char *buffer, size_t size) {
			printf("[");
			for (size_t i = 0; i < size; ++i) {
				if (isprint(*buffer))
					printf("%c", *buffer);
				else
					printf("\e[1;29m\\%u\e[0m", *(unsigned char*)buffer);
				++buffer;
			}
			printf("]");
		}, (void *b1, void *b2, size_t size) { return memcmp(b1, b2, size); });

# endif

#endif
