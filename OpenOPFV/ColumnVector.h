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
 * Module ColumnVector.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__ColumnVector__
#define __OptimalPowerFlowVisualization__ColumnVector__

#include "BasicDataType.h"

template <class T>
struct ColumnVector {
    /******************************
     member variables
     ******************************/
    vector<T> _data;                // store entries of the vector
    
    
    /******************************
     basic functions
     ******************************/
    ColumnVector(const string &phase = "a");                // default constructor
    ColumnVector(const int &size);
    ColumnVector(const ColumnVector<T> &vec);               // copy constructor
    ~ColumnVector();                                        // deconstructor
    void operator=(const ColumnVector<T> &vec);             // assignment
    friend ostream &operator<<(ostream &cout,
                               const ColumnVector<T> &vec)  // print
    {
        for (int i = 0; i < vec.size(); i ++)
            cout << vec._data[i] << ' ';
        return cout;
    }
    
    
    /******************************
     numeric operations
     ******************************/
    
    // negative sign
    ColumnVector<T> operator-() const;
    
    // add two vectors
    ColumnVector<T> operator+(const ColumnVector<T> &vec) const;
    
    // substract two vectors
    ColumnVector<T> operator-(const ColumnVector<T> &vec) const;
    
    // multiply by a scalar
    ColumnVector<T> operator*(const T &num) const;
    
    // multiply by a vector
    ColumnVector<T> operator*(const ColumnVector<T> &vec) const;
    
    // divide by a scalar
    ColumnVector<T> operator/(const T &num) const;
    
    // divide by a vector
    ColumnVector<T> operator/(const ColumnVector<T> &vec) const;
    
    // multiply a scalar to self
    void operator*=(const T &num);
    
    // divide self by a scalar
    void operator/=(const T &num);
    
    // add a column vector
    void operator+=(const ColumnVector<T> &vec);
    
    // left multiply a complex number
    friend ColumnVector<T> operator*(const T &num,
                                     const ColumnVector<T> &vec)
    {return vec * num;}
    
    // compute norm
    friend double norm(const ColumnVector<T> &vec)
    {
        double result = 0.0;
        for (int i = 0; i < vec.size(); i ++)
            result += std::norm( vec._data[i] );
        return result;
    }
    
    
    /******************************
     other operations
     ******************************/
    int size() const;                                   // return vector size
    T &operator[](const int &index);                    // return element with index
    void reset();                                       // set all values to 0
    void addToIndices(const ColumnVector<T> &vec,
                      const vector<int> &indices);      // add vec to self[indices]
    void addFromIndices(const ColumnVector<T> &vec,
                        const vector<int> &indices);    // add vec[indices] to self
    
    
    /******************************
     helper functions
     ******************************/
    bool validPhase(const string &phase) const;         // return true if phase is valid
};

#endif /* defined(__OptimalPowerFlowVisualization__ColumnVector__) */
