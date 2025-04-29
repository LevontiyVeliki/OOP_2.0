#include <functional>
#include <lib/ArgParser.h>

#include <iostream>
#include <numeric>

using namespace std;

struct Options {
  bool sum = false;
  bool mult = false;
};

int main(int argc, char** argv) {
  Options opt;
  vector<int> values;
  vector<string> args_vector(argv, argv + argc);

  ArgumentParser::ArgParser parser("Program");
  parser.AddIntArgument("N").MultiValue(1).Positional().StoreValues(values);
  parser.AddFlag("sum", "add args").StoreValue(opt.sum);
  parser.AddFlag("mult", "multiply args").StoreValue(opt.mult);
  parser.AddHelp('h', "help", "Program accumulate arguments");

  if (!parser.Parse(args_vector)) {
    cout << "Wrong argument" << endl;
    cout << parser.HelpDescription() << endl;
    return 1;
  }

  if (parser.Help()) {
    cout << parser.HelpDescription() << endl;
    return 0;
  }

  if (opt.sum) {
    cout << "Result: " << accumulate(values.begin(), values.end(), 0) << endl;
  } else if (opt.mult) {
    cout << "Result: " << accumulate(values.begin(), values.end(), 1, multiplies<int>()) << endl;
  } else {
    cout << "No one options had chosen" << endl;
    cout << parser.HelpDescription();
    return 1;
  }

  return 0;

}
