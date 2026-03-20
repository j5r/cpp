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

    mx d = {{30,20,1,5}};
    mxi vl = {{0,1,2,3}};
    d = d/d.sumsum();
    (d * vl.t()).msg("expected value").print();
    
    
    auto A = gen::randd(d,10,500000) + float(0);
    d.print();
    A.print();
    A.mean(1).print();

    






}catch(const std::exception &e){std::cout << e.what() << "\n";}return j5r(100);
}