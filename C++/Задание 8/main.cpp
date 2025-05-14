#include <iostream>

#include "lib/bst.h"


int main() {
  BST<int,InOrder> a({5,3,2,4,6});
  std::cout << a.find(4);

  return 0;
}