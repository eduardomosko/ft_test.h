#include <ft_test.h>

int g_fixture_ran = 0;

FT_SETUP(no_arguments)
{
	g_fixture_ran = 1;
}

FT_TEST(fix_no_args, no_arguments)
{
	FT_EQ(int, g_fixture_ran, 1);
}

FT_TEARDOWN(no_arguments)
{
	g_fixture_ran = 0;
}

FT_TEST(fix_no_args_runs_on_exit)
{
	// Asserts not runned
	FT_EQ(int, g_fixture_ran, 0);

	// Asserts ran
	FTT(test_runner__fix_no_args)();

	// Asserts teardown ran
	FT_EQ(int, g_fixture_ran, 0);
}
