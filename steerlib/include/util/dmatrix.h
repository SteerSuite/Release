//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __MATRIX_H__
#define __MATRIX_H__

#define _USE_MATH_DEFINES

#include <cmath>
#define MI_LOG2E    1.44269504088896340736
#define MI_PI       3.14159265358979323846
#include <float.h>
#include <iostream>
#include <assert.h>

#ifdef _WIN32
// #include<windows.h>
// #include<limits>
// #undef max()
// #undef identity()
#endif

struct mPair
{
	double x;
	double y;
};

class Matrix {

private:
  double *_elems;
  size_t _numRows;
  size_t _numColumns;

public:
  // constructors
  inline Matrix() {
    _numRows = 0;
    _numColumns = 0;
    _elems = 0;
  }
  inline Matrix(size_t numRows, size_t numColumns) {
    _numRows = numRows;
    _numColumns = numColumns;
    _elems = new double[_numRows * _numColumns];
  }
  inline Matrix(size_t numRows) {
    _numRows = numRows;
    _numColumns = 1;
    _elems = new double[_numRows];
  }
  inline Matrix(const Matrix& q) {
    _numRows = q._numRows;
    _numColumns = q._numColumns;
    _elems = new double[_numRows * _numColumns];
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      _elems[i] = q._elems[i];
    }
  }

  /**
   * The vector does not necessarily need to be rectangular but it would work
   * much better for using the matrix.
   */
  inline Matrix(const std::vector<std::vector<double>> & data)
  {
      _numRows = data.size();
      _numColumns = data.at(0).size();
      _elems = new double[_numRows * _numColumns];
      for (size_t i = 0; i < _numRows ; ++i)
      {
    	 for (size_t j = 0; j < _numColumns; j++)
    	 {
    		 _elems[i * _numColumns + j] = data.at(i).at(j);
    	 }
      }
    }

  // destructor
  inline ~Matrix() { 
    delete[] _elems;
  }
  
  // assignment
  Matrix& operator = (const Matrix& q) {
    if (_numRows * _numColumns != q._numRows * q._numColumns) {
      delete[] _elems;
      _elems = new double[q._numRows * q._numColumns];
    }
    _numRows = q._numRows;
    _numColumns = q._numColumns;
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      _elems[i] = q._elems[i]; 
    }
    return (*this);
  }

  // Retrieval
  inline size_t numRows() const { 
    return _numRows; 
  }
  inline size_t numColumns() const { 
    return _numColumns; 
  }
  
  // Subscript operator
  inline double& operator () (size_t row, size_t column) {
    assert(row < _numRows && column < _numColumns);
    return _elems[row * _numColumns + column]; 
  }
  inline double  operator () (size_t row, size_t column) const {
    assert(row < _numRows && column < _numColumns);
    return _elems[row * _numColumns + column]; 
  }

  inline double& operator [] (size_t elt) {
    assert(elt < _numRows * _numColumns);
    return _elems[elt]; 
  }

  inline double  operator [] (size_t elt) const
  {
	  /*
	  if ( elt >= _numRows * _numColumns )
	  {
		  int i=0;
	  }*/
	  // std::cout << "elt is " << elt << std::endl;
    assert(elt < _numRows * _numColumns);
    return _elems[elt]; 
  }

  // Reset to zeros
  inline void reset() { 
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      _elems[i] = double(0);
    }
  }

  // Submatrix
  inline Matrix subMatrix(size_t row, size_t column, size_t numRows, size_t numColumns) const {
    assert(row + numRows <= _numRows && column + numColumns <= _numColumns);
    Matrix m(numRows, numColumns);
    for (size_t i = 0; i < numRows; ++i) {
      for (size_t j = 0; j < numColumns; ++j) {
        m(i, j) = (*this)(row + i, column + j); /*_elems[(row + i) * _numColumns + (column + j)];*/
      }
    }
    return m;
  }

  // Insert
  inline void insert(size_t row, size_t column, const Matrix& q) {
    assert(row + q._numRows <= _numRows && column + q._numColumns <= _numColumns);
    for (size_t i = 0; i < q._numRows; ++i) {
      for (size_t j = 0; j < q._numColumns; ++j) {
        (*this)(row + i, column + j) = q(i, j);
      }
    }
  }
  
  // Unary minus
  inline Matrix operator-() const {
    Matrix m(_numRows, _numColumns);
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      m._elems[i] = -_elems[i];
    }
    return m;
  }
  
  // Unary plus
  inline const Matrix& operator+() const { 
    return *this;
  }

  // Equality
  inline bool operator==(const Matrix& q) const {
    if (_numRows != q._numRows || _numColumns != q._numColumns) {
      return false;
    }
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      if (_elems[i] != q._elems[i]) {
        return false;
      }
    }
    return true;
  }

  // Inequality
  inline bool operator!=(const Matrix& q) const {
    if (_numRows != q._numRows || _numColumns != q._numColumns) {
      return true;
    }
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      if (_elems[i] != q._elems[i]) {
        return true;
      }
    }
    return false;
  }

  // Matrix addition
  inline Matrix operator+(const Matrix& q) const {
    assert(_numRows == q._numRows && _numColumns == q._numColumns);
    Matrix m(_numRows, _numColumns);
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      m._elems[i] = _elems[i] + q._elems[i];
    }
    return m;
  }
  inline const Matrix& operator+=(const Matrix& q) { 
    assert(_numRows == q._numRows && _numColumns == q._numColumns);
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      _elems[i] += q._elems[i];
    }
    return *this; 
  }
  
  // Matrix subtraction
  inline Matrix operator-(const Matrix& q) const { 
    assert(_numRows == q._numRows && _numColumns == q._numColumns);
    Matrix m(_numRows, _numColumns);
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      m._elems[i] = _elems[i] - q._elems[i];
    }
    return m;
  }
  inline const Matrix& operator-=(const Matrix& q) { 
    assert(_numRows == q._numRows && _numColumns == q._numColumns);
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      _elems[i] -= q._elems[i];
    }
    return *this; 
  }

  // Scalar multiplication
  inline Matrix operator*(double a) const { 
    Matrix m(_numRows, _numColumns);
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      m._elems[i] = _elems[i] * a;
    }
    return m;
  }
  inline const Matrix& operator*=(double a) { 
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      _elems[i] *= a;
    }
    return *this;
  }
  
  // Scalar division
  inline Matrix operator/(double a) const { 
    Matrix m(_numRows, _numColumns);
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      m._elems[i] = _elems[i] / a;
    }
    return m;
  }
  inline const Matrix& operator/=(double a) { 
    for (size_t i = 0; i < _numRows * _numColumns; ++i) {
      _elems[i] /= a;
    }
    return *this;
  }

  // Matrix multiplication
  inline Matrix operator*(const Matrix& q) const {
    assert(_numColumns == q._numRows);
    Matrix m(_numRows, q._numColumns);
    for (size_t i = 0; i < _numRows; ++i) {
      for (size_t j = 0; j < q._numColumns; ++j) {
        double temp = double(0);
        for (size_t k = 0; k < _numColumns; ++k) {
          temp += (*this)(i, k) * q(k, j);
        }
        m(i, j) = temp;
      }
    }
    return m;
  }
  
  inline const Matrix& operator*=(const Matrix& q) { 
    return ((*this) = (*this) * q);
  }

  // Matrix transpose
  inline Matrix operator~() const {
    Matrix m(_numColumns, _numRows);
    for (size_t i = 0; i < _numColumns; ++i) {
      for (size_t j = 0; j < _numRows; ++j) {
        m(i, j) = (*this)(j, i);
      }
    }
    return m;
  }

  // Solve this*X=q for q (faster and more accurate than !this*q)
  inline Matrix operator%(const Matrix& q) const {
    assert(_numRows == _numColumns && _numRows == q._numRows);
    size_t _size = _numRows;
    Matrix m(*this);
    Matrix inv(q);
        
    size_t* row_p = new size_t[_size];
    size_t* col_p = new size_t[_size];
    for (size_t i = 0; i < _size; ++i) {
      row_p[i] = i; col_p[i] = i;
    }

    // Gaussian elimination
    for (size_t k = 0; k < _size; ++k)
    {
      // find maximal pivot element
      double maximum = double(0); size_t max_row = k; size_t max_col = k;
      for (size_t i = k; i < _size; ++i)
      {
        for (size_t j = k; j < _size; ++j)
        {
          double abs_ij = fabs(m(row_p[i], col_p[j]));
          if (abs_ij > maximum)
          {
            maximum = abs_ij; max_row = i; max_col = j;
            // std::cout << "found a max " << maximum << std::endl;
          }
        }
      }

      /*
      if ( maximum == double(0))
      {
    	  std::cout << "Matrix thing: ";
    	  for (size_t i = 0; i < m.numRows(); ++i) {
    	      for (size_t j = 0; j < m.numColumns(); ++j) {
    	    	  if (m(i,j) == double(0))
    	    	  {
    	    		  std::cout << "found 0 at :" << i << "," << j << std::endl;
    	    	  }
    	    	  // std::cout << m(i,j) << "\t";
    	      }
    	      std::cout << std::endl;
    	    }
    	  std::cout << std::endl;
      }*/

      // I think this means there are some rows/columns that are not independent.
      assert(maximum != double(0));

      // swap rows and columns
      if (k != max_row) {
        size_t swap = row_p[k]; row_p[k] = row_p[max_row]; row_p[max_row] = swap;
      }
      if (k != max_col) {
        size_t swap = col_p[k]; col_p[k] = col_p[max_col]; col_p[max_col] = swap;
      }

      // eliminate column
      for (size_t i = k + 1; i < _size; ++i) {
        double factor = m(row_p[i], col_p[k]) / m(row_p[k], col_p[k]);
        for (size_t j = k + 1; j < _size; ++j) {
          m(row_p[i], col_p[j]) -= factor * m(row_p[k], col_p[j]);
        }
        for (size_t j = 0; j < inv._numColumns; ++j) {
          inv(row_p[i], j) -= factor * inv(row_p[k], j);
        }
      } 
    }

    // Backward substitution
    for (size_t k = _size - 1; k != -1; --k) {
      double quotient = m(row_p[k], col_p[k]);
      for (size_t j = 0; j < inv._numColumns; ++j) {
        inv(row_p[k], j) /= quotient;
      }
      
      for (size_t i = 0; i < k; ++i) {
        double factor = m(row_p[i], col_p[k]);
        for (size_t j = 0; j < inv._numColumns; ++j) {
          inv(row_p[i], j) -= factor * inv(row_p[k], j);
        }
      } 
    }

    // reshuffle result
    Matrix result(inv._numRows, inv._numColumns);
    for (size_t i = 0; i < _size; ++i) {
      for (size_t j = 0; j < inv._numColumns; ++j) {
        result(col_p[i], j) = inv(row_p[i], j);
      }
    }

    delete[] row_p;
    delete[] col_p;
    return result; 
  }

  // Solve this=X*q for X (faster and more accurate than this*!q)
  inline Matrix operator/(const Matrix& q) const {
    return ~(~q%~(*this));
  }

};

