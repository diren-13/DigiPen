/************************************************************************************//*!
\file           matrix.hpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           Oct 28, 2021
\brief          Contains the implementation of the Matrix class.
 
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <initializer_list>
#include <algorithm>            // std::copy
#include <iterator>
#include <exception>            // std::runtime_error
#include <memory>               // std::unique_ptr
#include <type_traits>          // std::is_floating_point_v
#include <numeric>              // std::numeric_limits::epsilon

namespace csd2125 
{
    /*---------------------------------------------------------------------------------*/
    /* Class Definitions                                                               */
    /*---------------------------------------------------------------------------------*/ 
    template <typename T>
    class Matrix
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Aliases                                                                */
        /*-----------------------------------------------------------------------------*/ 
        using value_type        = T;
        using size_type         = size_t;
        using reference         = value_type&;
        using const_reference   = const reference;
        using pointer           = std::unique_ptr<value_type[]>;
        using const_pointer     = const pointer;

    public:
        /*-----------------------------------------------------------------------------*/
        /* Proxy Class Definitions                                                     */
        /*-----------------------------------------------------------------------------*/ 
        class Proxy
        {
        public:
            /*-------------------------------------------------------------------------*/
            /* Constructors                                                            */
            /*-------------------------------------------------------------------------*/ 
            Proxy() = delete;
            /************************************************************************//*!
            @brief  Constructor for Proxy.

            @param  m
                The matrix for the proxy to assign as the reference.
            @param  r
                The row the proxy is handling
            *//*************************************************************************/
            Proxy(Matrix<value_type>& m, size_type r);

            /*-------------------------------------------------------------------------*/
            /* Operator Overloads                                                      */
            /*-------------------------------------------------------------------------*/ 
            /************************************************************************//*!
            @brief      Subscript operator overload for the proxy. Used as a proxy to allow
                        double subscript operator calls on Matrix.

            @param      c
                The column to retrieve the matrix element from

            @returns    The value located at the Matrix[row][c]
            *//*************************************************************************/
            value_type& operator[](size_type c);
        private:
            /*-------------------------------------------------------------------------*/
            /* Data Members                                                            */
            /*-------------------------------------------------------------------------*/ 
            Matrix<value_type>& mtx;
            size_type           row;
        };

        class ConstProxy
        {
        public:
            /*-------------------------------------------------------------------------*/
            /* Constructors                                                            */
            /*-------------------------------------------------------------------------*/ 
            ConstProxy() = delete;
            /************************************************************************//*!
            @brief  Constructor for ConstProxy.

            @param  m
                The matrix for the proxy to assign as the const reference.
            @param  r
                The row the proxy is handling
            *//*************************************************************************/
            ConstProxy(const Matrix& m, size_type r);

            /*-------------------------------------------------------------------------*/
            /* Operator Overloads                                                      */
            /*-------------------------------------------------------------------------*/ 
            /************************************************************************//*!
            @brief      Subscript operator overload for the proxy. Used as a proxy to allow
                        double subscript operator calls on Matrix.

            @param      c
                The column to retrieve the matrix element from

            @returns    The value located at the Matrix[row][c]
            *//*************************************************************************/
            const value_type& operator[](size_type c) const;
        private:
            /*-------------------------------------------------------------------------*/
            /* Data Members                                                            */
            /*-------------------------------------------------------------------------*/ 
            const Matrix&   mtx;
            size_type       row;
        };

    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/ 
        Matrix() = delete;

        /****************************************************************************//*!
        @brief  Constructor for Matrix. Dynamically allocates storage.

        @param  nr
            The number of rows the matrix will have.
        @param  nc
            The number of columns the matrix will have.
        *//*****************************************************************************/
        Matrix(size_type nr, size_type nc);

        /****************************************************************************//*!
        @brief  Constructor for Matrix. Dynamically allocates storage

        @param  list
            A list of data to store in the matrix.
        *//*****************************************************************************/
        Matrix(const std::initializer_list<std::initializer_list<value_type>>& list);

        /****************************************************************************//*!
        @brief  Copy Constructor for Matrix. A deep copy the matrix is made.

        @param  rhs
            The matrix to copy the data from.
        *//*****************************************************************************/
        Matrix(const Matrix<value_type>& rhs);

        /****************************************************************************//*!
        @brief  Move Constructor for Matrix.

        @param  rhs
            The matrix to move the data from.
        *//*****************************************************************************/
        Matrix(Matrix<value_type>&& rhs) noexcept;

        /****************************************************************************//*!
        @brief  Destructor for Matrix
        *//*****************************************************************************/
        ~Matrix() noexcept;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/ 
        /****************************************************************************//*!
        @brief      Get the number of rows in the Matrix.

        @returns    The number of rows in the Matrix.
        *//*****************************************************************************/
        size_type get_rows() const noexcept;

        /****************************************************************************//*!
        @brief      Get the number of columns in the Matrix.

        @returns    The number of columns in the Matrix.
        *//*****************************************************************************/
        size_type get_cols() const noexcept;

        /*-----------------------------------------------------------------------------*/
        /* Operator Overloads                                                          */
        /*-----------------------------------------------------------------------------*/ 
        /****************************************************************************//*!
        @brief      Assigns a copy of another Matrix to this Matrix.

        @param[in]  rhs
            A Matrix to copy the data from.

        @returns    A reference to this Matrix with the copied data.
        *//*****************************************************************************/
        Matrix<value_type>& operator=(const Matrix<value_type>& rhs);

        /****************************************************************************//*!
        @brief      Moves data from another Matrix to this Matrix.

        @param[in]  rhs
            A Matrix to move the data from.

        @returns    A reference to this Matrix with the moved data.
        *//*****************************************************************************/
        Matrix<value_type>& operator=(Matrix<value_type>&& rhs) noexcept;

        /****************************************************************************//*!
        @brief      Gets a row in the Matrix.

        @param[in]  r
            The row to access

        @returns    A proxy that holds the row of the Matrix.
        *//*****************************************************************************/
        Proxy operator[](size_type r);

        /****************************************************************************//*!
        @brief      Gets a row in the Matrix.

        @param[in]  r
            The row to access

        @returns    A read-only that holds the row of the Matrix.
        *//*****************************************************************************/
        ConstProxy operator[](size_type r) const;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/ 
        size_type   rows; 
        size_type   cols;
        pointer     data;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/ 
        /****************************************************************************//*!
        @brief      Internal swap function for copy swap idiom.

        @param[in]  lhs
            A Matrix.
        @param[in]  rhs
            A Matrix.
        *//*****************************************************************************/
        static void swap(Matrix<value_type>& lhs, Matrix<value_type>& rhs);
    };

    /*---------------------------------------------------------------------------------*/
    /* Global Function Declarations                                                    */
    /*---------------------------------------------------------------------------------*/
    /****************************************************************************//*!
    @brief      Adds two matrices together.

    @param[in]  lhs
        A Matrix.
    @param[in]  rhs
        A Matrix.

    @returns    The result of the addition of the two matrices
    *//*****************************************************************************/
    template <typename T>
    Matrix<T> operator+(const Matrix<T>& lhs, const Matrix<T>& rhs);

    /****************************************************************************//*!
    @brief      Subtracts one matrix from another.

    @param[in]  lhs
        A Matrix to subtract from.
    @param[in]  rhs
        A Matrix used for subtraction.

    @returns    The result of lhs - rhs.
    *//*****************************************************************************/
    template <typename T>
    Matrix<T> operator-(const Matrix<T>& lhs, const Matrix<T>& rhs);

    /****************************************************************************//*!
    @brief      Multiplies two matrices.

    @param[in]  lhs
        A Matrix.
    @param[in]  rhs
        A Matrix.

    @returns    The result of the multiplication of the two matrices
    *//*****************************************************************************/
    template <typename T>
    Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs);

    /****************************************************************************//*!
    @brief      Multiplies each element in the Matrix by a value.

    @param[in]  lhs
        A value matching the value_type of the matrix
    @param[in]  rhs
        A Matrix.

    @returns    The result of the multiplication.
    *//*****************************************************************************/
    template <typename T>
    Matrix<T> operator*(typename Matrix<T>::value_type lhs, const Matrix<T>& rhs);

    /****************************************************************************//*!
    @brief      Compares two matrices for equality.

    @param[in]  lhs
        A Matrix.
    @param[in]  rhs
        A Matrix.

    @returns    True if all elements of the matrices are equal.
    *//*****************************************************************************/
    template <typename T>
    bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs);

    /****************************************************************************//*!
    @brief      Compares two matrices for inequality.

    @param[in]  lhs
        A Matrix.
    @param[in]  rhs
        A Matrix.

    @returns    True if any element of the matrices are not equal.
    *//*****************************************************************************/
    template <typename T>
    bool operator!=(const Matrix<T>& lhs, const Matrix<T>& rhs);

    /*---------------------------------------------------------------------------------*/
    /* Matrix Constructors & Destructor Definitions                                    */
    /*---------------------------------------------------------------------------------*/
    template <typename T>
    Matrix<T>::Matrix(size_type nr, size_type nc)
    : rows {nr}
    , cols {nc}
    , data {std::make_unique<value_type[]>(rows * cols)}
    {}

    template <typename T>
    Matrix<T>::Matrix(const std::initializer_list<std::initializer_list<T>>& list)
    try
    : rows {list.size()}
    , cols {list.begin()->size()}
    , data {std::make_unique<value_type[]>(rows * cols)}
    {
        auto tmp = data.get();

        for (auto& r : list)
        {
            // If rows have unequal number of columns, list is invalid
            if (r.size() != cols) { throw std::runtime_error(""); }

            for (auto& c : r)
            {
                *tmp++ = c;
            }
        }
    }
    catch (std::runtime_error& re)
    {
        // Delete allocated memory
        data.reset();
        // Rethrow
        throw std::runtime_error("bad initializer list");
    }

    template <typename T>
    Matrix<T>::Matrix(const Matrix<T>& rhs)
    : rows {rhs.rows}
    , cols {rhs.cols}
    , data {std::make_unique<value_type[]>(rows * cols)}
    {
        // Copy data over
        size_type size = rows * cols;
        std::copy(rhs.data.get(), rhs.data.get() + size, data.get());
    }

    template <typename T>
    Matrix<T>::Matrix(Matrix<T>&& rhs) noexcept
    : rows {rhs.rows}
    , cols {rhs.cols}
    , data {std::move(rhs.data)}
    {
        // Empty rhs
        rhs.data = nullptr;
        rhs.rows = 0;
        rhs.cols = 0;
    }

    template <typename T>
    Matrix<T>::~Matrix() noexcept
    {
        data.reset();
    }

    /*---------------------------------------------------------------------------------*/
    /* Matrix Member Function Definitions                                              */
    /*---------------------------------------------------------------------------------*/
    template <typename T>
    typename Matrix<T>::size_type Matrix<T>::get_rows() const noexcept
    {
        return rows;
    }

    template <typename T>
    typename Matrix<T>::size_type Matrix<T>::get_cols() const noexcept
    {
        return cols;
    }

    template <typename T>
    void Matrix<T>::swap(Matrix<T>& lhs, Matrix<T>& rhs)
    {
        std::swap(lhs.rows, rhs.rows);
        std::swap(lhs.cols, rhs.cols);
        std::swap(lhs.data, rhs.data);
    }

    /*---------------------------------------------------------------------------------*/
    /* Matrix Operator Overloads Definitions                                           */
    /*---------------------------------------------------------------------------------*/
    template <typename T>
    Matrix<T>& Matrix<T>::operator=(const Matrix<T>& rhs)
    {
        // Use copy-swap idiom
        Matrix<T> tmp = rhs;
        swap(*this, tmp);
        return *this;
    }

    template <typename T>
    Matrix<T>& Matrix<T>::operator=(Matrix<T>&& rhs) noexcept
    {
        // Free existing data
        data.reset();

        // Copy data over
        rows = rhs.rows;
        cols = rhs.cols;
        data = std::move(rhs.data);

        // Empty rhs
        rhs.data = nullptr;
        rhs.rows = 0;
        rhs.cols = 0;
        
        return *this;
    }

    template <typename T>
    typename Matrix<T>::Proxy Matrix<T>::operator[](typename Matrix<T>::size_type r)
    {
        if (r >= rows)
        {
            throw std::out_of_range("row is out of range");
        }

        return Matrix<T>::Proxy{*this, r};
    }

    template <typename T>
    typename Matrix<T>::ConstProxy Matrix<T>::operator[](typename Matrix<T>::size_type r) const
    {
        if (r >= rows)
        {
            throw std::out_of_range("row is out of range");
        }

        return Matrix<T>::ConstProxy{*this, r};
    }

    /*---------------------------------------------------------------------------------*/
    /* Matrix Proxy Function Definitions                                               */
    /*---------------------------------------------------------------------------------*/
    template <typename T>
    Matrix<T>::Proxy::Proxy(Matrix<T>& m, size_type r)
    : mtx {m}
    , row {r}
    {}

    template <typename T>
    typename Matrix<T>::value_type& Matrix<T>::Proxy::operator[](typename Matrix<T>::size_type c)
    {
        if (c >= mtx.cols)
        {
            throw std::out_of_range("column is out of range");
        }

        size_type offset = row * mtx.cols + c;
        return *(mtx.data.get() + offset);
    }

    /*---------------------------------------------------------------------------------*/
    /* Matrix Const Proxy Function Definitions                                         */
    /*---------------------------------------------------------------------------------*/
    template <typename T>
    Matrix<T>::ConstProxy::ConstProxy(const Matrix<T>& m, size_type r)
    : mtx {m}
    , row {r}
    {}

    template <typename T>
    const typename Matrix<T>::value_type& Matrix<T>::ConstProxy::operator[](typename Matrix<T>::size_type c) const
    {
        if (c >= mtx.cols)
        {
            throw std::out_of_range("column is out of range");
        }

        size_type offset = row * mtx.cols + c;
        return *(mtx.data.get() + offset);
    }

    /*---------------------------------------------------------------------------------*/
    /* Global Function Definitions                                                     */
    /*---------------------------------------------------------------------------------*/
    template <typename T>
    Matrix<T> operator+(const Matrix<T>& lhs, const Matrix<T>& rhs)
    {
        using sizeType = typename Matrix<T>::size_type;
        sizeType lhsNumRows = lhs.get_rows();
        sizeType lhsNumCols = lhs.get_cols();
        sizeType rhsNumRows = rhs.get_rows();
        sizeType rhsNumCols = rhs.get_cols();

        // Check if number of rows and columns are the same. 
        // If sizes are different, return false.
        if (lhsNumRows != rhsNumRows || lhsNumCols != rhsNumCols)
        {
            throw std::runtime_error("operands for matrix addition must have same dimensions");
        }

        Matrix<T> result(lhsNumRows, lhsNumCols);
        for (sizeType r = 0; r < lhsNumRows; ++r)
        {
            for (sizeType c = 0; c < lhsNumCols; ++c)
            {
                result[r][c] = lhs[r][c] + rhs[r][c];
            }
        }
        return result;
    }

    template <typename T>
    Matrix<T> operator-(const Matrix<T>& lhs, const Matrix<T>& rhs)
    {
        using sizeType = typename Matrix<T>::size_type;
        sizeType lhsNumRows = lhs.get_rows();
        sizeType lhsNumCols = lhs.get_cols();
        sizeType rhsNumRows = rhs.get_rows();
        sizeType rhsNumCols = rhs.get_cols();

        // Check if number of rows and columns are the same. 
        // If sizes are different, return false.
        if (lhsNumRows != rhsNumRows || lhsNumCols != rhsNumCols)
        {
            throw std::runtime_error("operands for matrix subtraction must have same dimensions");
        }

        Matrix<T> result(lhsNumRows, lhsNumCols);
        for (sizeType r = 0; r < lhsNumRows; ++r)
        {
            for (sizeType c = 0; c < lhsNumCols; ++c)
            {
                result[r][c] = lhs[r][c] - rhs[r][c];
            }
        }
        return result;
    }

    template <typename T>
    Matrix<T> operator*(const Matrix<T>& lhs, const Matrix<T>& rhs)
    {
        using sizeType = typename Matrix<T>::size_type;
        sizeType lhsNumRows = lhs.get_rows();
        sizeType lhsNumCols = lhs.get_cols();
        sizeType rhsNumRows = rhs.get_rows();
        sizeType rhsNumCols = rhs.get_cols();

        // Check if number of rows and columns are the same. 
        // If sizes are different, return false.
        if (lhsNumCols != rhsNumRows)
        {
            throw std::runtime_error("number of columns in left operand must match number of rows in right operand");
        }

        Matrix<T> result(lhsNumRows, rhsNumCols);
        for(sizeType r = 0; r < lhsNumRows; ++r)
        {
            for(sizeType c = 0; c < rhsNumCols; ++c)
            {
                for(sizeType n = 0; n < lhsNumCols; ++n)
                {
                    result[r][c] += lhs[r][n] * rhs[n][c];
                }
            }
        }
        return result;
    }

    template <typename T>
    Matrix<T> operator*(typename Matrix<T>::value_type lhs, const Matrix<T>& rhs)
    {
        using sizeType = typename Matrix<T>::size_type;
        sizeType numRows = rhs.get_rows();
        sizeType numCols = rhs.get_cols();

        Matrix<T> result(numRows, numCols);
        for (sizeType r = 0; r < numRows; ++r)
        {
            for (sizeType c = 0; c < numCols; ++c)
            {
                result[r][c] = lhs * rhs[r][c];
            }
        }
        return result;
    }

    template <typename T>
    bool operator==(const Matrix<T>& lhs, const Matrix<T>& rhs)
    {
        using sizeType = typename Matrix<T>::size_type;
        sizeType lhsNumRows = lhs.get_rows();
        sizeType lhsNumCols = lhs.get_cols();
        sizeType rhsNumRows = rhs.get_rows();
        sizeType rhsNumCols = rhs.get_cols();

        // Check if number of rows and columns are the same. 
        // If sizes are different, return false.
        if (lhsNumRows != rhsNumRows || lhsNumCols != rhsNumCols)
        {
            return false;
        }

        for (sizeType r = 0; r < lhsNumRows; ++r)
        {
            for (sizeType c = 0; c < lhsNumCols; ++c)
            {
                if constexpr (std::is_floating_point_v<T>)
                {
                    T diff = std::abs(lhs[r][c] - rhs[r][c]);
                    if (diff > std::numeric_limits<T>::epsilon())
                    {
                        return false;
                    }
                }
                else
                {
                    if (lhs[r][c] != rhs[r][c])
                    {
                        return false;
                    }
                }                
            }
        }

        return true;
    }

    template <typename T>
    bool operator!=(const Matrix<T>& lhs, const Matrix<T>& rhs)
    {
        using sizeType = typename Matrix<T>::size_type;
        sizeType lhsNumRows = lhs.get_rows();
        sizeType lhsNumCols = lhs.get_cols();
        sizeType rhsNumRows = rhs.get_rows();
        sizeType rhsNumCols = rhs.get_cols();

        // Check if number of rows and columns are the same. 
        // If sizes are different, return true.
        if (lhsNumRows != rhsNumRows || lhsNumCols != rhsNumCols)
        {
            return true;
        }

        for (sizeType r = 0; r < lhsNumRows; ++r)
        {
            for (sizeType c = 0; c < lhsNumCols; ++c)
            {
                if constexpr (std::is_floating_point_v<T>)
                {
                    T diff = std::abs(lhs[r][c] - rhs[r][c]);
                    if (diff <= std::numeric_limits<T>::epsilon())
                    {
                        return false;
                    }
                }
                else
                {
                    if (lhs[r][c] == rhs[r][c])
                    {
                        return false;
                    }
                } 
            }
        }

        return true;
    }

}   // namespace csd2125

#endif

