#include <iostream>
#include "Matrix.hpp"
#include "MatrixShape.hpp"
#include "MatrixGen.hpp"
#include "MatrixLinAlg.hpp"

 

int main()
{
    try
    {

        using alg = MatrixLinAlg;
        using shape = MatrixShape;
        using gen = MatrixGen;
        using mx = Matrix<double>;
        using mxi = Matrix<int>; // mxi::set_print_precision(0); mxi::set_print_width(0);
        using mxb = Matrixbool;

        // Matriz Assimétrica (Autovalores conhecidos: 3, 2, 1)

        mx A = {{7, 0, 0, 9},
                 {-1, 0, 9, 9},
                 {-2, 2, 2, 9}};
    
        
        A.print();
                 A.diag2().print();
        
        
        
        
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    
    return j5r(100);
}