// Identity matrix
inline Matrix m_identity(size_t size) {
  Matrix m(size, size);
  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; ++j) {
      m(i, j) = (i == j ? double(1) : double(0));
    }
  }
  return m;
}

// Zero matrix
inline Matrix zeros(size_t numRows, size_t numColumns) {
  Matrix m(numRows, numColumns);
  m.reset();
  return m;
}

inline Matrix zeros(size_t numRows) {
  Matrix m(numRows, 1);
  m.reset();
  return m;
}

// Matrix inverse
inline Matrix operator!(const Matrix& q) {
  assert(q.numRows() == q.numColumns());
  size_t _size = q.numRows();
  Matrix m(q);
  Matrix inv = m_identity(_size);
  
  size_t* row_p = new size_t[_size];
  size_t* col_p = new size_t[_size];
  for (size_t i = 0; i < _size; ++i) {
    row_p[i] = i; col_p[i] = i;
  }

  // Gaussian elimination
  for (size_t k = 0; k < _size; ++k) {
    // find maximal pivot element
    double maximum = double(0); size_t max_row = k; size_t max_col = k;
    for (size_t i = k; i < _size; ++i) {
      for (size_t j = k; j < _size; ++j) {
        double abs_ij = fabs(m(row_p[i], col_p[j]));
        if (abs_ij > maximum) {
          maximum = abs_ij; max_row = i; max_col = j;
        }
      }
    }

    assert(maximum != double(0));

    // swap rows and columns
    if (k != max_row) {
      size_t swap = row_p[k]; row_p[k] = row_p[max_row]; row_p[max_row] = swap;
    }
    if (k != max_col) {
      size_t swap = col_p[k]; col_p[k] = col_p[max_col]; col_p[max_col] = swap;
    }

    // eliminate column
    for (size_t i = k + 1; i < _size; ++i) {
      double factor = m(row_p[i], col_p[k]) / m(row_p[k], col_p[k]);
      for (size_t j = k + 1; j < _size; ++j) {
        m(row_p[i], col_p[j]) -= factor * m(row_p[k], col_p[j]);
      }
      for (size_t j = 0; j < k; ++j) {
        inv(row_p[i], row_p[j]) -= factor * inv(row_p[k], row_p[j]);
      }
      inv(row_p[i], row_p[k]) = -factor;
    } 
  }

  // Backward substitution
  for (size_t k = _size - 1; k != -1; --k) {
    double quotient = m(row_p[k], col_p[k]);
    for (size_t j = 0; j < _size; ++j) {
      inv(row_p[k], j) /= quotient;
    }
    
    for (size_t i = 0; i < k; ++i) {
      double factor = m(row_p[i], col_p[k]);
      for (size_t j = 0; j < _size; ++j) {
        inv(row_p[i], j) -= factor * inv(row_p[k], j);
      }
    } 
  }

  // reshuffle result
  for (size_t i = 0; i < _size; ++i) {
    for (size_t j = 0; j < _size; ++j) {
      m(col_p[i], j) = inv(row_p[i], j);
    }
  }

  delete[] row_p;
  delete[] col_p;
  return m; 
}

