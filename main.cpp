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

    
    
    
    
mxi A = gen::randi<int>(4,5,0,10);
A.print();
auto [idxi, vali] = A.min(1);
auto [idxa, vala] = A.max(1);







}catch(const std::exception &e){std::cout << e.what() << "\n";}return j5r(100);
}