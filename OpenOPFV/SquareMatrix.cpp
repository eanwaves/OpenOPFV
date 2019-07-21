/***********************************************************************
 * Copyright (c) 2014 Energy Adaptive Networks Corp. All rights reserved.
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v2 or Energy Adaptive Networks
 * Commercial License, which accompanies this distribution.
 * Any use, reproduction or distribution of the program constitutes the
 * recipient’s acceptance of this agreement.
 * http://www.eclipse.org/legal/epl-v20.html
 *
 * Contributors:
 *    Energy Adaptive Networks Corp. - dev API
 *    Lingwen Gan - initial flow implementation
 *    Peter Enescu - initial documentation
 *
 * Project OpenOPFV - Optimal Power Flow Visualizer – 7/20/2019
 * Module SquareMatrix.cpp
 *
 ***********************************************************************/

#include "SquareMatrix.h"

/******************************
 basic functions
 ******************************/

// default constructor
template <class T>
SquareMatrix<T>::SquareMatrix(const string &phase) {
    if (!validPhase(phase)) {
        std::cout << "Invalid phase!" << std::endl;
        return;
    }
    
    int n = int( phase.size() );
    vector<T> row(n, 0.0);
    _data = vector<vector<T>>(n, row);
}

template <class T>
SquareMatrix<T>::SquareMatrix(const int &size) {
    vector<T> row(size, 0.0);
    _data = vector<vector<T>>(size, row);
}

// copy constructor
template <class T>
SquareMatrix<T>::SquareMatrix(const SquareMatrix<T> &mat) : _data(mat._data) {
}

// deconstructor
template <class T>
SquareMatrix<T>::~SquareMatrix() {
}

// assignment
template <class T>
void SquareMatrix<T>::operator=(const SquareMatrix<T> &mat) {
    _data = mat._data;
}


/******************************
 numeric operations
 ******************************/

// hermitian transpose
template <class T>
SquareMatrix<T> SquareMatrix<T>::hermitian() const {
    int n = size();
    SquareMatrix<T> result(n);
    for (int row = 0; row < n; row ++) {
        for (int col = 0; col < n; col ++) {
            result[row][col] = std::conj( _data[col][row] );
        }
    }
    return result;
}

// add two matrices
template <class T>
SquareMatrix<T> SquareMatrix<T>::operator+(const SquareMatrix<T> &mat) const {
    SquareMatrix sum(mat);
    for (int row = 0; row < size(); row ++) {
        for (int col = 0; col < size(); col ++) {
            sum[row][col] += _data[row][col];
        }
    }
    return sum;
}

// add a matrix to itself
template <class T>
void SquareMatrix<T>::operator+=(const SquareMatrix<T> &mat) {
    for (int row = 0; row < size(); row ++) {
        for (int col = 0; col < size(); col ++)
            _data[row][col] += mat._data[row][col];
    }
}

// multiply a vector
template <class T>
ColumnVector<T> SquareMatrix<T>::operator*(const ColumnVector<T> &vec) {
    ColumnVector<T> result(vec);
    for (int row = 0; row < size(); row ++) {
        result[row] = 0.0;
        for (int col = 0; col < size(); col ++)
            result[row] += _data[row][col] * vec._data[col];
    }
    return result;
}

// multiply by a scalar
template <class T>
SquareMatrix<T> SquareMatrix<T>::operator*(const T &num) const {
    SquareMatrix<T> result(*this);
    for (int row = 0; row < size(); row ++) {
        for (int col = 0; col < size(); col ++)
            result[row][col] *= num;
    }
    return result;
}

// multiply a scalar to self
template <class T>
void SquareMatrix<T>::operator*=(const T &num) {
    for (int row = 0; row < size(); row ++) {
        for (int col = 0; col < size(); col ++)
            _data[row][col] *= num;
    }
}

// divide self by a scalar
template <class T>
void SquareMatrix<T>::operator/=(const T &num) {
    for (int row = 0; row < size(); row ++) {
        for (int col = 0; col < size(); col ++)
            _data[row][col] /= num;
    }
}


/******************************
 other operations
 ******************************/

// return matrix size
template <class T>
int SquareMatrix<T>::size() const {
    return int( _data.size() );
}

// return a row at index
template <class T>
vector<T> &SquareMatrix<T>::operator[](const int &index) {
    return _data[index];
}

// reset values to 0
template <class T>
void SquareMatrix<T>::reset() {
    int n = size();
    for (int i = 0; i < n; i ++)
        _data[i].assign(n, 0.0);
}

// add mat to self[indices]
template <class T>
void SquareMatrix<T>::addToIndices(const SquareMatrix<T> &mat,
                                   const vector<int> &indices) {
    int n = int( indices.size() );
    for (int row = 0; row < n; row ++) {
        for (int col = 0; col < n; col ++)
            _data[indices[row]][indices[col]] += mat._data[row][col];
    }
}

// add mat[indices] to self
template <class T>
void SquareMatrix<T>::addFromIndices(const SquareMatrix<T> &mat,
                                     const vector<int> &indices) {
    int n = int( indices.size() );
    for (int row = 0; row < n; row ++) {
        for (int col = 0; col < n; col ++)
            _data[row][col] += mat._data[indices[row]][indices[col]];
    }
}


/******************************
 helper operations
 ******************************/

// return true if phase is valid
template <class T>
bool SquareMatrix<T>::validPhase(const string &phase) const {
    if (phase.empty())  return false;
    if (phase[0] < 'a') return false;
    for (int i = 1; i < phase.size(); i ++) {
        if (phase[i] <= phase[i-1])
            return false;
    }
    return phase.back() <= 'c';
}