// Scalar multiplication 
inline Matrix operator*(double a, const Matrix& q) { return q*a; }

// Matrix trace
inline double tr(const Matrix& q) {
  assert(q.numRows() == q.numColumns());
  double trace = double(0);
  for (size_t i = 0; i < q.numRows(); ++i){
    trace += q(i, i);
  }
  return trace;
}

// Matrix diagonal
inline Matrix diag(const Matrix & q) { 
	Matrix m(q.numRows());
	for (size_t i = 0; i < q.numRows(); ++i){
		m[i] = q(i, i);
	}
	return m;
}

// Matrix diagonal
inline Matrix abs(const Matrix & q)
{
	Matrix m(q);
	size_t els = q.numColumns()*q.numRows();
	for (size_t i = 0; i < els; ++i)
	{
		m[i] = fabs(q[i]);
	}
	return m;
}

// Matrix 1-norm
inline double norm(const Matrix& q) {
  double norm1 = double(0);
  for (size_t j = 0; j < q.numColumns(); ++j) {
    double colabssum = double(0);
    for (size_t i = 0; i < q.numRows(); ++i) {
      colabssum += fabs(q(i,j));
    }
    if (colabssum > norm1) {
      norm1 = colabssum;
    }
  }
  return norm1;
}

// Matrix determinant
inline double det(const Matrix& q) {
  assert(q.numRows() == q.numColumns());
  Matrix m(q);
  double D = double(1);

  size_t _size = q.numRows();
  size_t* row_p = new size_t[_size];
  size_t* col_p = new size_t[_size];
  for (size_t i = 0; i < _size; ++i) {
    row_p[i] = i; col_p[i] = i;
  }

  // Gaussian elimination
  for (size_t k = 0; k < _size; ++k) {
    // find maximal pivot element
    double maximum = double(0); size_t max_row = k; size_t max_col = k;
    for (size_t i = k; i < _size; ++i) {
      for (size_t j = k; j < _size; ++j) {
        double abs_ij = std::abs(m(row_p[i], col_p[j]));
        if (abs_ij > maximum) {
          maximum = abs_ij; max_row = i; max_col = j;
        }
      }
    }
         
    // swap rows and columns
    if (k != max_row) {
      size_t swap = row_p[k]; row_p[k] = row_p[max_row]; row_p[max_row] = swap;
      D = -D;
    }
    if (k != max_col) {
      size_t swap = col_p[k]; col_p[k] = col_p[max_col]; col_p[max_col] = swap;
      D = -D;
    }

    D *= m(row_p[k], col_p[k]);
    if (D == double(0)) {
      delete[] row_p;
      delete[] col_p;
      return double(0);
    }

    // eliminate column
    for (size_t i = k + 1; i < _size; ++i) {
      double factor = m(row_p[i], col_p[k]) / m(row_p[k], col_p[k]);
      for (size_t j = k + 1; j < _size; ++j) {
        m(row_p[i], col_p[j]) -= factor * m(row_p[k], col_p[j]);
      }
    }  
  }

  delete[] row_p;
  delete[] col_p;
  return D;
}

