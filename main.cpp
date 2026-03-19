#include <iostream>
#include "Matrix.hpp"
#include "MatrixShape.hpp"
#include "MatrixGen.hpp"
#include "MatrixLinAlg.hpp"

int main()
{
    using alg = MatrixLinAlg;
    using shape = MatrixShape;
    using gen = MatrixGen;
    using mx = Matrix<double>;
    using mxi = Matrix<int>;
    using mxb = Matrixbool;
    //using mxb = Matrix<bool>;
    mx::set_print_precision(4);
    mx::set_print_width(4);
    mx::set_print_debug(1);
    mxb::set_print_debug(1);
    gen::set_random_seed(10);
    //mxb::set_print_debug(1);
    // mxb::set_print_debug(1);

         //mx::set_print_debug(1);
         //mx::set_print_precision(6);
    //  mx::set_print_color(1);
    //    mx::set_print_font(1);

mxi A={{1,2,3},{4,5,6}};
alg::pivot(A,1,2).print();
try{
std::cout << A << "\n";
std::cout << A.get(-1,-3) << "\n";
}catch(const std::exception& e){
    std::cout << e.what() << "\n";
}

    return j5r(100);
}