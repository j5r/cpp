#include <iostream>
#include "matrix.cpp"


int main(){ 
  Matrix<int> m(2,2);
  std::cout << m.get(0,1) << "\n";
  m.set(1,0,3.5);
  m.print();
  
  

std::cout<< std::endl;
return 0;
}