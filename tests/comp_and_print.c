#include "../ft_test.h"


FT_TEST(comp_print)
{
	FT_TRUE(1);
	FT_FALSE(0);

	FT_NEQ(double, 30.0, 30.09, .tol = 0.05);
	FT_EQ(double, 30.0, 30.09, .tol = 0.09);
	FT_EQ(float, 1.0f, 2.0f, .tol = 2.1f);
}
