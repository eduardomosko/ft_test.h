/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_test.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By : emendes- <emendes-@student.42sp.org.br>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/12 18:50:43 by emendes-          #+#    #+#             */
/*   Updated: 2025/02/09 13:17:00 by emendes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_TEST_H
#define FT_TEST_H

#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#ifndef FT_ALLOW_WARNINGS
#	pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#ifndef FT_TEST_DEBUG
#	define FTT(x) __________ftt_##x
#else
#	define FTT(x) ftt_##x
#endif

#ifndef FT_TEST_CACHELINE
#	define FT_TEST_CACHELINE 64
#endif

typedef struct FTT(test_s) {
	struct FTT(test_s) * next;
	const char* name;
	const char* file;
	void (*run)();
} FTT(test_t);

typedef struct FTT(options_s) {
	const char* program_name;

	/* Logging options */
	int help_only;
	int list_tests;

	/* Running options */
	int verbose;
	int exit_first;
	int run_all;

	/* Tests to run */
	FTT(test_t) * tests;

} FTT(options_t);

/*
 * Mix of dinamic array and linked list which
 * occupies exactly a cache line, minimizing realocation
 * and cache misses
 */
#define FTT_LST_BUF_SIZE (FT_TEST_CACHELINE * sizeof(uint8_t) - sizeof(void*) - sizeof(uint8_t))
typedef struct FTT(lstarr_t) {
	uint8_t size;
	uint8_t buffer[FTT_LST_BUF_SIZE];

	struct FTT(lstarr_t) * next;
} FTT(lstarr_t);

void FTT(lstarr_read_fd)(int fd, FTT(lstarr_t) * a);

typedef char FTT(fixture_data__);

extern int FTT(test_failed);

extern FTT(test_t) * FTT(tests);
extern FTT(test_t) * FTT(current_test);

extern FTT(options_t) FTT(options);
extern int FTT(must_put_nl);

void FTT(test_register)(const char* name, const char* file, void (*test)());
FTT(fixture_data__) * FTT(fixture_setup__)(void);
void FTT(fixture_teardown__)(FTT(fixture_data__) * ftt);

FTT(lstarr_t) * FTT(lstarr_create)();
FTT(lstarr_t) * FTT(lstarr_append)(FTT(lstarr_t) * a);
void FTT(lstarr_destroy)(FTT(lstarr_t) * arr);
void FTT(lstarr_read_fd)(int fd, FTT(lstarr_t) * a);
void FTT(lstarr_print)(FTT(lstarr_t) * a);
void FTT(lstarr_print_escaped)(FTT(lstarr_t) * a);
int	 FTT(lstarr_comp)(FTT(lstarr_t) * a, FTT(lstarr_t) * b);

/*
 *
 *   TESTS CASES
 *
 */