// Matrix pseudo-inverse
inline Matrix pseudoInverse(const Matrix& q) { 
  Matrix m(q);

  size_t _numRows = q.numRows();
  size_t _numColumns = q.numColumns();
  
  size_t* row_p = new size_t[_numRows];
  size_t* col_p = new size_t[_numColumns];
  for (size_t i = 0; i < _numRows; ++i) {
    row_p[i] = i;
  }
  for (size_t i = 0; i < _numColumns; ++i) {
    col_p[i] = i;
  }

  // Gaussian elimination to determine rank
  size_t rank = (_numRows < _numColumns ? _numRows : _numColumns);
  for (size_t k = 0; k < rank; ++k) {
    // find maximal pivot element
    double maximum = double(0); size_t max_row = k; size_t max_col = k;
    for (size_t i = k; i < _numRows; ++i) {
      for (size_t j = k; j < _numColumns; ++j) {
        double abs_ij = fabs(m(row_p[i], col_p[j]));
        if (abs_ij > maximum) {
          maximum = abs_ij; max_row = i; max_col = j;
        }
      }
    }

    // check if zero
    if (maximum <= DBL_EPSILON) {
      rank = k;
      break;
    }

    // swap rows and columns
    if (k != max_row) {
      size_t swap = row_p[k]; row_p[k] = row_p[max_row]; row_p[max_row] = swap;
    }
    if (k != max_col) {
      size_t swap = col_p[k]; col_p[k] = col_p[max_col]; col_p[max_col] = swap;
    }

    // eliminate column
    for (size_t i = k + 1; i < _numRows; ++i) {
      double factor = m(row_p[i], col_p[k]) / m(row_p[k], col_p[k]);
      for (size_t j = k + 1; j < _numColumns; ++j) {
        m(row_p[i], col_p[j]) -= factor * m(row_p[k], col_p[j]);
      }
    } 
  }

  if (rank == 0) { // zero or empty matrix
    delete[] row_p; delete[] col_p;
    return zeros(_numColumns, _numRows);
  } else if (rank == _numRows && rank == _numColumns) { // full rank square matrix
    delete[] row_p; delete[] col_p;
    return !q;
  } else if (rank == _numRows) { // full row-rank matrix
    delete[] row_p; delete[] col_p;
    return ~q/(q*~q);
  } else if (rank == _numColumns) { // full column-rank matrix
    delete[] row_p; delete[] col_p;
    return (~q*q)%~q;
  } // else: not full rank, perform rank decomposition

  // bring m into reduced row echelon form
  
  // normalize rows such that 1's appear on the diagonal
  for (size_t k = 0; k < rank; ++k) {
    double quotient = m(row_p[k], col_p[k]);
    for (size_t j = 0; j < k; ++j) {
      m(row_p[k], col_p[j]) = double(0);
    }
    m(row_p[k], col_p[k]) = double(1);
    for (size_t j = k + 1; j < _numColumns; ++j) {
      m(row_p[k], col_p[j]) /= quotient;
    }
  }
  
  // subtract rows such that 0's appear above leading row elements
  for (size_t k = rank - 1; k != -1; --k) {
    for (size_t i = 0; i < k; ++i) {
      double factor = m(row_p[i], col_p[k]);
      m(row_p[i], col_p[k]) = double(0);
      for (size_t j = k + 1; j < _numColumns; ++j) {
        m(row_p[i], col_p[j]) -= factor * m(row_p[k], col_p[j]);
      }
    } 
  }

  // copy m into smaller matrix C and swap columns
  Matrix C(rank, _numColumns);
  for (size_t k = 0; k < rank; ++k) {
    for (size_t j = 0; j < _numColumns; ++j) {
      C(k,j) = m(row_p[k], j);
    }
  }
  
  // B is copy of A with columns swapped and non-pivot columns left out
  Matrix B(_numRows, rank);
  for (size_t k = 0; k < _numRows; ++k) {
    for (size_t j = 0; j < rank; ++j ) {
      B(k,j) = q(k, col_p[j]);
    }
  }

  // Now, q = B*C and B and C are of full row/column rank
  delete[] row_p; delete[] col_p;
  return ~C*(((~B*B)*(C*~C))%~B);
}

