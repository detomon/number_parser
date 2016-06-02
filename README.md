Number Parser
=============

A general purpose number parser.

The parser is not intended to parse a string itself, but rather to receive the input from an external parser.

It starts interpreting the number as a signed integer and switches to floating-point if either the number is too long to represent it as an integer, the radix point is set or the an exponent digit is added.

After initialization with `number_parser_init()`, the parser functions can be called in any order. `number_parser_end()` terminates the parser and calculates the final number.

Numbers exceeding the value range of a `double` will have the value `INFINITY` or `-INFINITY` if positive or negative, respectively.

```c
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

if (is_float) {
    printf("float: %lf\n", parser.fval);
}
else {
    printf("int: %lld\n", parser.ival);
}
```
