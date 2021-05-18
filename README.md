# ft_test.h

Welcome to what aspires to be the easiest testing framework for C. To achieve this, `ft_test.h` cuts numerous corners when it comes to portbility and performance, for example it is only available for linux and Mac OS at the moment, and is only tested with gcc and clang. However those conditions allow for a very clean API that is great for testing small projects and assignments.

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

And you may compile and run as expected:
```bash
$ gcc fibo.c && ./a.out
[ft_fibonacci]: KO: expected ft_fibonacci(20) == 6765, but 10946 != 6765
```

Run with `-h` or `--help` for additional options.
```c
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

- FT_TEST
  
  This registers a new test case. It only accepts valid C identifiers as name (so \[\_0-9a-zA-Z] characters only).

- FT_EQUALS, FT_EQUALS, FT_LESS_THAN, FT_GREATER_THAN, FT_LESS_EQUAL, FT_GREATER_EQUAL
  
  These are the basic testing macros, they check the condition for the specified [type](#available-types). All of them also come in the obvious compact form (FT_EQ, FT_NEQ, FT_LT...)

- FT_OUTPUT, FT_INPUT

  These are probably one of the biggest selling points of this framework. They let you redirect stdin and stdout temporarly for your tests. FT_OUTPUT checks that the output of a statement (may be multi-line) is the same of another. FT_INPUT redirects the output of the first to the input of the second. It does not perform any checks. Examples:

```c
FT_TEST(output)
{
  FT_OUTPUT(printf("orange"), printf("orange"));  // OK
  FT_OUTPUT(puts("apple"), printf("apple"));      // KO:  {apple\n} != {apple}
  FT_OUTPUT(puts("banana"), printf("banana\n"));  // OK
}

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

- FT_SETUP and FT_TEARDOWN

  Fixture macros! They allow you to setup some data for all the tests that request them.

```c
FT_SETUP(memcmp, { char b1[512]; char b2[512] })
{
	memset(ftt->b1, 0, 512);
	memset(ftt->b2, 0, 512);
}

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
	/* Each test data is isolated */
	FT_EQ(int, memcmp(ftt->b1, ftt->b2, 512), 0);
}

/* You must have a teardown for each setup, even if it does nothing */
FT_TEARDOWN(memcmp) {}
```

- FT_TEST_REGISTER_TYPE_LAMBDA, FT_TEST_REGISTER_TYPE_LMBD, FT_TEST_REGISTER_TYPE_LMBD_, FT_TEST_REGISTER_TYPE, FT_TEST_TYPE_ALIAS, FT_TYPE

  These are for registering your own types. More info coming soon...


# Available types

For now, these are the included types available for testing, I hope it's obvious what each of them represent.

- int, uint, long, ulong
- float, double, aprx_float, aprx_double
- str, buffer, ptr
- fd


# License
Copyright: Eduardo "emendes-" Mosko  2021.

Refer to LICENSE for licensing info, but in a nutshell, you gotta keep my Copyright Info if reproducing the source code.
