#include <iostream>
#include "matrix.cpp"


int main(){ 
  Matrix<float> m(2,4);
  Matrix<double> n(53,123);
  n.set(-3,-2,3.14159265358979);
  std::cout << m.get(0,1) << "\n";
  m.set(1,0,-3.5);
  m.print(2);
  n.set(1,12,-112.91);
  n.print();

  
  
  

std::cout<< std::endl;
return 0;
}