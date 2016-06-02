/*
 * Copyright (c) 2016 Simon Schoenenberger
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "number_parser.h"

#define MAX_INT ((uint64_t) INT64_MAX + 1)
#define MAX_POS_INT (MAX_INT - 1)
#define MAX_EXP (-DBL_MIN_10_EXP)
#define MAX_LEN INT16_MAX

static void convert_to_float(number_parser* parser) {
	if (!parser->is_float) {
		parser->is_float = 1;
		parser->fval = parser->uval;
	}
}

void number_parser_add_digit(number_parser* parser, int digit) {
	if (parser->int_len >= MAX_LEN) {
		return;
	}

	if (!parser->is_float) {
		// check if there is room for another digit, otherwise convert to float
		if (parser->uval > MAX_INT / parser->base) {
			convert_to_float(parser);
		}
	}

	if (!parser->is_float) {
		parser->uval *= parser->base;

		// convert to float if integer value is greater than the maximum
		// representable negative value
		if (parser->uval > MAX_INT - digit) {
			convert_to_float(parser);
			parser->fval += digit;
		}
		else {
			parser->uval += digit;
		}
	}
	else {
		parser->fval = parser->fval * parser->base + digit;
	}

	parser->int_len ++;
}

void number_parser_add_exp_digit(number_parser* parser, int digit) {
	// ignore large exponent values
	if (parser->exp_val < MAX_EXP) {
		parser->exp_val = parser->exp_val * parser->base + digit;
		parser->has_exp = 1;
	}
}

int number_parser_end(number_parser* parser) {
	// As the maximum magnitude of a positive integer is one less than its
	// negative counterpart, it cannot be represented as a signed integer if
	// its value is greater or equal as such. So convert to a floating-point
	// value. Also convert if an exponent or the radix point is set.
	if ((!parser->sign && parser->uval > MAX_POS_INT) || parser->has_exp || parser->rad_off >= 0) {
		convert_to_float(parser);
	}

	if (parser->is_float) {
		int n = parser->exp_val;
		double d = parser->base;
		double e = 1.0;

		if (parser->exp_sign) {
			n = -n;
		}

		if (parser->rad_off >= 0) {
			n -= parser->int_len - parser->rad_off;
		}

		if (n < 0) {
			parser->exp_sign = 1;
			n = -n;
		}

		while (n) {
			if (n & 1) {
				e *= d;
			}

			n >>= 1;
			d *= d;
		}

		if (parser->sign) {
			parser->fval = -parser->fval;
		}

		if (parser->exp_sign) {
			parser->fval /= e;
		}
		else {
			parser->fval *= e;
		}
	}
	else if (parser->sign) {
		parser->ival = -parser->ival;
	}

	return parser->is_float;
}
