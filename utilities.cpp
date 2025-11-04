#include "utilities.h"
#include "splashkit.h"
#include <cstdint>

string read_string(string prompt)
{
  write(prompt);
  return read_line();
}

int read_integer(string prompt)
{
  string line = read_string(prompt);
  while (!is_integer(line))
  {
    write_line("Please enter a whole number.");
    line = read_string(prompt);
  }
  return stoi(line);
}

double read_double(string prompt)
{
  string line = read_string(prompt);
  while (!is_double(line))
  {
    write_line("Please enter a float value.");
    line = read_string(prompt);
  }
  return stod(line);
}

uint64_t read_uint64(string prompt)
{
  string line = read_string(prompt);
  while (!is_integer(line))
  {
    write_line("Please enter a whole, 64bit number.");
    line = read_string(prompt);
  }
  
  return std::stoull(line);
}