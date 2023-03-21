#include "matrix.hpp"

#include <iostream>
#include <initializer_list>

int main()
{
    std::initializer_list<int> row1 = {2, 3};
    std::initializer_list<int> row2 = {4, 5};

    std::initializer_list<std::initializer_list<int>> mtxList { row1, row2 };

    csd2125::Matrix<int> mtx(mtxList);
    csd2125::Matrix<int> mtx2(2, 2);

    mtx2 = std::move(mtx);

    using size_type = csd2125::Matrix<int>::size_type;
    
    for (size_type r = 0; r < mtx2.get_rows(); ++r)
    {
        for (size_type c = 0; c < mtx2.get_cols(); ++c)
        {
            std::cout << mtx2[r][c] << ' ';
        }
        std::cout << std::endl;
    }
}