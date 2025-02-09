#include <unistd.h>

#include "../ft_test.h"

int g_should_fail = 0;

FT_TEST(internal_ft_msg) {
	FT_FALSE(g_should_fail, FT_MSG("-- FT_MSG OUTPUT --"));
}

FT_TEST(internal_ft_msg_format) {
	FT_FALSE(g_should_fail, FT_MSG("-- FORMAT:  %s / %.2f / %d --", "<>", 2.2222, 35));
}

// read from fd until EOF or len
void read_full(int fd, char* target, int len) {
	int offset = 0;
	int read_bytes = 0;
	do {
		offset += read_bytes;
	} while ((read_bytes = read(fileno(stdin), target + offset, 200 - offset)) > 0);
}

FT_TEST(ft_msg_does_output) {
	char printed[200] = {0};

	g_should_fail = 1;

	// check not formatted
	FT_INPUT(FTT(test_runner__internal_ft_msg)(), read_full(fileno(stdin), printed, 200));

	FT_TRUE(strstr(printed, "-- FT_MSG OUTPUT --") != NULL, FT_MSG("got: %s", printed));

	// check with formatting
	memset(printed, 0, 200);
	FT_INPUT(FTT(test_runner__internal_ft_msg_format)(), read_full(fileno(stdin), printed, 200));
	FT_TRUE(strstr(printed, "-- FORMAT:  <> / 2.22 / 35 --") != NULL, FT_MSG("got: %s", printed));

	g_should_fail = 0;
}
