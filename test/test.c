#include <stdio.h>
#include "number_parser.h"

int main() {
	// Define parser.
	number_parser parser;

	// Initialize parser with base 10.
	number_parser_init(&parser, 10);

	// Parse the number "-12.3e4".
	// Notice that the call order does not have to match the input, except for the digits.

	number_parser_add_exp_digit(&parser, 4); // add exponent digit "4"
	number_parser_add_digit(&parser, 1);     // add integer digit "1"
	number_parser_add_digit(&parser, 2);     // add integer digit "2"
	number_parser_set_rad_point(&parser);    // set radix point at current offset
	number_parser_add_digit(&parser, 3);     // add integer digit "3"
	number_parser_set_neg(&parser, 1);       // set number negative

	// Terminate parser.
	int is_float = number_parser_end(&parser);

	if (is_float) {
		printf("float: %lf\n", parser.fval);
	}
	else {
		printf("int: %lld\n", parser.ival);
	}

	return 0;
}
