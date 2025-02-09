#include "../ft_test.h"
 

FT_TEST(output)
{
	FT_OUTPUT(printf("aa\n"), puts("aa"));
	FT_OUTPUT(printf("bb\n"), puts("bb"));
}

FT_TEST(more_output)
{
	FT_OUTPUT(printf("orange"), printf("orange"));
	FT_OUTPUT(puts("apple"), puts("apple"));
	FT_OUTPUT(puts("banana"), printf("banana\n"));
}

FT_TEST(input) {
	int val = 0;
	int val2 = 0;
	char ch = 0;

	FT_EQ(int, val, 0);
	FT_EQ(int, val2, 0);

	FT_INPUT(printf("This is my line printed by printf\n\n\n\n"), ch = getchar());
	FT_EQ(int, 'T', ch);

	FT_INPUT(puts("Another line, it is printed by puts"), ch = getchar());
	FT_EQ(int, 'A', ch);

	char *line = 0;
	size_t size;
	FT_INPUT(printf("This is my line printed by printf"), getline(&line, &size, stdin));

	FT_INPUT(puts("10 345"), scanf("%i %i", &val, &val2));
	FT_EQ(int, val, 10);

	FT_EQ(int, val2, 345);

	FT_EQ(str, "This is my line printed by printf", line);
	free(line);
}

FT_TEST(output_binary)
{
	FT_OUTPUT(
		write(1, "this contains a \0 :o", 20),
		printf("this contains a %c :o", '\0')
	);
}

FT_TEST(output_multiline)
{
	FT_OUTPUT(
		printf("the ");
		printf("str");
		printf("ing"),

		printf("the string");
	);
}

FT_TEST(floating_point) {
	FT_EQ(float, 0.1f, 0.1f);
	FT_EQ(double, 0.1, 0.1);

	FT_LT(float, 0.1f, 0.2f);
	FT_LT(double, 0.1, 0.2);

	FT_NEQ(float , 1.f, 1.0000001f);
	FT_EQ(float , 1.f, 1.0000001f, .tol = 0.0000001f);

	FT_EQ (float , 1.f, 1.00000001f);

	FT_NEQ(double, 1.0, 1.00000001);
	FT_NEQ(double, 1.0, 1.000000000000001);
	FT_EQ (double, 1.0, 1.0000000000000001);

	FT_EQ(double, 1.0, 1.000000000000001, .tol = 0.000000000000001);
}

