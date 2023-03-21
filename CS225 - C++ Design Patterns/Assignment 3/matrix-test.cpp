// matrixtest.cpp
// -- simple test of matrix class with proxy class
// csd2125

#include <exception>
#include <iostream>
#include <iomanip>
#include <random>
#include <type_traits>
#include "matrix.hpp"

// tests for Matrix::value_type == int
void test1(); 
void test2();
// tests for Matrix::value_type == double
void test3(); 
void test4();

int main() 
{
    std::cout << "------------------------TEST 1 START------------------------\n";
    test1();
    std::cout << "------------------------TEST 1 END------------------------\n";

    std::cout << "------------------------TEST 2 START------------------------\n";
    test2();
    std::cout << "------------------------TEST 2 END------------------------\n";

    std::cout << "------------------------TEST 3 START------------------------\n";
    test3();
    std::cout << "------------------------TEST 3 END------------------------\n";

    std::cout << "------------------------TEST 4 START------------------------\n";
    test4();
    std::cout << "------------------------TEST 4 END------------------------";
}

template <typename T>
std::ostream& operator<<(std::ostream& s, csd2125::Matrix<T> const& m) 
{
    std::ios::fmtflags old_settings;
    if (std::is_floating_point_v<T>) 
    {
        old_settings = std::cout.flags();
        std::cout << std::fixed;
        std::cout << std::setprecision(2);
    }

    using size_type = typename csd2125::Matrix<T>::size_type;

    for (size_type i=0; i < m.get_rows(); ++i) 
    {
        s << (i==0 ? '{' : ',');
        for (size_type j=0; j < m.get_cols(); ++j) 
        {
            s << (j==0 ? '{' : ',') << m[i][j];
        }
        s << '}';
    }
    s << '}';

    if (std::is_floating_point_v<T>) 
    {
        std::cout.flags(old_settings);
    }
    return s;
}

using namespace csd2125;

template <typename T>
Matrix<T> matrix_generator(size_t rows, size_t cols) 
{
    static_assert(std::is_arithmetic<T>::value == true, "T must be arithmetic type\n");
    std::default_random_engine defEngine{99};

    if (std::is_integral<T>::value) 
    {
        std::uniform_int_distribution<int> intDistro(0,10);

        Matrix<T> tmp(rows, cols);
        using size_type = typename Matrix<T>::size_type;

        for (size_type i{0}; i < rows; ++i) 
        {
            for (size_type j{0}; j < cols; ++j) 
            {
                tmp[i][j] = intDistro(defEngine);
            }
        }
        return tmp;
    } 
    else if (std::is_floating_point<T>::value) 
    {
        std::uniform_real_distribution<double> intDistro(0.0,10.0);

        Matrix<T> tmp(rows, cols);
        using size_type = typename Matrix<T>::size_type;

        for (size_type i{0}; i < rows; ++i) 
        {
            for (size_type j{0}; j < cols; ++j) 
            {
                tmp[i][j] = intDistro(defEngine);
            }
        }
        
        return tmp;
    }
}

template <typename T>
Matrix<T> matrix_generator(Matrix<T> const& m) 
{
    return m;
}

