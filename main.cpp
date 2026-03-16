#include <iostream>
#include "Matrix.hpp"
#include "MatrixShape.hpp"
#include "MatrixGen.hpp"

int main()
{
    using shape = MatrixShape;
    using gen = MatrixGen;
    using mx = Matrix<double>;
    mx::set_print_precision(4);
    mx::set_print_width(4);
    
    

    auto m = gen::exprand<double>(.3,7,7);
    auto n = gen::randi<int>(7, 7,-100);

    m.msg("m").print();
    n.msg("n").print();
    (m>n).msg("M>N").print();


    return j5r(100);
}