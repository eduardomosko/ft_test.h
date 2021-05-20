#include "ft_test.h"


FT_TEST(comp_print)
{
	void *a;
	void *b;
	FT_TRUE(1);
	FT_FALSE(0);
	FT_EQ(aprx_double, 30.0, 30.09, .tol = 0.1);
	FT_EQ(aprx_float, 1.0, 2.0, .tol = 2);
}
