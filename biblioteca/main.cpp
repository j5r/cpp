#include <iostream>
#include "matrix.cpp"


int main(){ 
  
  Matrix<int> m(3,3);
  Matrix<float> n(3,3);
  Matrix<float> p;

  m.set(1,2,6);
  n.set(0,1,3);

  m.print();
  n.print();
  p = n+4;
  p = -p/0;
  p.print();
  

  

  


  //m.set(1,1,13);
  //n.set(1,1,22);
  
  //m.print();
  //n.print();
  
  
  

std::cout<< std::endl;
return 0;
}