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
 * Module LoadValue.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__LoadValue__
#define __OptimalPowerFlowVisualization__LoadValue__

#include "BasicDataType.h"
#include "ColumnVector.h"
#include "SquareMatrix.h"

struct LoadValue {
    /******************************
     member variable
     ******************************/
    SquareMatrix<complex_type> _admittance;     // load (fixed admittance part)
    ColumnVector<complex_type> _power;          // load (fixed power part)
    
    
    /******************************
     basic functions
     ******************************/
    LoadValue(const string &phase = "a");                   // default constructor
    LoadValue(const LoadValue &loadValue);                  // copy constructor
    ~LoadValue();                                           // deconstructor
    void operator=(const LoadValue &loadValue);             // assignment
    friend ostream &operator<<(ostream &cout,
                               const LoadValue &loadValue);  // print
    
    
    /******************************
     numeric operation
     ******************************/
    LoadValue operator+(const LoadValue &loadValue) const;  // add two LoadValues
    void operator+=(const LoadValue &loadValue);            // add a loadValue to self
    LoadValue operator*(const double &num) const;           // multiply a scalar
    LoadValue operator/(const double &num) const;           // divide a scalar
    
    
    /******************************
     other operation
     ******************************/
    int size();                                         // return size
    void reset();                                       // reset values to 0
    void addToIndices(const LoadValue &loadValue,
                      const vector<int> &indices);      // add loadValue to self[indices]
    void addFromIndices(const LoadValue &loadValue,
                        const vector<int> &indices);    // add loadValue[indices] to self
    
    
    /******************************
     helper operation
     ******************************/
    bool validPhase(const string &phase) const;     // return true if phase is valid
};

#endif /* defined(__OptimalPowerFlowVisualization__LoadValue__) */
