#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <cstdint>
using std::string;

/**
 * Read a string from the user
 *
 * @param prompt the message to show the user
 * @returns the string entered
 */
string read_string(string prompt);

/**
 * Read an integer from the user
 *
 * @param prompt the message to show the user
 * @returns the integer entered
 */
int read_integer(string prompt);

/**
 * Read an double from the user
 *
 * @param prompt the message to show the user
 * @returns the integer entered
 */
double read_double(string prompt);
uint64_t read_uint64(string prompt);
#endif