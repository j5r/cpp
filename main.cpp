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
                 {-2, 2, 2, 9},
                 {0, 1, 17, 9}}, At;

        mx num = gen::randi<int>(7,7);
        mx den = gen::rand<double>(7,7,-10,10);
       
        num.print();
        std::cout << "contando 0's = " <<num.count() << std::endl;
        std::cout << "contando 1's = " <<(num==1).count() << std::endl;

        
        
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    
    return j5r(100);
}