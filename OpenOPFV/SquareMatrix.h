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
 * Module SquareMatrix.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__SquareMatrix__
#define __OptimalPowerFlowVisualization__SquareMatrix__

#include "BasicDataType.h"
#include "ColumnVector.h"

template <class T>
struct SquareMatrix {
    /******************************
     member variables
     ******************************/
    vector<vector<T>> _data;        // store entries of the matrix
    
    
    /******************************
     basic functions
     ******************************/
    SquareMatrix(const string &phase = "a");                // default constructor
    SquareMatrix(const int &size);
    SquareMatrix(const SquareMatrix<T> &mat);               // copy constructor
    ~SquareMatrix();                                        // deconstructor
    void operator=(const SquareMatrix<T> &mat);             // assignment
    friend ostream &operator<<(ostream &cout,
                               const SquareMatrix<T> &mat)  // print
    {
        for (int row = 0; row < mat.size(); row ++) {
            for (int col = 0; col < mat.size(); col ++)
                cout << mat._data[row][col] << ' ';
            cout << '\t';
        }
        return cout;
    }
    
    
    /******************************
     numeric operations
     ******************************/
    
    // hermitain transpose
    SquareMatrix<T> hermitian() const;
    
    // add two matrices
    SquareMatrix<T> operator+(const SquareMatrix<T> &mat) const;
    
    // add a matrix to self
    void operator+=(const SquareMatrix<T> &mat);
    
    // multiply a vector
    ColumnVector<T> operator*(const ColumnVector<T> &vec);
    
    // multiply by a scalar
    SquareMatrix<T> operator*(const T &num) const;
    
    // multiply a scalar to self
    void operator*=(const T &num);
    
    // divide self by a scalar
    void operator/=(const T &num);
    
    // left multiply by a scalar
    friend SquareMatrix<T> operator*(const T &num,
                                     const SquareMatrix &mat)
    {return mat * num;}
    
    
    /******************************
     other operations
     ******************************/
    int size() const;                                   // return matrix size
    vector<T> &operator[](const int &index);            // return a row at index
    void reset();                                       // reset values to 0
    void addToIndices(const SquareMatrix<T> &mat,
                      const vector<int> &indices);      // add mat to self[indices]
    void addFromIndices(const SquareMatrix<T> &mat,
                        const vector<int> &indices);    // add mat[indices] to self
    
    
    /******************************
     helper functions
     ******************************/
    bool validPhase(const string &phase) const;         // return true if phase is valid
};

#endif /* defined(__OptimalPowerFlowVisualization__SquareMatrix__) */