// test 1 for T == int: ctors, dtor, op=, overloaded op[], op==, and op!=
void test1() 
{
    using size_type = typename Matrix<int>::size_type;
    std::default_random_engine defEngine(99);
    std::uniform_int_distribution<int> intDistro(0, 100);

    // test: ctor (two parameter) and operator[]
    Matrix<int> A(2,3);
    A[0][0] = 1;  A[0][1] = 2;  A[0][2] = 3;
    A[1][0] = 4;  A[1][1] = 5;  A[1][2] = 6;
    std::cout << "A:\n" << A << "\n";

    size_type rows{6}, cols{5};
    Matrix<int> B(rows, cols);
    for (size_type i{0}; i < rows; ++i) 
    {
        for (size_type j{0}; j < cols; ++j) 
        {
            B[i][j] = intDistro(defEngine);
        }
    }
    std::cout << "B:\n" << B << "\n";

    // test: copy ctor and op==
    Matrix<int> const C{B};
    if (B != C) 
    {
        std::cout << "problem with copy ctor!!!\n";
        return;
    }
    std::cout << "C:\n" << C << "\n";

    // test: move ctor
    Matrix<int> const D(std::move(matrix_generator<int>(C)));
    if (false == (D == C)) 
    {
        std::cout << "problem with move ctor!!!\n";
        return;
    }
    std::cout << "D:\n" << D << "\n";

    Matrix<int> E
    {
        {D[0][0],D[0][1],D[0][2],D[0][3],D[0][4]},
        {D[1][0],D[1][1],D[1][2],D[1][3],D[1][4]},
        {D[2][0],D[2][1],D[2][2],D[2][3],D[2][4]},
        {D[3][0],D[3][1],D[3][2],D[3][3],D[3][4]},
        {D[4][0],D[4][1],D[4][2],D[4][3],D[4][4]},
        {D[5][0],D[5][1],D[5][2],D[5][3],D[5][4]}
    };
    if (E != D) 
    {
        std::cout << "problem with initializer list ctor!!!\n";
        return;
    }
    std::cout << "E:\n" << E << "\n";

    // copy op=
    E = A;
    std::cout << "E:\n" << E << "\n";

    // move op=
    E = {{6,5},{4,3},{2,1}}; // E changes from 6x5 to 3x2 
    std::cout << "E:\n" << E << "\n";

    try 
    {
        Matrix<int> F = {{1,2,3},{4,5}};
        std::cout << F << "\n";
    } 
    catch (std::exception &e) 
    {
        std::cout << e.what() << "\n";
    }
}

// test 2 for T == int: ctors, overloaded op[], op==, op!=, op+, op-, overloaded op*
void test2() 
{
    using size_type = typename Matrix<int>::size_type;
    std::default_random_engine defEngine(2);
    std::uniform_int_distribution<int> intDistro(0,100);
    size_type rows{7}, cols{6};
    Matrix<int> A(rows, cols);
    for (size_type i{0}; i < rows; ++i) 
    {
        for (size_type j{0}; j < cols; ++j) 
        {
            A[i][j] = intDistro(defEngine);
        }
    }
    std::cout << "A:\n" << A << "\n";

    rows = 6; cols = 7;
    Matrix<int> B(rows, cols);
    for (size_type i{0}; i < rows; ++i) 
    {
        for (size_type j{0}; j < cols; ++j) 
        {
            B[i][j] = intDistro(defEngine);
        }
    }
    std::cout << "B:\n" << B << "\n";

    Matrix<int> C{std::move(matrix_generator<int>(A.get_rows(), A.get_cols()))};
    std::cout << "C:\n" << C << "\n";

    // impossible addition, subtraction, multiplication
    try 
    {
        Matrix<int> Z = A + B;
        std::cout << "Z:\n" << Z << "\n";
    } 
    catch (std::exception& e) 
    {
        std::cout << e.what() << "\n";
    }

    try 
    {
        Matrix<int> Z = A - B;
        std::cout << "Z:\n" << Z << "\n";
    } 
    catch (std::exception& e) 
    {
        std::cout << e.what() << "\n";
    }

    try 
    {
        Matrix<int> Z = A * C;
        std::cout << "Z:\n" << Z << "\n";
    } 
    catch (std::exception& e) 
    {
        std::cout << e.what() << "\n";
    }

    Matrix<int> D = A * B; // 7x6 and 6x4 == 7x4
    std::cout << "D:\n" << D << "\n";
    D = A*(2*B);
    std::cout << "D:\n" << D << "\n";

    Matrix<int> E = A + C;
    std::cout << "E:\n" << E << "\n";
    E = A - C;
    std::cout << "E:\n" << E << "\n";

    Matrix<int> F = D - 2*D; 
    std::cout << "F:\n" << F << "\n";
}

