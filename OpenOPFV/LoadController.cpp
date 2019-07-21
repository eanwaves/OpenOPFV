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
 * Module LoadController.cpp
 *
 ***********************************************************************/

#include "LoadController.h"
#include "Load.h"
#include "BusController.h"

/******************************
 basic functions
 ******************************/

// default constructor
LoadController::LoadController(Load *load, int numberOfSlots) :
_load(load),
_valueArray(numberOfSlots, load->value()),
_locationBus(NULL),
_phaseIndicesInLocationBus(load->phaseIndicesInLocationBus()),
_oldValueArray(numberOfSlots, load->value()) {
}

// copy constructor
LoadController::LoadController(const LoadController &controller) :
_load(controller._load),
_valueArray(controller._valueArray),
_locationBus(controller._locationBus),
_phaseIndicesInLocationBus(controller._phaseIndicesInLocationBus),
_oldValueArray(controller._oldValueArray) {
}

// destructor
LoadController::~LoadController() {
    _load = NULL;
    _valueArray.clear();
    _locationBus = NULL;
    _phaseIndicesInLocationBus.clear();
    _oldValueArray.clear();
}

// assignment
void LoadController::operator=(const LoadController &controller) {
    _load = controller._load;
    _valueArray = controller._valueArray;
    _locationBus = controller._locationBus;
    _phaseIndicesInLocationBus = controller._phaseIndicesInLocationBus;
    _oldValueArray = controller._oldValueArray;
}

// print
ostream &operator<<(ostream &cout, const LoadController &controller) {
    cout << controller._load;
    return cout;
}


/******************************
 optimization functions
 ******************************/

// compute tentative power consumption, _oldValueArray[timeSlotId] unchanged
void LoadController::attemptPowerAtTime(const double &stepSize, const int &timeSlotId) {
}

void LoadController::attemptPowerOverHorizon(const double &stepSize) {
}

// set _oldValueArray[timeSlotId] to _valueArray[timeSlotId]
void LoadController::updatePowerAtTime(const int &timeSlotId) {
    _oldValueArray[timeSlotId] = _valueArray[timeSlotId];
}

void LoadController::updatePowerOverHorizon() {
    _oldValueArray = _valueArray;
}

// set _valueArray[timeSlotId] to _oldValueArray[timeSlotId]
void LoadController::resetPowerAtTime(const int &timeSlotId) {
    _valueArray[timeSlotId] = _oldValueArray[timeSlotId];
}

void LoadController::resetPowerOverHorizon() {
    _valueArray = _oldValueArray;
}

// compute objective value
double LoadController::objectiveValueAtTime(const int &timeSlotId) {
    return 0.0;
}

double LoadController::objectiveValueOverHorizon() {
    return 0.0;
}

// compute the expected change of the objective value
double LoadController::expectedObjectiveValueChangeAtTime(const int &timeSlotId) {
    double result = 0.0;
    ColumnVector<complex_type> gradient(int(_phaseIndicesInLocationBus.size()));
    gradient.reset();
    gradient.addFromIndices(_locationBus->_gradient[timeSlotId], _phaseIndicesInLocationBus);
    ColumnVector<complex_type> powerUpdate = _valueArray[timeSlotId]._power - _oldValueArray[timeSlotId]._power;
    for (int phaseId = 0; phaseId < powerUpdate.size(); phaseId ++) {
        result += gradient[phaseId].real() * powerUpdate[phaseId].real() + gradient[phaseId].imag() * powerUpdate[phaseId].imag();
     }
    return result;
}

double LoadController::expectedObjectiveValueChangeOverHorizon() {
    double result = 0.0;
    for (int timeSlotId = 0; timeSlotId < _valueArray.size(); timeSlotId ++)
        result += expectedObjectiveValueChangeAtTime(timeSlotId);
    return result;
}
