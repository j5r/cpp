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

    mx A = {{1,2,3},{2,3,4}};
    mx B;
   
    A.print("A");
    A /= B;
    A.print("1");
    



    






}catch(const std::exception &e){std::cout << e.what() << "\n";}return j5r(100);
}