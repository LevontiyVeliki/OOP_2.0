#include <iostream>
#include <map>

#include "lib/myadapters.h"

int main() {
  std::vector<int> v = {1, 2, 3, 4, 5, 6};
  std::vector<int> r2 = {4, 5};
  auto r = v | Transform([](int i){
    return i * i;}) | Transform([](int i){
    return i * 3;});
  for (auto i : r){
    std::cout << i << ' ';
  }

  return 0;
}