// test 3 for T == double: ctors, dtor, op=, overloaded op[], op==, and op!=
void test3() 
{
    using size_type = typename Matrix<double>::size_type;
    std::default_random_engine defEngine(99);
    std::uniform_real_distribution<double> intDistro(-10.0, 10.0);

    // test: ctor (two parameter) and operator[]
    Matrix<double> A(2,3);
    A[0][0] = 1;  A[0][1] = 2;  A[0][2] = 3;
    A[1][0] = 4;  A[1][1] = 5;  A[1][2] = 6;
    std::cout << "A:\n" << A << "\n";

    size_type rows{6}, cols{5};
    Matrix<double> B(rows, cols);
    for (size_type i{0}; i < rows; ++i) 
    {
        for (size_type j{0}; j < cols; ++j) 
        {
            B[i][j] = intDistro(defEngine);
        }
    }
    std::cout << "B:\n" << B << "\n";

    // test: copy ctor and op==
    Matrix<double> const C{B};
    if (B != C) 
    {
        std::cout << "problem with copy ctor!!!\n";
        return;
    }
    std::cout << "C:\n" << C << "\n";

    // test: move ctor
    Matrix<double> const D(std::move(matrix_generator<double>(C)));
    if (false == (D == C)) 
    {
        std::cout << "problem with move ctor!!!\n";
        return;
    }
    std::cout << "D:\n" << D << "\n";

    Matrix<double> E
    {
        {D[0][0],D[0][1],D[0][2],D[0][3],D[0][4]},
        {D[1][0],D[1][1],D[1][2],D[1][3],D[1][4]},
        {D[2][0],D[2][1],D[2][2],D[2][3],D[2][4]},
        {D[3][0],D[3][1],D[3][2],D[3][3],D[3][4]},
        {D[4][0],D[4][1],D[4][2],D[4][3],D[4][4]},
        {D[5][0],D[5][1],D[5][2],D[5][3],D[5][4]}
    };

    if (E != D) 
    {
        std::cout << "problem with initializer list ctor!!!\n";
        return;
    }
    std::cout << "E:\n" << E << "\n";

    // copy op=
    E = A;
    std::cout << "E:\n" << E << "\n";

    // move op=
    E = {{6.1,5.2},{4.1,3.2},{2.2,1.3}}; // E changes from 6x5 to 3x2 
    std::cout << "E:\n" << E << "\n";

    try 
    {
        Matrix<double> F = {{1.1,2.2,3.3},{4.4,5.5}};
        std::cout << F << "\n";
    } 
    catch (std::exception &e) 
    {
        std::cout << e.what() << "\n";
    }
}

// test 4 for T == double: ctors, overloaded op[], op==, op!=, op+, op-, overloaded op*
void test4() 
{
    using size_type = Matrix<double>::size_type;

    std::default_random_engine defEngine(2);
    std::uniform_real_distribution<double> intDistro(-10.0,10.0);
    size_type rows{7}, cols{6};
    Matrix<double> A(rows, cols);

    for (size_type i{0}; i < rows; ++i) 
    {
        for (size_type j{0}; j < cols; ++j) 
        {
            A[i][j] = intDistro(defEngine);
        }
    }
    std::cout << "A:\n" << A << "\n";

    rows = 6; cols = 7;
    Matrix<double> B(rows, cols);

    for (size_type i{0}; i < rows; ++i) 
    {
        for (size_type j{0}; j < cols; ++j) 
        {
            B[i][j] = intDistro(defEngine);
        }
    }
    std::cout << "B:\n" << B << "\n";

    Matrix<double> C{std::move(matrix_generator<double>(A.get_rows(), A.get_cols()))};
    std::cout << "C:\n" << C << "\n";

    // impossible addition, subtraction, multiplication
    try 
    {
        Matrix<double> Z = A + B;
        std::cout << "Z:\n" << Z << "\n";
    } 
    catch (std::exception& e) 
    {
        std::cout << e.what() << "\n";
    }

    try 
    {
        Matrix<double> Z = A - B;
        std::cout << "Z:\n" << Z << "\n";
    } 
    catch (std::exception& e) 
    {
        std::cout << e.what() << "\n";
    }

    try 
    {
        Matrix<double> Z = A * C;
        std::cout << "Z:\n" << Z << "\n";
    }
    catch (std::exception& e) 
    {
        std::cout << e.what() << "\n";
    }

    Matrix<double> D = A * B; // 7x6 and 6x4 == 7x4
    std::cout << "D:\n" << D << "\n";
    D = A*(0.9*B);
    std::cout << "D:\n" << D << "\n";

    Matrix<double> E = A + C;
    std::cout << "E:\n" << E << "\n";
    E = A - C;
    std::cout << "E:\n" << E << "\n";

    Matrix<double> F = D - 0.5*D; 
    std::cout << "F:\n" << F << "\n";
}