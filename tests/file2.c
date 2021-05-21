/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By : emendes- <emendes-@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 22:37:06 by emendes-          #+#    #+#             */
/*   Updated: 2021/05/13 23:04:06 by emendes-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_test.h>

FT_TEST(str) {
	FT_EQ(str, "batata", "batata");
	FT_GT(str, "batata", "banana");
	FT_EQ(str, "batata", "batata");
	FT_NEQ(str, "batata", "banana");
}

FT_TEST(buffer2) {
	char *buf1 = "0123\200456780";
	char *buf2 = "0123\200456789";

	FT_EQ(buffer, buf1, buf2, 9);
	FT_LE(buffer, buf1, buf2, 10);
	FT_NEQ(buffer, buf1, buf2, 11);
	FT_LT(buffer, buf1, buf2, 11);
}
