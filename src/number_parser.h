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

/**
 * @file
 *
 * A general purpose number parser.
 *
 * The parser is not intended to parse a string itself, but rather to receive
 * the input from an external parser.
 *
 * It starts interpreting the number as a signed integer and switches to
 * floating-point if either the number is too long to represent it as an
 * integer, the radix point is set or the an exponent digit is added.
 *
 * After initialization with number_parser_init(), the parser functions can be
 * called in any order. number_parser_end() terminates the parser and calculates
 * the final number.
 *
 * Numbers exceeding the value range of a `double` will have the value
 * `INFINITY` or `-INFINITY` if positive or negative, respectively.
 *
 * @code{.c}
 * // Define parser.
 * number_parser parser;
 *
 * // Initialize parser with base 10.
 * number_parser_init(&parser, 10);
 *
 * // Parse the number "-12.3e4".
 * // Notice that the call order has not to match the input except for
 * // the digits.
 *
 * number_parser_add_exp_digit(&parser, 4); // add exponent digit "4"
 * number_parser_add_digit(&parser, 1);     // add integer digit "1"
 * number_parser_add_digit(&parser, 2);     // add integer digit "2"
 * number_parser_set_rad_point(&parser);    // set radix point at current offset
 * number_parser_add_digit(&parser, 3);     // add integer digit "3"
 * number_parser_set_neg(&parser, 1);       // set number negative
 *
 * // Terminate parser.
 * int is_float = number_parser_end(&parser);
 *
 * if (is_float) {
 *     printf("float: %lf\n", parser.fval);
 * }
 * else {
 *     printf("int: %lld\n", parser.ival);
 * }
 * @endcode
 */

#pragma once

#include <float.h>
#include <stdint.h>

#ifndef DBL_MAX_10_EXP
#define DBL_MAX_10_EXP 307  ///< The maximum representable decimal exponent.
#define DBL_MIN_10_EXP -308 ///< The minimum representable decimal exponent.
#endif

/**
 * A general purpose number parser.
 */
typedef struct {
	int16_t int_len;    ///< Number of digits including integer and fractional part.
	int16_t rad_off;    ///< Offset of radix point.
	int16_t exp_val;    ///< Exponent value.
	uint8_t base;       ///< Number base.
	uint8_t sign:1;     ///< Number sign; 0: positive, 1: negative.
	uint8_t exp_sign:1; ///< Exponent sign; 0: positive, 1: negative.
	uint8_t is_float:1; ///< Number type; 0: integer, 1: floating-point.
	uint8_t has_exp:1;  ///< Has exponent.
	uint8_t was_int:1;  ///< Set to 1 if integer was converted to float because of overflow.
	union {
		int64_t ival;   ///< Signed integer value.
		uint64_t uval;  ///< Unsigned integer value.
		double fval;    ///< Floating-point value.
	};                  ///< Mantissa containing number value ignoring radix point.
} number_parser;

/**
 * Initialize a number parser struct with `base`, which can be a value between
 * 2 and 255.
 *
 * @param parser The number parser struct to be initialized.
 * @param base The number base between 2 and 255.
 */
static inline void number_parser_init(number_parser* parser, uint8_t base) {
	*parser = (number_parser) {
		.base = base,
		.rad_off = -1,
	};
}

/**
 * Add the next integer or fraction `digit`.
 * The number will be converted to floating-point if needed.
 *
 * @param parser The number parser to add a digit to.
 * @param digit An integer between 0 and `base` - 1.
 */
extern void number_parser_add_digit(number_parser* parser, int digit);

/**
 * Add the next exponent `digit`.
 * This will convert the number to floating-point.
 *
 * @param parser The number parser to add a exponent digit to.
 * @param digit An integer between 0 and  `base` - 1.
 */
extern void number_parser_add_exp_digit(number_parser* parser, int digit);

/**
 * Set the radix point at the current offset.
 * This will convert the number to floating-point.
 *
 * @param parser The number parser to set the radix point for.
 */
static inline void number_parser_set_rad_point(number_parser* parser) {
	parser->rad_off = parser->int_len;
}

/**
 * Set the number sign negative.
 *
 * @param parser The number parser to set the sign for.
 * @param negative A flag indicating that the number should be set to negative.
 */
static inline void number_parser_set_neg(number_parser* parser, int negative) {
	parser->sign = negative != 0;
}

/**
 * Set the exponent sign negative.
 *
 * @param parser The number parser to set the exponent sign for.
 * @param negative A flag indicating that the exponent should be set to negative.
 */
static inline void number_parser_set_exp_neg(number_parser* parser, int negative) {
	parser->exp_sign = negative != 0;
}

/**
 * End parser and calculate the final number.
 *
 * Returns 0 if the number is an integer or 1 if the number is a floating-point
 * value. The value can be read with `parser.ival` or `parser.fval`, respectively.
 *
 * @param parser The number parser to terminate.
 * @return 0 if the number is an integer or 1 if the number is a floating-point
 * value.
 */
extern int number_parser_end(number_parser* parser);
