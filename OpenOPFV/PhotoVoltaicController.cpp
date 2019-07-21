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
 * Module PhotoVoltaicController.cpp
 *
 ***********************************************************************/

#include "PhotoVoltaicController.h"
#include "PhotoVoltaic.h"
#include "BusController.h"

/******************************
 basic functions
 ******************************/

// default constructor
PhotoVoltaicController::PhotoVoltaicController(PhotoVoltaic *pv, int numberOfSlots) :
LoadController(pv, numberOfSlots),
_nameplate(pv->nameplate()) {
}

// copy constructor
PhotoVoltaicController::PhotoVoltaicController(const PhotoVoltaicController &controller) :
LoadController(controller),
_nameplate(controller._nameplate) {
}

// deconstructor
PhotoVoltaicController::~PhotoVoltaicController() {
}

// assignment
void PhotoVoltaicController::operator=(const PhotoVoltaicController &controller) {
    LoadController::operator=(controller);
    _nameplate = controller._nameplate;
}

// print
ostream &operator<<(ostream &cout, const PhotoVoltaicController *controller) {
    cout << (LoadController *)(controller);
    return cout;
}


/******************************
 optimization functions
 ******************************/

// compute tentative power consumption, _oldValueArray[timeSlotId] unchanged
void PhotoVoltaicController::attemptPowerAtTime(const double &stepSize, const int &timeSlotId) {
    int numberOfPhases = int( _phaseIndicesInLocationBus.size() );
    
    // get gradient
    ColumnVector<complex_type> gradient( numberOfPhases );
    gradient.reset();
    gradient.addFromIndices(_locationBus->_gradient[timeSlotId], _phaseIndicesInLocationBus);
    
    // do movement and projection
    for (int phaseId = 0; phaseId < numberOfPhases; phaseId ++) {
        // movement
        double reactivePower = _oldValueArray[timeSlotId]._power[phaseId].imag();
        reactivePower -= stepSize * gradient[phaseId].imag();
        
        // projection
        double realPower = _oldValueArray[timeSlotId]._power[phaseId].real();
        double maxReactivePowerMagnitude = sqrt( _nameplate * _nameplate - realPower * realPower );
        if (reactivePower > maxReactivePowerMagnitude)
            reactivePower = maxReactivePowerMagnitude;
        else if (reactivePower < -maxReactivePowerMagnitude)
            reactivePower = -maxReactivePowerMagnitude;
        
        _valueArray[timeSlotId]._power[phaseId].imag(reactivePower);
    }
}

void PhotoVoltaicController::attemptPowerOverHorizon(const double &stepSize) {
    for (int timeSlotId = 0; timeSlotId < _valueArray.size(); timeSlotId ++) {
        attemptPowerAtTime(stepSize, timeSlotId);
    }
}