inline void jacobi(const Matrix& q, Matrix& V, Matrix& D) {
  assert(q.numRows() == q.numColumns());
  size_t _size = q.numRows();
  D = q;
  V = m_identity(_size);
  
  while (true) {
    double maximum = 0; size_t max_row = 0; size_t max_col = 0;
    for (size_t i = 0; i < _size; ++i) {
      for (size_t j = i + 1; j < _size; ++j) {
        if (fabs(D(i,j)) > maximum) {
          maximum = fabs(D(i,j));
          max_row = i;
          max_col = j;
        }
      }
    }

    if (maximum <= DBL_EPSILON) {
      break;
    }

    double theta = (D(max_col, max_col) - D(max_row, max_row)) / (2 * D(max_row, max_col));
    double t = 1 / (fabs(theta) + sqrt(theta*theta+1));
    if (theta < 0) t = -t;
    double c = 1 / sqrt(t*t+1); 
    double s = c*t;

    Matrix R(m_identity(_size));
    R(max_row,max_row) = c;
    R(max_col,max_col) = c;
    R(max_row,max_col) = s;
    R(max_col,max_row) = -s;

    // update D // 
    double temp1 = c*c*D(max_row, max_row) + s*s*D(max_col, max_col) - 2*c*s*D(max_row, max_col);
    double temp2 = s*s*D(max_row, max_row) + c*c*D(max_col, max_col) + 2*c*s*D(max_row, max_col);
    D(max_row, max_col) = 0;
    D(max_col, max_row) = 0;
    D(max_row, max_row) = temp1;
    D(max_col, max_col) = temp2;
    for (size_t j = 0; j < _size; ++j) {
      if ((j != max_row) && (j != max_col)) {
        temp1 = c * D(j, max_row) - s * D(j, max_col);
        temp2 = c * D(j, max_col) + s * D(j, max_row);
        D(j, max_row) = (D(max_row, j) = temp1);
        D(j, max_col) = (D(max_col, j) = temp2);
      }
    }
    //std::cout << D << std::endl << std::endl;
    

    V = V * R;
  } 
}


