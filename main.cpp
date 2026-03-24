#include <iostream>
#include "Matrix.hpp"
#include "MatrixShape.hpp"
#include "MatrixGen.hpp"
#include "MatrixLinAlg.hpp"

int main()
{try{

    using alg = MatrixLinAlg;
    using shape = MatrixShape;
    using gen = MatrixGen;
    using mx = Matrix<double>;
    using mxi = Matrix<int>;
    mx::set_print_precision(2);
    mx::set_print_width(2);
 

 mx A = {{1,2,3},{4,5,6},{7,8,3}};
 mx b = {{14},{32},{32}};
 A.print();
 b.print();
 mx c = alg::gmres(A,b);
 c.print();





    






}catch(const std::exception &e){std::cout << e.what() << "\n";}return j5r(100);
}