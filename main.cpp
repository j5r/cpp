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
    using mxb = Matrixbool;
    using mxmm = Matrix<size_t>;
    mxmm::set_print_debug(0);

    auto A = gen::randi<int>(4,6,0,100).msg("A gen::randi");
    auto B = A.flatten().msg("B flatten()");
    auto C = A.reshape(3,8).msg("C reshape(3,8)");

    A.print();
    B.print();
    C.print();
    
    






}catch(const std::exception &e){std::cout << e.what() << "\n";}return j5r(100);
}