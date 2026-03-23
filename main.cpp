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
 

 auto A = gen::rand<float>(3,4,-10,10);
 
 A.print("A rand -10,10\n");
 A.round().print("A round\n");
 A.floor().print("A floor\n");
 A.ceil().print("A ceil\n");




    






}catch(const std::exception &e){std::cout << e.what() << "\n";}return j5r(100);
}