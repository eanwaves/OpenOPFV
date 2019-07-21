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
 * Module LoadValue.cpp
 *
 ***********************************************************************/

#include "LoadValue.h"

/******************************
 basic functions
 ******************************/

// default constructor
LoadValue::LoadValue(const string &phase) : _admittance(phase), _power(phase) {
}

// copy constructor
LoadValue::LoadValue(const LoadValue &loadValue) : _admittance(loadValue._admittance), _power(loadValue._power) {
}

// deconstructor
LoadValue::~LoadValue() {
}

// assignment
void LoadValue::operator=(const LoadValue &loadValue) {
    _admittance = loadValue._admittance;
    _power = loadValue._power;
}

// print
ostream &operator<<(ostream &cout, const LoadValue &loadValue) {
    cout << loadValue._admittance << loadValue._power;
    return cout;
}


/******************************
 numeric operation
 ******************************/

// add two loadValues
LoadValue LoadValue::operator+(const LoadValue &loadValue) const {
    LoadValue sum(loadValue);
    sum._admittance += _admittance;
    sum._power += _power;
    return sum;
}

// add a loadValue to self
void LoadValue::operator+=(const LoadValue &loadValue) {
    _admittance += loadValue._admittance;
    _power += loadValue._power;
}

// multiply a scalar
LoadValue LoadValue::operator*(const double &num) const {
    LoadValue result(*this);
    result._admittance *= num;
    result._power *= num;
    return result;
}

// divide a scalar
LoadValue LoadValue::operator/(const double &num) const {
    LoadValue result(*this);
    result._admittance /= num;
    result._power /= num;
    return result;
}


/******************************
 other operation
 ******************************/

// return size
int LoadValue::size() {
    return int( _power.size() );
}

// reset values to 0
void LoadValue::reset() {
    _admittance.reset();
    _power.reset();
}

// add loadValue to self[indices]
void LoadValue::addToIndices(const LoadValue &loadValue,
                             const vector<int> &indices) {
    _admittance.addToIndices(loadValue._admittance, indices);
    _power.addToIndices(loadValue._power, indices);
}

// add loadValue[indices] to self
void LoadValue::addFromIndices(const LoadValue &loadValue,
                               const vector<int> &indices) {
    _admittance.addFromIndices(loadValue._admittance, indices);
    _power.addFromIndices(loadValue._power, indices);
}


/******************************
 helper operation
 ******************************/

// return true if phase is valid
bool LoadValue::validPhase(const string &phase) const {
    return _admittance.validPhase(phase);
}
