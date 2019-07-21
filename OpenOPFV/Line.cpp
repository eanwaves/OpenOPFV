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
 * Module Line.cpp
 *
 ***********************************************************************/

#include "Line.h"
#include "Bus.h"

/******************************
 basic functions
 ******************************/

// default constructor
Line::Line(const string &name,
           const string &phase,
           const LineType &type) :
Element(name, phase),
_type(type),
_impedance(phase),
_current(phase),
_fromBus(NULL),
_toBus(NULL) {
}

// copy constructor
Line::Line(const Line &line) :
Element(line),
_type(line._type),
_impedance(line._impedance),
_current(line._current),
_fromBus(line._fromBus),
_toBus(line._toBus),
_phaseIndicesInFromBus(line._phaseIndicesInFromBus) {
}

// deconstructor
Line::~Line() {
}

// assignment
void Line::operator=(const Line &line) {
    Element::operator=(line);
    _type = line._type;
    _impedance = line._impedance;
    _current = line._current;
    _fromBus = line._fromBus;
    _toBus = line._toBus;
    _phaseIndicesInFromBus = line._phaseIndicesInFromBus;
}

// print
ostream &operator<<(ostream &cout, const Line *line) {
    cout << "line " << line->name() << '\n';
    cout << "\tphase " << line->phase() << '\n';
    cout << "\tfrom bus " << line->_fromBus->name() << '\n';
    cout << "\tto bus   " << line->_toBus->name() << '\n';
    return cout;
}


/******************************
 accessor functions
 ******************************/
LineType Line::type() const {
    return _type;
}

SquareMatrix<complex_type> Line::impedance() const {
    return _impedance;
}

ColumnVector<complex_type> Line::current() const {
    return _current;
}

Bus *Line::fromBus() const {
    return _fromBus;
}

Bus *Line::toBus() const {
    return _toBus;
}

vector<int> Line::phaseIndicesInFromBus() const {
    return _phaseIndicesInFromBus;
}

void Line::setPhase(const string &phase) {
    if (phase.compare(_phase) == 0 || !validPhase(phase))
        return;
    
    _phase = phase;
    _impedance = SquareMatrix<complex_type>(phase);
    _current = ColumnVector<complex_type>(phase);
}

void Line::setType(const LineType &type) {
    _type = type;
}

void Line::setImpedance(const SquareMatrix<complex_type> &impedance) {
    _impedance = impedance;
}

void Line::setCurrent(const ColumnVector<complex_type> &current) {
    _current = current;
}

void Line::setFromBus(Bus *fromBus) {
    _fromBus = fromBus;
}

void Line::setToBus(Bus *toBus) {
    _toBus = toBus;
}


/******************************
 initializers
 ******************************/

// initialize phase indices at from bus
void Line::initPhaseIndicesInFromBus() {
    // reset indices to empty
    _phaseIndicesInFromBus.clear();
    
    string fromBusPhase = _fromBus->phase();    // get from bus phase
    
    // for each phase of self, find corresponding index at from bus
    int phaseIdAtFrom = 0;
    for (int phaseIdAtSelf = 0; phaseIdAtSelf < _phase.size() ; phaseIdAtSelf ++) {
        
        // find the corresponding index at parent bus
        while (phaseIdAtFrom < fromBusPhase.size() &&
               fromBusPhase[phaseIdAtFrom] < _phase[phaseIdAtSelf]) {
            phaseIdAtFrom ++;
        }
        if (phaseIdAtFrom == fromBusPhase.size() ||
            fromBusPhase[phaseIdAtFrom] > _phase[phaseIdAtSelf]) {
            std::cout << "from bus must contain the phases of line!" << std::endl;
            std::cout << "From bus: " << _fromBus << std::endl;
            std::cout << "Line:     " << this << std::endl;
            exit(1);
        }
        
        _phaseIndicesInFromBus.push_back(phaseIdAtFrom);
        phaseIdAtFrom ++;
    }
}
