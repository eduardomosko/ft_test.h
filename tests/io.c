#include "ft_test.h"

FT_TEST(output) {
	FT_OUTPUT(printf("aa\n"), puts("aa"));
	FT_OUTPUT(printf("bb\n"), puts("bb"));
}

FT_TEST(input) {
	int val = 0;
	int val2 = 0;
	char *line = 0;
	size_t read_size;

	FT_EQ(int, val, 0);
	FT_EQ(int, val2, 0);

	FT_INPUT(printf("This is my line printed by printf\n\n\n\n"), getline(&line, &read_size, stdin));
	FT_EQ(str, "This is my line printed by printf\n", line);
	free(line);
	line = 0;

	FT_INPUT(puts("This is my line printed by puts"), getline(&line, &read_size, stdin));
	FT_EQ(str, "This is my line printed by puts\n", line);
	free(line);

	FT_INPUT(puts("10 345"), scanf("%i %i", &val, &val2));
	FT_EQ(int, val, 10);
	FT_EQ(int, val2, 345);
}

FT_TEST(floating_point) {
	FT_EQ(float, 0.1f, 0.1f);
	FT_EQ(double, 0.1, 0.1);

	FT_LT(float, 0.1f, 0.2f);
	FT_LT(double, 0.1, 0.2);

	FT_NEQ(float , 1.f, 1.0000001f);
	FT_EQ (float , 1.f, 1.00000001f);

	FT_NEQ(double, 1.0, 1.00000001);
	FT_NEQ(double, 1.0, 1.000000000000001);
	FT_EQ (double, 1.0, 1.0000000000000001);

	FT_EQ(aprx_float , 1.f, 1.0000001f, 0.0000001f);
	FT_EQ(aprx_double, 1.0, 1.000000000000001, 0.000000000000001);
}

