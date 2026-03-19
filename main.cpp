#include <iostream>
#include "Matrix.hpp"
#include "MatrixShape.hpp"
#include "MatrixGen.hpp"

int main()
{
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

mxi A= gen::randi<int>(100,100);
mx B(100,100);
B.save_to_binary("B.dat");
B.save_to_file("B.csv");

mxb C = A > 0.5;
C.msg("A>0.5");


std::cout <<A ;
std::cout <<B ;
std::cout <<C ;
A.save_to_binary("A.dat");
A.save_to_file("A.csv");


    return j5r(100);
}