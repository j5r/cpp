#include <iostream>
#include "matrix.cpp"


int main(){ 
  Matrix<float> m(2,4);
  Matrix<double> n(2,13);
  n.set(0,0,3.14159265358979);
  std::cout << m.get(0,1) << "\n";
  m.set(1,0,-3.5);
  m.print(2);
  n.print(2);
  
  
  

std::cout<< std::endl;
return 0;
}