# ft_test.h

Welcome to what aspires to be the easiest testing framework for C. To achieve
this, `ft_test.h` cuts numerous corners when it comes to portbility and
performance, for instance it is only available for linux and Mac OS currently,
and is only tested with gcc and clang. However those compromises allow for a
clean API that is great for writing tests fast and easily.

For example, when testing a fibonacci function all you need is this:

```c
#define FT_TEST_MAIN
#include "ft_test.h"

int ft_fibonacci(int i)
{
	if (i < 0)
		return (0);
	if (i == 0 || i == 1)
		return (1);
	return (ft_fibonacci(i - 1) + ft_fibonacci(i - 2));
}

FT_TEST(ft_fibonacci)
{
	FT_EQ(int, ft_fibonacci(0), 1);
	FT_EQ(int, ft_fibonacci(1), 1);
	FT_EQ(int, ft_fibonacci(2), 2);
	FT_EQ(int, ft_fibonacci(20), 6765);
}
```

And you may compile and run as you'd expect:
```
$ gcc fibo.c && ./a.out
fibo.c:18 [ft_fibonacci]: ft_fibonacci(20) == 6765  KO: 10946 != 6765
```

Run with `-h` or `--help` for additional options.

```
$ ./a.out --help
Usage: ./a.out [OPTIONS...] [TESTS...]

OPTIONS:
        -v, --verbose       Shows information about every assertion
        -x, --exitfirst     Exits on first assertion failed
        -a, --all           Runs every assertion
        -h, --help          Shows this message

TESTS:
        Tests to run


ft_test.h: made with <3 by Eduardo "emendes-" Mosko at 42saopaulo
```

# Available macros

Until this moment, these are the macros implemented:

##	FT_TEST

This registers a new test case. It only accepts valid C identifiers as name (so
\[\_0-9a-zA-Z] characters only). Think of it as a function declaration for your
test case. Just write it in any file and when you run the executable the test
will be automagically registered and executed.

```c
#include "ft_test.h"

FT_TEST(this_is_a_test_case)
{
	FT_EQ(str, "This should be equal", " to this");
}

FT_TEST(this_is_another_test_case)
{
	FT_NEQ(int, 0, 10);
	FT_NEQ(int, 7, 7);
}
```

```
$ gcc -D FT_TEST_MAIN ft_test.c && ./a.out
ft_test.c:7 [this_is_a_test_case]: "This should be equal" == " to this"  KO: "This should be equal" != " to this"
ft_test.c:13 [this_is_another_test_case]: 7 != 7  KO: 7 == 7
```

It optionally accepts as second parameter a fixture to use.

```c
#include "ft_test.h"

// Makes a fixture "fixture"
FT_SETUP(fixture)
{
	printf("This runs before the funcion\n");
}

FT_TEST(test_case_with_fixture, fixture)
{
	FT_NEQ(int, 0, 10);
	FT_NEQ(int, 7, 7);
}

// Sets the teardown for fixture "fixture"
FT_TEARDOWN(fixture)
{
	printf("This runs after the funcion\n");
}
```

```
$ gcc -D FT_TEST_MAIN ft_test_with_fixture.c && ./a.out
This runs before the funcion
ft_test_with_fixture.c:13 [test_case_with_fixture]: 7 != 7  KO: 7 == 7
This runs after the funcion
```


## FT_EQUALS, FT_EQUALS, FT_LESS_THAN, FT_GREATER_THAN, FT_LESS_EQUAL, FT_GREATER_EQUAL

These are the basic testing macros, they check the condition for the specified
[type](#available-types). All of them also come in the obvious compact form
(FT_EQ, FT_NEQ, FT_LT...). Use them inside your tests to verify any condition
you need.

```c
#include "ft_test.h"


```


# FT_OUTPUT, FT_INPUT

These are probably super cool. They let you redirect stdin and stdout
temporarly for your tests (without writing to disk!).

FT_OUTPUT checks that the output of a statement (may be multi-line) is the same
as of another.


```c
#include "ft_test.h"

FT_TEST(output)
{
	FT_OUTPUT(printf("orange"), printf("orange"));  // OK
	FT_OUTPUT(puts("apple"), printf("apple"));      // KO:  {apple\n} != {apple}
	FT_OUTPUT(puts("banana"), printf("banana\n"));  // OK
}

FT_TEST(output_binary)
{
	// OK
	FT_OUTPUT(
		write(1, "this contains a \0 :o", 20),
		printf("this contains a %c :o", '\0')
	);
}

FT_TEST(output_multiline)
{
	// OK
	FT_OUTPUT(
		printf("the ");
		printf("str");
		printf("ing"),

		printf("the string");
	);
}
```

FT_INPUT redirects the output of the first to the input of the
second. It does not perform any checks. Examples:

```c
#include "ft_test.h"

FT_TEST(input)
{
	int v1;
	int v2;
	
	// Redirects what is printed to stdin
	FT_INPUT(printf("102, 307"), scanf("%i, %i", &v1, &v2));
	
	FT_EQ(int, v1, 102);  // OK
	FT_EQ(int, v2, 307);  // OK
}
```

## FT_SETUP and FT_TEARDOWN

Fixture macros! They allow you to setup some data for all the tests that
request them. Each fixture **must** have both a setup and a teardown.

For FT_SETUP you should give the fixture name and you may provide a list of
variables that you want to prepare and pass to the tests. They are available
under the pointer (namespace?) `ftt` and are accessed with the arrow notation.

```c
FT_SETUP(memcmp, char b1[512]; char b2[512])
{
	memset(ftt->b1, 0, 512);
	memset(ftt->b2, 0, 512);
}

// Requests the fixture
FT_TEST(memcmp1, memcmp)
{
	FT_EQ(int, memcmp(ftt->b1, ftt->b2, 512), 0);

	ftt->b2[511] = 'a';
	FT_LT(int, memcmp(ftt->b1, ftt->b2, 512), 0);

	ftt->b1[5] = 'a';
	FT_GT(int, memcmp(ftt->b1, ftt->b2, 512), 0);
}

FT_TEST(memcmp2, memcmp)
{
	/* Each test's data is completely isolated */
	FT_EQ(int, memcmp(ftt->b1, ftt->b2, 512), 0);
}

/* You must have a teardown for each setup, even if it does nothing */
FT_TEARDOWN(memcmp) {}
```

## FT_TEST_REGISTER_TYPE, FT_TEST_REGISTER_TYPE_

These are for registering your own types. They accept 4 arguments:
 - type_name -> The alias for the type that you wanna use within FT_TEST
   macros, for example `str` for null terminated char arrays;
 - type -> The real C type of the type. In `str` this is `char *`;
 - printfn -> The body of a function that accepts a `type type_name`. For `str`
   this could be like `{ printf("%s", str); }`
 - compfn -> The body of a function that accepts a `type type_name1` and a
   `type type_name2` and compares the two in `strcmp` style. That is, it returns:
   ```
   int <  0  if  type_name1 <  type_name2
   int == 0  if  type_name1 == type_name2
   int >  0  if  type_name1 >  type_name2
   ```
   For `str` this could be `{ return strcmp(str1, str2); }`

If you use something like `clangd` it should be able to provide autocomplete
and type-checking inside the function bodys.

# Available types

For now, these are the included types available for testing, I hope it's
obvious what each of them represent.

- int, uint, long, ulong
- float, double, aprx_float, aprx_double
- str, buffer, ptr
- fd


# License
Copyright: Eduardo "emendes-" Mosko  2021.

Refer to LICENSE for licensing info, but in a nutshell, you gotta keep my
Copyright Info if reproducing the source code.
