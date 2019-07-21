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
 *    Peter Enescu - initial implementation, test, docs
 *
 * Project OpenOPFV - Optimal Power Flow Visualizer – 7/20/2019
 * Module LineController.cpp
 *
 ***********************************************************************/

#include "LineController.h"
#include "Line.h"
#include "BusController.h"

/******************************
 basic functions
 ******************************/

// default constructor
LineController::LineController(Line *line, int numberOfSlots) :
_line(line),
_impedance(line->impedance()),
_currentArray(numberOfSlots, line->current()),
_phaseIndicesInFromBus(line->phaseIndicesInFromBus()) {
}

// copy constructor
LineController::LineController(const LineController &controller) :
_line(controller._line),
_impedance(controller._impedance),
_currentArray(controller._currentArray),
_fromBus(controller._fromBus),
_toBus(controller._toBus),
_phaseIndicesInFromBus(controller._phaseIndicesInFromBus) {
}

// destructor
LineController::~LineController() {
    _line = NULL;
    _currentArray.clear();
    _fromBus = NULL;
    _toBus = NULL;
    _phaseIndicesInFromBus.clear();
}

// assignment
void LineController::operator=(const LineController &controller) {
    _line = controller._line;
    _impedance = controller._impedance;
    _currentArray = controller._currentArray;
    _fromBus = controller._fromBus;
    _toBus = controller._toBus;
    _phaseIndicesInFromBus = controller._phaseIndicesInFromBus;
}

// print
ostream &operator<<(ostream &cout, const LineController *controller) {
    cout << controller->_line;
    return cout;
}
