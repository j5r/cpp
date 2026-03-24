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
    //mx::set_print_precision(2);
    //mx::set_print_width(2);
 

 mx A = {{1,2,3},{4,5,6},{7,8,3},{10,11,11},{90,180,-200}};
 A.print();
 auto [U,S,V] = alg::svd(A);
 U.print();
 S.print();
 V.print();
 auto res = A - U * S * V.t();
 
 res.msg("matriz residual").print();
 std::cout << "residuo norma " << alg::norm(res) << "\n";
std::cout << "rank " << alg::rank(A) << "\n";
 





    






}catch(const std::exception &e){std::cout << e.what() << "\n";}return j5r(100);
}