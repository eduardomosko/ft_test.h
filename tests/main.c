/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By : emendes- <emendes-@student.42sp.org.br>    +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 19:42:25 by emendes-          #+#    #+#             */
/*   Updated: 2021/05/13 22:38:10 by emendes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define FT_TEST_MAIN
#include "../ft_test.h"

FT_TEST(buffer) {
	char* buf1 = "0123\200456782";
	char* buf2 = "0123\200456789";

	FT_EQ(buffer, buf1, buf2, .size = 9);
	FT_EQ(buffer, buf1, buf2, .size = 10);
	FT_NEQ(buffer, buf1, buf2, .size = 11);
	FT_NOT_EQUALS(str, buf1, buf2);
}

FT_TEST(num) {
	FT_LE(int, 2, 2);
	FT_GT(int, 3, 2);
	FT_NEQ(int, 3, 2);
}