#define FT_SETUP(fixture_name, data_t)                                                            \
	typedef struct FTT(fixture_data__##fixture_name) {                                            \
		data_t                                                                                    \
	} FTT(fixture_data__##fixture_name);                                                          \
	static void FTT(fixture_setup_impl__##fixture_name)(FTT(fixture_data__##fixture_name) * ftt); \
	static void FTT(fixture_teardown__##fixture_name)(FTT(fixture_data__##fixture_name) * ftt);   \
	static struct FTT(fixture_data__##fixture_name) * FTT(fixture_setup__##fixture_name)(void) {  \
		struct FTT(fixture_data__##fixture_name)* data = malloc(sizeof(*data));                   \
		FTT(fixture_setup_impl__##fixture_name)(data);                                            \
		return (data);                                                                            \
	}                                                                                             \
	static void FTT(fixture_setup_impl__##fixture_name)(FTT(fixture_data__##fixture_name) * ftt)

#define FT_TEST(test_name, fixture_name...)                                                                 \
	void							  FTT(test_case__##test_name)(FTT(fixture_data__##fixture_name) * ftt); \
	void							  FTT(test_runner__##test_name)();                                      \
	void __attribute__((constructor)) FTT(construct_##test_name)() {                                        \
		FTT(test_register)(#test_name, __FILE__, FTT(test_runner__##test_name));                            \
	}                                                                                                       \
	void FTT(test_runner__##test_name)(void) {                                                              \
		void* data = FTT(fixture_setup__##fixture_name)();                                                  \
		int	  has_failed = FTT(test_failed);                                                                \
		FTT(test_failed) = 0;                                                                               \
		FTT(test_case__##test_name)(data);                                                                  \
		if (FTT(options).verbose == 0 && !FTT(test_failed)) {                                               \
			printf("\e[0;32m.\e[0m");                                                                       \
			fflush(stdout);                                                                                 \
			FTT(must_put_nl) = 1;                                                                           \
		}                                                                                                   \
		FTT(test_failed) = has_failed || FTT(test_failed);                                                  \
		FTT(fixture_teardown__##fixture_name)(data);                                                        \
	}                                                                                                       \
	void FTT(test_case__##test_name)(FTT(fixture_data__##fixture_name) * ftt)

#define FT_TEARDOWN(fixture_name)                                                                    \
	static void FTT(fixture_teardown_impl__##fixture_name)(FTT(fixture_data__##fixture_name) * ftt); \
	static void FTT(fixture_teardown__##fixture_name)(FTT(fixture_data__##fixture_name) * ftt) {     \
		FTT(fixture_teardown_impl__##fixture_name)(ftt);                                             \
		free(ftt);                                                                                   \
	}                                                                                                \
	static void FTT(fixture_teardown_impl__##fixture_name)(FTT(fixture_data__##fixture_name) * ftt)

/*
 *
 *   ASSERTIONS
 *
 */

#define ___FTT_INTERNAL__GET_PRINT(statement, lstarr) \
	do {                                              \
		int p[2];                                     \
		pipe(p);                                      \
		fflush(stdout);                               \
		if (!fork()) {                                \
			close(p[0]);                              \
			dup2(p[1], fileno(stdout));               \
			do {                                      \
				statement;                            \
			} while (0);                              \
			fflush(stdout);                           \
			close(p[1]);                              \
			exit(0);                                  \
		}                                             \
		close(p[1]);                                  \
		FTT(lstarr_read_fd)(p[0], lstarr);            \
		close(p[0]);                                  \
	} while (0)

#define ___FTT_INTERNAL__TEST_FAIL() \
	do {                             \
		FTT(test_failed) = 1;        \
		if (!FTT(options).run_all)   \
			return;                  \
	} while (0)

#define ___FTT_INTERNAL__LOG_CHECK(success, msg, aftermsg)                                                              \
	do {                                                                                                                \
		if (FTT(must_put_nl))                                                                                           \
			printf("\n");                                                                                               \
		printf("\e[1;29m[%s]:\e[0m %s  \t%15s", FTT(current_test)->name, msg, (success) ? "\e[0;32mOK" : "\e[0;31mKO"); \
		if (!success || FTT(options).verbose > 1) {                                                                     \
			aftermsg                                                                                                    \
		}                                                                                                               \
		printf("\e[0m\n");                                                                                              \
		FTT(must_put_nl) = 0;                                                                                           \
	} while (0)

#define ___FTT_INTERNAL_RUN_CHECK(check, msg, aftermsg, cleanup) \
	do {                                                         \
		int success = check;                                     \
		if (!success || FTT(options).verbose) {                  \
			___FTT_INTERNAL__LOG_CHECK(success, msg, aftermsg);  \
		}                                                        \
		do {                                                     \
			cleanup                                              \
		} while (0);                                             \
		if (!success)                                            \
			___FTT_INTERNAL__TEST_FAIL();                        \
	} while (0);

#define ___FTT_INTERNAL_RUN_COMP(type_name, a, check, b, oposite_check, opts...)                             \
	do {                                                                                                     \
		FTT(lstarr_t)* va = FTT(lstarr_create)();                                                            \
		FTT(lstarr_t)* vb = FTT(lstarr_create)();                                                            \
		FTT(type_name##_t) t = {opts};                                                                       \
		___FTT_INTERNAL_RUN_CHECK(                                                                           \
			FTT(comp_and_read_##type_name)((a), (b), va, vb, &t) check 0, #a " \e[1;29m" #check "\e[0m " #b, \
			{                                                                                                \
				printf(": ");                                                                                \
				FTT(lstarr_print)(va);                                                                       \
				printf(" %s ", success ? #check : #oposite_check);                                           \
				FTT(lstarr_print)(vb);                                                                       \
			},                                                                                               \
			{                                                                                                \
				FTT(lstarr_destroy)(va);                                                                     \
				FTT(lstarr_destroy)(vb);                                                                     \
			})                                                                                               \
	} while (0)

#define FT_TRUE(condition) ___FTT_INTERNAL_RUN_CHECK((condition), #condition " \e[1;29mis\e[0m TRUE", , )

#define FT_FALSE(condition) ___FTT_INTERNAL_RUN_CHECK(!(condition), #condition " \e[1;29mis\e[0m FALSE", , )

#define FT_EQ(type_name, a, b, ...) ___FTT_INTERNAL_RUN_COMP(type_name, a, ==, b, !=, ##__VA_ARGS__)
#define FT_EQUALS(...) FT_EQ(__VA_ARGS__)

#define FT_NEQ(type_name, a, b, ...) ___FTT_INTERNAL_RUN_COMP(type_name, a, !=, b, ==, ##__VA_ARGS__)
#define FT_NOT_EQUALS(...) FT_NEQ(__VA_ARGS__)

#define FT_LT(type_name, a, b, ...) ___FTT_INTERNAL_RUN_COMP(type_name, a, <, b, >=, ##__VA_ARGS__)
#define FT_LESS_THAN(...) FT_LT(__VA_ARGS__)

#define FT_LE(type_name, a, b, ...) ___FTT_INTERNAL_RUN_COMP(type_name, a, <=, b, >, ##__VA_ARGS__)
#define FT_LESS_EQUAL(...) FT_LE(__VA_ARGS__)

#define FT_GT(type_name, a, b, ...) ___FTT_INTERNAL_RUN_COMP(type_name, a, >, b, <=, ##__VA_ARGS__)
#define FT_GREATER_THAN(...) FT_GT(__VA_ARGS__)

#define FT_GE(type_name, a, b, ...) ___FTT_INTERNAL_RUN_COMP(type_name, a, >=, b, <, ##__VA_ARGS__)
#define FT_GREATER_EQUAL(...) FT_GT(__VA_ARGS__)

/*
 *
 *   I/O CHECKS
 *
 */

#define FTT_STR_IMPL(x) #x
#define FTT_STR(x) FTT_STR_IMPL(x)

#define __FT_OUTPUT_IMPL(statement1, statement2, file, line)                                                                   \
	do {                                                                                                                       \
		FTT(lstarr_t)* a = FTT(lstarr_create)();                                                                               \
		FTT(lstarr_t)* b = FTT(lstarr_create)();                                                                               \
		___FTT_INTERNAL__GET_PRINT(statement1, a);                                                                             \
		___FTT_INTERNAL__GET_PRINT(statement2, b);                                                                             \
		___FTT_INTERNAL_RUN_CHECK(                                                                                             \
			FTT(lstarr_comp)(a, b) == 0, "\e[1;29mFT_OUTPUT(\e[0m" #statement1 "\e[1;29m, \e[0m" #statement2 "\e[1;29m)\e[0m", \
			{                                                                                                                  \
				printf(": {");                                                                                                 \
				FTT(lstarr_print_escaped)(a);                                                                                  \
				printf("} %s {", success ? "==" : "!=");                                                                       \
				FTT(lstarr_print_escaped)(b);                                                                                  \
				printf("}");                                                                                                   \
			},                                                                                                                 \
			{                                                                                                                  \
				FTT(lstarr_destroy)(a);                                                                                        \
				FTT(lstarr_destroy)(b);                                                                                        \
			});                                                                                                                \
	} while (0);

#define FT_OUTPUT(statement, expected) __FT_OUTPUT_IMPL(statement, expected, __FILE__, __LINE__)

#define FT_INPUT(printer, reader)             \
	do {                                      \
		fflush(stdout);                       \
		int p[2];                             \
		pipe(p);                              \
		if (fork()) {                         \
			close(p[1]);                      \
			int backup = dup(fileno(stdin));  \
			dup2(p[0], fileno(stdin));        \
			reader;                           \
			close(p[0]);                      \
			freopen("/dev/null", "r", stdin); \
			dup2(backup, fileno(stdin));      \
			close(backup);                    \
		} else {                              \
			close(p[0]);                      \
			int backup = dup(fileno(stdout)); \
			dup2(p[1], fileno(stdout));       \
			printer;                          \
			fflush(stdout);                   \
			dup2(backup, fileno(stdout));     \
			close(p[1]);                      \
			exit(0);                          \
		}                                     \
	} while (0)

/*
 *
 *   TYPES
 *
 */

#define ___FTT_INTERNAL_REGISTER_TYPE_LMBD(type_name, type_nameesc, type, printfn, compfn, extra...)           \
	typedef struct FTT(type_name##_s) {                                                                        \
		extra;                                                                                                 \
	} FTT(type_name##_t);                                                                                      \
	void			   FTT(print_##type_name)(type type_nameesc, FTT(type_name##_t) * ftt)                     \
		printfn int	   FTT(comp_##type_name)(type type_name##1, type type_name##2, FTT(type_name##_t) * ftt)   \
			compfn int FTT(comp_and_read_##type_name)(type type_name##1, type type_name##2, FTT(lstarr_t) * a, \
													  FTT(lstarr_t) * b, FTT(type_name##_t) * ftt) {           \
		int res = FTT(comp_##type_name)(type_name##1, type_name##2, ftt);                                      \
		___FTT_INTERNAL__GET_PRINT(FTT(print_##type_name)(type_name##1, ftt), a);                              \
		___FTT_INTERNAL__GET_PRINT(FTT(print_##type_name)(type_name##2, ftt), b);                              \
		return (res);                                                                                          \
	}

#define FT_TEST_REGISTER_TYPE_LMBD(type_name, type, printfn, compfn, extra...) \
	___FTT_INTERNAL_REGISTER_TYPE_LMBD(type_name, type_name, type, printfn, compfn, ##extra)

#define FT_TEST_REGISTER_TYPE_LMBD_(type_name, type, printfn, compfn, extra...) \
	___FTT_INTERNAL_REGISTER_TYPE_LMBD(type_name, type_name##_, type, printfn, compfn, ##extra)

#define FT_TYPE(type_name, type, args...)                         \
	typedef struct FTT(type_name##_s) {                           \
		args;                                                     \
	} FTT(type_name##_t);                                         \
	int	 FTT(comp_##type_name)(type, type, FTT(type_name##_t)*);  \
	void FTT(print_##type_name)(type, type, FTT(type_name##_t)*); \
	int	 FTT(comp_and_read_##type_name)(type, type, FTT(lstarr_t)*, FTT(lstarr_t)*, FTT(type_name##_t)*)

#define FT_TYPE_TE(type_name, type, args...)                      \
	typedef struct FTT(type_name##_s) {                           \
		args;                                                     \
	} FTT(type_name##_t);                                         \
	int	 FTT(comp_##type_name)(type, FTT(type_name##_t)*);        \
	void FTT(print_##type_name)(type, type, FTT(type_name##_t)*); \
	int	 FTT(comp_and_read_##type_name)(type, type, FTT(lstarr_t)*, FTT(lstarr_t)*, FTT(type_name##_t)*)

#define FT_TYPE_T(type_name, args...) FT_TYPE_TE(type_name, type_name, ##args)

#ifndef FT_TEST_MAIN

FT_TYPE(int, int);
FT_TYPE(long, long);
FT_TYPE(ptr, void*);
FT_TYPE(uint, unsigned int);
FT_TYPE(ulong, unsigned long);
FT_TYPE(str, char*);
FT_TYPE(buffer, void*, size_t size);
FT_TYPE(fd, int);
FT_TYPE_T(double, double tol);
FT_TYPE_T(float, float tol);

#endif

#ifdef FT_TEST_MAIN

int FTT(test_failed) = 0;
FTT(test_t) * FTT(tests) = 0;
FTT(test_t) * FTT(current_test) = 0;
FTT(test_t) * *FTT(register_handle) = &FTT(tests);
FTT(options_t) FTT(options) = {0};
int FTT(must_put_nl) = 0;

FTT(test_t) * FTT(test_copy)(FTT(test_t) * t);

void FTT(argparser)(int argc, char** argv) {
	FTT(options).program_name = *argv;

	/* Check every argument */
	int i;
	for (i = 1; i < argc; ++i) {
		char* opt = argv[i];

		if (*opt == '-') {
			++opt;
			if (*opt != '-') {
				for (; *opt; ++opt) {
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
			} else {
				++opt;
				if (strcmp(opt, "verbose") == 0) {
					FTT(options).verbose += 1;
				} else if (strcmp(opt, "exitfirst") == 0) {
					FTT(options).exit_first = 1;
					FTT(options).run_all = 0;
				} else if (strcmp(opt, "all") == 0) {
					FTT(options).exit_first = 0;
					FTT(options).run_all = 1;
				} else if (strcmp(opt, "help") == 0) {
					FTT(options).help_only = 1;
				} else if (strcmp(opt, "list") == 0) {
					FTT(options).list_tests = 1;
				}
			}
		} else {
			static FTT(test_t)** to_run_register_handle = &FTT(options).tests;
			int found = 0;
			FTT(test_t) * test;
			for (test = FTT(tests); test != 0; test = test->next) {
				if (strcmp(opt, test->name) == 0) {
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

	if (FTT(options).tests == 0) {
		FTT(options).tests = FTT(tests);
	}
}

void FTT(show_help)() {
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
		"ft_test.h: made with <3 by Eduardo \"emendes-\" Mosko at 42saopaulo\n",
		FTT(options).program_name);
}

void FTT(list_tests)() {
	puts("Tests found:");
	FTT(test_t) * test;
	for (test = FTT(tests); test != 0; test = test->next) {
		printf("	%-19s %s\n", test->name, test->file);
	}
}

int main(int argc, char** argv) {
	FTT(argparser)(argc, argv);

	if (FTT(options).help_only) {
		FTT(show_help)();
	} else if (FTT(options).list_tests) {
		FTT(list_tests)();
	} else {
		for (FTT(current_test) = FTT(options).tests; FTT(current_test) != 0; FTT(current_test) = FTT(current_test)->next) {
			FTT(current_test)->run();

			if (FTT(options).exit_first && FTT(test_failed)) {
				break;
			}
		}

		if (FTT(must_put_nl)) {
			printf("\n");
		}
	}
	return FTT(test_failed);
}

FTT(fixture_data__) * FTT(fixture_setup__)(void) {
	return (NULL);
}
void FTT(fixture_teardown__)(FTT(fixture_data__) * ftt) {}

FTT(test_t) * FTT(test_new)(const char* name, const char* file, void (*test)()) {
	FTT(test_t)* ret = (FTT(test_t)*)malloc(sizeof(FTT(test_t)));
	if (ret == NULL) {
		exit(-1);
	}
	ret->next = NULL;
	ret->name = name;
	ret->file = file;
	ret->run = test;
	return (ret);
}

FTT(test_t) * FTT(test_copy)(FTT(test_t) * t) {
	return FTT(test_new)(t->name, t->file, t->run);
}

void FTT(test_register)(const char* name, const char* file, void (*test)()) {
	*FTT(register_handle) = FTT(test_new)(name, file, test);
	FTT(register_handle) = &(*FTT(register_handle))->next;
}

void FTT(print_escaped_buffer)(char* buffer, size_t size) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (isprint(*buffer)) {
			printf("%c", *buffer);
		} else {
			/* TODO make previous color bold and restore afterwards */
			/* printf("\e[1;29m"); */
			switch (*buffer) {
				case '\a':
					printf("\\a");
					break;
				case '\b':
					printf("\\b");
					break;
				case '\f':
					printf("\\f");
					break;
				case '\n':
					printf("\\n");
					break;
				case '\r':
					printf("\\r");
					break;
				case '\t':
					printf("\\t");
					break;
				case '\v':
					printf("\\v");
					break;
				case '\\':
					printf("\\\\");
					break;
				case '\'':
					printf("\\'");
					break;
				case '\"':
					printf("\\\"");
					break;
				case '\?':
					printf("\\?");
					break;
				case '\e':
					printf("\\e");
					break;
				default:
					printf("\\%03o", *(unsigned char*)buffer);
					break;
			}
			/* printf("\e[0m"); */
		}
		++buffer;
	}
}

void FTT(print_escaped_buffer_nocol)(char* buffer, size_t size) {
	size_t i;
	for (i = 0; i < size; ++i) {
		if (isprint(*buffer)) {
			printf("%c", *buffer);
		} else {
			// printf("\e[1;29m");
			switch (*buffer) {
				case '\a':
					printf("\\a");
					break;
				case '\b':
					printf("\\b");
					break;
				case '\f':
					printf("\\f");
					break;
				case '\n':
					printf("\\n");
					break;
				case '\r':
					printf("\\r");
					break;
				case '\t':
					printf("\\t");
					break;
				case '\v':
					printf("\\v");
					break;
				case '\\':
					printf("\\\\");
					break;
				case '\'':
					printf("\\'");
					break;
				case '\"':
					printf("\\\"");
					break;
				case '\?':
					printf("\\?");
					break;
				case '\e':
					printf("\\e");
					break;
				default:
					printf("\\%03o", *(unsigned char*)buffer);
					break;
			}
			// printf("\e[0m");
		}
		++buffer;
	}
}

FT_TEST_REGISTER_TYPE_LMBD_(int, int, { printf("%i", int_); }, { return int1 - int2; });
FT_TEST_REGISTER_TYPE_LMBD_(long, long, { printf("%li", long_); }, { return long1 - long2; });
FT_TEST_REGISTER_TYPE_LMBD(ptr, void*, { printf("%p", ptr); }, { return ((char*)ptr1 - (char*)ptr2); });

FT_TEST_REGISTER_TYPE_LMBD(uint, unsigned int, { printf("%u", uint); }, { return uint1 - uint2; });
FT_TEST_REGISTER_TYPE_LMBD(ulong, unsigned long, { printf("%lu", ulong); }, { return ulong1 - ulong2; });

FT_TEST_REGISTER_TYPE_LMBD_(
	float,
	float,
	{ printf("%f", float_); },
	{ return (float1 + ftt->tol) < float2	? -1
			 : (float1 - ftt->tol) > float2 ? 1
											: 0; },
	float tol);

FT_TEST_REGISTER_TYPE_LMBD_(
	double,
	double,
	{ printf("%f", double_); },
	{ return (double1 + ftt->tol) < double2	  ? -1
			 : (double1 - ftt->tol) > double2 ? 1
											  : 0; },
	double tol);

FT_TEST_REGISTER_TYPE_LMBD(
	str,
	char*,
	{
		printf("\"");
		FTT(print_escaped_buffer)(str, strlen(str));
		printf("\"");
	},
	{ return strcmp(str1, str2); });

FT_TEST_REGISTER_TYPE_LMBD(
	buffer,
	void*,
	{
		printf("[");
		FTT(print_escaped_buffer)((char*)buffer, ftt->size);
		printf("]");
	},
	{ return memcmp(buffer1, buffer2, ftt->size); },
	size_t size;);

FT_TEST_REGISTER_TYPE_LMBD(
	fd,
	int,
	{
		char	buffer[128];
		ssize_t bytes_read;
		lseek(fd, 0, SEEK_SET);
		printf("{");
		while ((bytes_read = read(fd, buffer, 128)) > 0) {
			FTT(print_escaped_buffer)(buffer, bytes_read);
		}
		printf("}");
	},
	{
		char	buffer1[128];
		char	buffer2[128];
		ssize_t br1;
		ssize_t br2;
		int		ret;
		lseek(fd1, 0, SEEK_SET);
		lseek(fd2, 0, SEEK_SET);
		while (1) {
			br1 = read(fd1, buffer1, 128);
			br2 = read(fd2, buffer2, 128);
			if (br1 != br2) {
				return (br1 - br2);
			}
			if (br1 > 0 && (ret = memcmp(buffer1, buffer2, br1)) != 0) {
				return (ret);
			} else {
				break;
			}
		}
		return (0);
	});

/*
 *
 * lstarr_t
 *
 */

FTT(lstarr_t) * FTT(lstarr_create)() {
	FTT(lstarr_t) * ret;

	ret = (FTT(lstarr_t)*)malloc(sizeof(*ret));
	memset(ret, 0, sizeof(*ret));

	return (ret);
}

FTT(lstarr_t) * FTT(lstarr_append)(FTT(lstarr_t) * a) {
	FTT(lstarr_t) * created;

	/* TODO log error if a is NULL */

	while (a->next != NULL) {
		a = a->next;
	}

	created = FTT(lstarr_create)();
	a->next = created;

	return (created);
}

void FTT(lstarr_destroy)(FTT(lstarr_t) * arr) {
	FTT(lstarr_t) * next;

	while (arr != NULL) {
		next = arr->next;
		free(arr);
		arr = next;
	}
}

void FTT(lstarr_read_fd)(int fd, FTT(lstarr_t) * a) {
	FTT(lstarr_t) * it;
	ssize_t	 read_bytes;
	uint8_t	 to_read;
	uint8_t* buffer;

	read_bytes = 0;
	it = a;

	do {
		it->size += read_bytes;
		to_read = FTT_LST_BUF_SIZE - it->size;

		if (to_read == 0) {
			it = FTT(lstarr_append)(it);
		}

		to_read = FTT_LST_BUF_SIZE - it->size;
		buffer = it->buffer + it->size;
	} while ((read_bytes = read(fd, buffer, to_read)) > 0);
}

void FTT(lstarr_print)(FTT(lstarr_t) * a) {
	fflush(stdout);
	for (; a != NULL; a = a->next) {
		write(fileno(stdout), (char*)a->buffer, a->size);
	}
}

void FTT(lstarr_print_escaped)(FTT(lstarr_t) * a) {
	for (; a != NULL; a = a->next) {
		FTT(print_escaped_buffer)((char*)a->buffer, a->size);
	}
}

int FTT(lstarr_comp)(FTT(lstarr_t) * a, FTT(lstarr_t) * b) {
	int res;

	res = 0;
	for (; a != NULL && b != NULL; a = a->next, b = b->next) {
		res = (a->size != b->size);
		if (res) {
			break;
		}
		res = memcmp(a->buffer, b->buffer, a->size);
		if (res) {
			break;
		}
	}
	return (res);
}

#endif /* FT_TEST_MAIN */
#endif /* FT_TEST_H */
