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
 * Module ColumnVector.cpp
 *
 ***********************************************************************/

#include "ColumnVector.h"

/******************************
 basic functions
 ******************************/

// default constructor
template <class T>
ColumnVector<T>::ColumnVector(const string &phase) {
    if (!validPhase(phase)) {
        std::cout << "Invalid phase!" << std::endl;
        return;
    }
    
    int n = int( phase.size() );
    _data.assign(n, 0);
}

template <class T>
ColumnVector<T>::ColumnVector(const int &size) : _data(size, 0.0) {
}

// copy constructor
template <class T>
ColumnVector<T>::ColumnVector(const ColumnVector<T> &vec) : _data(vec._data) {
}

// deconstructor
template <class T>
ColumnVector<T>::~ColumnVector() {
}

// assignment
template <class T>
void ColumnVector<T>::operator=(const ColumnVector<T> &vec) {
    _data = vec._data;
}


/******************************
 numeric operations
 ******************************/

// negative sign
template <class T>
ColumnVector<T> ColumnVector<T>::operator-() const {
    ColumnVector<T> result(*this);
    for (int i = 0; i < size(); i ++)
        result[i] = -_data[i];
    return result;
}

// add two vectors
template <class T>
ColumnVector<T> ColumnVector<T>::operator+(const ColumnVector<T> &vec) const {
    ColumnVector<T> sum(vec);
    for (int i = 0; i < size(); i ++)
        sum[i] += _data[i];
    return sum;
}

// substract two vectors
template <class T>
ColumnVector<T> ColumnVector<T>::operator-(const ColumnVector<T> &vec) const {
    ColumnVector<T> diff(*this);
    for (int i = 0; i < size(); i ++)
        diff[i] -= vec._data[i];
    return diff;
}

// multiply by a scalar
template <class T>
ColumnVector<T> ColumnVector<T>::operator*(const T &num) const {
    ColumnVector<T> result(*this);
    for (int i = 0; i < size(); i ++)
        result[i] *= num;
    return result;
}

// multiply by a vector
template <class T>
ColumnVector<T> ColumnVector<T>::operator*(const ColumnVector<T> &vec) const {
    ColumnVector<T> result(*this);
    for (int i = 0; i < size(); i ++) {
        result[i] *= vec._data[i];
    }
    return result;
}

// divide by a scalar
template <class T>
ColumnVector<T> ColumnVector<T>::operator/(const T &num) const {
    ColumnVector<T> result(*this);
    for (int i = 0; i < size(); i ++) {
        result[i] /= num;
    }
    return result;
}

// divide by a vector
template <class T>
ColumnVector<T> ColumnVector<T>::operator/(const ColumnVector<T> &vec) const {
    ColumnVector result(*this);
    for (int i = 0; i < size(); i ++)
        result[i] /= vec._data[i];
    return result;
}

// multiply a scalar to self
template <class T>
void ColumnVector<T>::operator*=(const T &num) {
    for (int i = 0; i < size(); i ++) {
        _data[i] *= num;
    }
}

// divide self by a scalar
template <class T>
void ColumnVector<T>::operator/=(const T &num) {
    for (int i = 0; i < size(); i ++) {
        _data[i] /= num;
    }
}

// add a column vector
template <class T>
void ColumnVector<T>::operator+=(const ColumnVector<T> &vec) {
    for (int i = 0; i < size(); i ++)
        _data[i] += vec._data[i];
}


/******************************
 other operations
 ******************************/

// return vector size
template <class T>
int ColumnVector<T>::size() const {
    return int( _data.size() );
}

// return element with index
template <class T>
T &ColumnVector<T>::operator[](const int &index) {
    return _data[index];
}

// set all values to 0
template <class T>
void ColumnVector<T>::reset() {
    _data.assign(size(), 0.0);
}

// add vec to self[indices]
template <class T>
void ColumnVector<T>::addToIndices(const ColumnVector<T> &vec,
                                   const vector<int> &indices) {
    for (int i = 0; i < indices.size(); i ++)
        _data[indices[i]] += vec._data[i];
}

// add vec[indices] to self
template <class T>
void ColumnVector<T>::addFromIndices(const ColumnVector<T> &vec,
                                     const vector<int> &indices) {
    for (int i = 0; i < indices.size(); i ++)
        _data[i] += vec._data[indices[i]];
}


/******************************
 helper functions
 ******************************/

// return true if phase is valid
template <class T>
bool ColumnVector<T>::validPhase(const string &phase) const {
    if (phase.empty())  return false;
    if (phase[0] < 'a') return false;
    for (int i = 1; i < phase.size(); i ++) {
        if (phase[i] <= phase[i-1]) {
            return false;
        }
    }
    return phase.back() <= 'c';
}