// Matrix exponentiation
#define _B0 1729728e1
#define _B1 864864e1
#define _B2 199584e1
#define _B3 2772e2
#define _B4 252e2
#define _B5 1512e0
#define _B6 56e0
#define _B7 1e0
#define _NORMLIM 9.504178996162932e-1

inline Matrix exp(const Matrix& q) {
  assert(q.numRows() == q.numColumns());
  size_t _size = q.numRows();
  Matrix A(q);
  int s = (int) max(double(0), ceil(log(norm(A)/_NORMLIM)*MI_LOG2E)); 

  A /= pow(2.0,s);
  Matrix A2(A*A);
  Matrix A4(A2*A2);
  Matrix A6(A2*A4);
  Matrix U( A*(A6*_B7 + A4*_B5 + A2*_B3 + m_identity(_size)*_B1) );
  Matrix V( A6*_B6 + A4*_B4 + A2*_B2 + m_identity(_size)*_B0 ); 
  Matrix R7 = (V - U)%(V + U);

  for (int i = 0; i < s; ++i) {
    R7 *= R7;
  }
  return R7;
}

// Input stream
inline std::istream& operator>>(std::istream& is, Matrix& q) {
  for (size_t i = 0; i < q.numRows(); ++i) {
    for (size_t j = 0; j < q.numColumns(); ++j) {
      is >> q(i,j);
    }
  }
  return is;
}

// Output stream
inline std::ostream& operator<<(std::ostream& os, const Matrix& q) {
  for (size_t i = 0; i < q.numRows(); ++i) {
    for (size_t j = 0; j < q.numColumns(); ++j) {
      os << q(i,j) << "\t";
    }
    os << std::endl;
  }
  return os;
}

// Hilbert matrix
inline Matrix hilbert(size_t size) {
  Matrix m(size, size);
  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; ++j) {
      m(i, j) = double(1) / (double) (i + j + 1);
    }
  }
  return m;
}

#endif
