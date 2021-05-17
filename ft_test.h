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
# include <unistd.h>
# include <fcntl.h>
# include <sys/mman.h>

# ifndef FT_TEST_DEBUG
#  define FTT(x) __________ftt_##x
# else
#  define FTT(x) ftt_##x
# endif

# ifndef FT_OUTPUT_MAXSIZE
#  define FT_OUTPUT_MAXSIZE 512
# endif

typedef struct	FTT(test_s)
{
	struct FTT(test_s) *next;
	const char *name;
	const char *file;
	void (*run)();
}				FTT(test_t);

typedef struct	FTT(options_s)
{
	const char	*program_name;

	// Logging options
	int			help_only;
	int			list_tests;

	// Running options
	int			verbose;
	int			exit_first;
	int			run_all;

	// Tests to run
	FTT(test_t)	*tests;
}				FTT(options_t);

extern int FTT(test_failed);

extern FTT(test_t) *FTT(tests);
extern FTT(test_t) *FTT(current_test);

extern FTT(options_t) FTT(options);

void	FTT(test_register)(const char *name, const char *file, void (*test)());

/*
 *
 *   TESTS
 *
 */

# define FT_TEST(test_name) \
	void FTT(test_case__##test_name)();\
	void __attribute__((constructor)) FTT(construct_##test_name)() {\
		FTT(test_register)(#test_name, __FILE__, FTT(test_case__##test_name));\
	}\
	void FTT(test_case__##test_name)()

# define FT_TRUE(condition)\
	do {\
		if (!(condition)) {\
			printf("Error on test %s: (%s) should be TRUE\n", FTT(current_test)->name, #condition);\
			FTT(test_failed) = 1;\
			if (!FTT(options).run_all) return;\
		}\
	} while (0);


# define FT_FALSE(condition)\
	do {\
		if ((condition)) {\
			printf("Error on test %s: (%s) should be FALSE\n", FTT(current_test)->name, #condition);\
			FTT(test_failed) = 1;\
			if (!FTT(options).run_all) return;\
		}\
	} while (0);

# define ___FTT_INTERNAL_RUN_CHECK(type_name, a, check, b, oposite_check, ...)\
	do {\
		if (FTT(comp_##type_name)((a), (b), ##__VA_ARGS__) oposite_check 0) {\
			printf("[%s]: KO: expected %s " #check " %s, but ", FTT(current_test)->name, #a, #b);\
			FTT(print_##type_name)((a), ##__VA_ARGS__);\
			printf(" " #oposite_check " ");\
			FTT(print_##type_name)((b), ##__VA_ARGS__);\
			printf("\n");\
			FTT(test_failed) = 1;\
			if (!FTT(options).run_all) return;\
		} else if (FTT(options).verbose) {\
			printf("[%s]: OK: expected %s " #check " %s, and ", FTT(current_test)->name, #a, #b);\
			FTT(print_##type_name)((a), ##__VA_ARGS__);\
			printf(" " #check " ");\
			FTT(print_##type_name)((b), ##__VA_ARGS__);\
			printf("\n");\
		}\
	} while (0);

# define FT_EQ(type_name, a, b, ...) ___FTT_INTERNAL_RUN_CHECK(type_name, a, ==, b, !=, ##__VA_ARGS__)
# define FT_EQUALS(...) FT_EQ(__VA_ARGS__)

# define FT_NEQ(type_name, a, b, ...) ___FTT_INTERNAL_RUN_CHECK(type_name, a, !=, b, ==, ##__VA_ARGS__)
# define FT_NOT_EQUALS(...) FT_NEQ(__VA_ARGS__)

# define FT_LT(type_name, a, b, ...) ___FTT_INTERNAL_RUN_CHECK(type_name, a, <, b, >=, ##__VA_ARGS__)
# define FT_LESS_THAN(...) FT_LT(__VA_ARGS__)

# define FT_LE(type_name, a, b, ...) ___FTT_INTERNAL_RUN_CHECK(type_name, a, <=, b, >, ##__VA_ARGS__)
# define FT_LESS_EQUAL(...) FT_LE(__VA_ARGS__)

# define FT_GT(type_name, a, b, ...) ___FTT_INTERNAL_RUN_CHECK(type_name, a, >, b, <=, ##__VA_ARGS__)
# define FT_GREATER_THAN(...) FT_GT(__VA_ARGS__)

# define FT_GE(type_name, a, b, ...) ___FTT_INTERNAL_RUN_CHECK(type_name, a, >=, b, <, ##__VA_ARGS__)
# define FT_GREATER_EQUAL(...) FT_GT(__VA_ARGS__)

/*
 *
 *   I/O CHECKS
 *
 */

# define FTT_STR_IMPL(x) #x
# define FTT_STR(x) FTT_STR_IMPL(x)

# ifdef linux_
#  define __FTT_INTERNAL_GET_TEMPFILE(name) open("/dev/shm" name, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU)
#  define __FTT_INTERNAL_CLEAN_TEMPFILE(name) unlink("/dev/shm" name)
#  define __FT_OUTPUT_IMPL(statement1, statement2, file, line)\
	do {\
		fflush(stdout);\
		int failed = 0;\
		int backup = dup(1);\
		int fd1 = __FTT_INTERNAL_GET_TEMPFILE("/__ft_test_" file FTT_STR(line) "_1");\
		dup2(fd1, 1);\
		statement1;\
		fflush(stdout);\
		int fd2 = __FTT_INTERNAL_GET_TEMPFILE("/__ft_test_" file FTT_STR(line) "_2");\
		dup2(fd2, 1);\
		statement2;\
		fflush(stdout);\
		dup2(backup, 1);\
		close(backup);\
		if (FTT(comp_fd)(fd1, fd2) != 0) {\
			printf("[%s]: KO: expected output of [ %s ] == output [ %s ], but ", FTT(current_test)->name, #statement1, #statement2);\
			FTT(print_fd)(fd1);\
			printf(" != ");\
			FTT(print_fd)(fd2);\
			printf("\n");\
			FTT(test_failed) = 1;\
			failed = 1;\
		} else if (FTT(options).verbose) {\
			printf("[%s]: OK: expected output of [ %s ] == output [ %s ], and ", FTT(current_test)->name, #statement1, #statement2);\
			FTT(print_fd)(fd1);\
			printf(" == ");\
			FTT(print_fd)(fd2);\
			printf("\n");\
		}\
		close(fd1);\
		close(fd2);\
		__FTT_INTERNAL_CLEAN_TEMPFILE("/__ft_test_" file FTT_STR(line) "_1");\
		__FTT_INTERNAL_CLEAN_TEMPFILE("/__ft_test_" file FTT_STR(line) "_2");\
		if (failed && !FTT(options).run_all) return;\
	} while(0)

# else
#  define __FT_OUTPUT_IMPL(statement1, statement2, file, line)\
	do {\
		int failed = 0;\
		int p1[2];\
		int p2[2];\
		int buffer1[FT_OUTPUT_MAXSIZE] = {0};\
		int buffer2[FT_OUTPUT_MAXSIZE] = {0};\
		ssize_t read1 = 0;\
		ssize_t read2 = 0;\
		pipe(p1);\
		pipe(p2);\
		fflush(stdout);\
		if (!fork()) {\
			dup2(p1[1], 1);\
			statement1;\
			fflush(stdout);\
			close(p1[1]);\
			exit(0);\
		} else if (!fork()) {\
			dup2(p2[1], 1);\
			statement2;\
			fflush(stdout);\
			close(p2[1]);\
			exit(0);\
		}\
		read1 = read(p1[0], buffer1, FT_OUTPUT_MAXSIZE);\
		close(p1[0]);\
		read2 = read(p2[0], buffer2, FT_OUTPUT_MAXSIZE);\
		close(p2[0]);\
		if (read1 != read2 || FTT(comp_buffer)(buffer1, buffer2, read1) != 0) {\
			printf("[%s]: KO: expected output of [ %s ] == output [ %s ], but ", FTT(current_test)->name, #statement1, #statement2);\
			FTT(print_buffer)(buffer1, read1);\
			printf(" != ");\
			FTT(print_buffer)(buffer2, read2);\
			printf("\n");\
			FTT(test_failed) = 1;\
			failed = 1;\
		} else if (FTT(options).verbose) {\
			printf("[%s]: OK: expected output of [ %s ] == output [ %s ], and ", FTT(current_test)->name, #statement1, #statement2);\
			FTT(print_buffer)(buffer1, read1);\
			printf(" == ");\
			FTT(print_buffer)(buffer2, read2);\
			printf("\n");\
		}\
		if (failed && !FTT(options).run_all) return;\
	} while(0)

# endif



# define FT_OUTPUT(statement, expected)\
	__FT_OUTPUT_IMPL(statement, expected, __FILE__, __LINE__)

# define FT_INPUT(printer, reader)\
	do {\
		fflush(stdout);\
		int p[2];\
		pipe(p);\
		if (fork()) {\
			int backup = dup(0);\
			dup2(p[0], 0);\
			reader;\
			dup2(backup, 0);\
			close(backup);\
			close(p[0]);\
		} else {\
			int backup = dup(1);\
			fflush(stdout);\
			dup2(p[1], 1);\
			printer;\
			fflush(stdout);\
			dup2(backup, 1);\
			close(p[1]);\
			exit(0);\
		}\
	} while(0)

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
FT_TYPE(fd);

# ifdef FT_TEST_MAIN

int FTT(test_failed) = 0;
FTT(test_t) *FTT(tests) = 0;
FTT(test_t) *FTT(current_test) = 0;
FTT(test_t) **FTT(register_handle) = &FTT(tests);
FTT(options_t) FTT(options) = {0};

FTT(test_t)	*FTT(test_copy)(FTT(test_t) *t);

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
						case 'l':
							FTT(options).list_tests = 1;
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
				else if (strcmp(opt, "list") == 0)
				{
					FTT(options).list_tests = 1;
				}
			}
		}
		else
		{
			static FTT(test_t) **to_run_register_handle = &FTT(options).tests;
			int found = 0;
			for (FTT(test_t) *test = FTT(tests); test != 0; test = test->next)
			{
				if (strcmp(opt, test->name) == 0)
				{
					*to_run_register_handle = FTT(test_copy)(test);
					to_run_register_handle = &(*to_run_register_handle)->next;
					found = 1;
					break;
				}
			}
			if (!found) {
				printf("Error: test %s not found. Run with -l to list available tests\n", opt);
				exit(-1);
			}
		}
	}

	if (FTT(options).tests == 0)
		FTT(options).tests = FTT(tests);
}

void FTT(show_help)()
{
	printf(
		"Usage: %s [OPTIONS...] [TESTS...]\n"
		"\n"
		"OPTIONS:\n"
		"	-v, --verbose       Shows information about every assertion\n"
		"	-x, --exitfirst     Exits on first assertion failed\n"
		"	-a, --all           Runs every assertion\n"
		"	-h, --help          Shows this message\n"
		"\n"
		"TESTS:\n"
		"	Tests to run\n"
		"\n"
		"\n"
		"ft_test.h: made with <3 by Eduardo \"emendes-\" Mosko at 42saopaulo\n"
	, FTT(options).program_name);
}

void FTT(list_tests)()
{
	puts("Tests found:");
	for (FTT(test_t) *test = FTT(tests); test != 0; test = test->next)
	{
		printf(
		"	%-19s %s\n", test->name, test->file);
	}
}

int main(int argc, char **argv) {
	FTT(argparser)(argc, argv);

	if (FTT(options).help_only)
	{
		FTT(show_help)();
	}
	else if (FTT(options).list_tests)
	{
		FTT(list_tests)();
	}
	else
	{
		for (FTT(current_test) = FTT(options).tests;
				FTT(current_test) != 0; FTT(current_test) = FTT(current_test)->next)
		{
			FTT(current_test)->run();

			if (FTT(options).exit_first && FTT(test_failed))
				break;
		}

		if (!FTT(test_failed))
			printf("OK\n");
	}
	return FTT(test_failed);
}

FTT(test_t)	*FTT(test_new)(const char *name, const char *file, void (*test)()) {
	FTT(test_t) *ret = malloc(sizeof(FTT(test_t)));
	if (ret == NULL)
		exit(-1);
	ret->next = NULL;
	ret->name = name;
	ret->file = file;
	ret->run = test;
	return (ret);
}

FTT(test_t)	*FTT(test_copy)(FTT(test_t) *t) {
	return FTT(test_new)(t->name, t->file, t->run);
}

void	FTT(test_register)(const char *name, const char *file, void (*test)()) {
	*FTT(register_handle) = FTT(test_new)(name, file, test);
	FTT(register_handle) = &(*FTT(register_handle))->next;
}

void FTT(print_escaped_buffer)(char *buffer, size_t size)
{
	for (size_t i = 0; i < size; ++i) {
		if (isprint(*buffer))
			printf("%c", *buffer);
		else
		{
			printf("\e[1;29m");
			switch (*buffer)
			{
				case '\a': printf("\\a"); break;
				case '\b': printf("\\b"); break;
				case '\f': printf("\\f"); break;
				case '\n': printf("\\n"); break;
				case '\r': printf("\\r"); break;
				case '\t': printf("\\t"); break;
				case '\v': printf("\\v"); break;
				case '\\': printf("\\\\"); break;
				case '\'': printf("\\'"); break;
				case '\"': printf("\\\""); break;
				case '\?': printf("\\?"); break;
				case '\e': printf("\\e"); break;
				default: printf("\\%03o", *(unsigned char*)buffer); break;
			}
			printf("\e[0m");
		}
		++buffer;
	}
}

FT_TEST_REGISTER_TYPE_LMBD_(int , int  , { printf("%i" ,  int_); }, { return  int1 -  int2; });
FT_TEST_REGISTER_TYPE_LMBD_(long, long , { printf("%li", long_); }, { return long1 - long2; });
FT_TEST_REGISTER_TYPE_LMBD (ptr , void*, { printf("%p" ,  ptr ); }, { return  ptr1 -  ptr2; });

FT_TEST_REGISTER_TYPE_LMBD (uint , unsigned  int, { printf( "%u", uint ); }, { return  uint1 -  uint2; });
FT_TEST_REGISTER_TYPE_LMBD (ulong, unsigned long, { printf("%lu", ulong); }, { return ulong1 - ulong2; });

FT_TEST_REGISTER_TYPE_LMBD (str, char*, {
			printf("\e[1;29m\"");
			FTT(print_escaped_buffer)(str, strlen(str));
			printf("\"\e[0m");
		}, { return strcmp(str1, str2); });

FT_TEST_REGISTER_TYPE_LAMBDA(buffer, void*,
		(void *buffer, size_t size) {
			printf("[");
			FTT(print_escaped_buffer)(buffer, size);
			printf("]");
		}, (void *b1, void *b2, size_t size) { return memcmp(b1, b2, size); });

FT_TEST_REGISTER_TYPE_LMBD(fd, int,
		{
			char buffer[128];
			ssize_t bytes_read;
			lseek(fd, 0, SEEK_SET);
			printf("{");
			while ((bytes_read = read(fd, buffer, 128)) > 0)
			{
				FTT(print_escaped_buffer)(buffer, bytes_read);
			}
			printf("}");
		},
		{
			char buffer1[128];
			char buffer2[128];
			ssize_t br1;
			ssize_t br2;
			int ret;
			lseek(fd1, 0, SEEK_SET);
			lseek(fd2, 0, SEEK_SET);
			while (1)
			{
				br1 = read(fd1, buffer1, 128);
				br2 = read(fd2, buffer2, 128);
				if (br1 != br2)
					return (br1 - br2);
				if (br1 > 0 && (ret = memcmp(buffer1, buffer2, br1)) != 0)
					return (ret);
				else
					break;
			}
			return (0);
		});

# endif

#endif
