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
 *    Caron Zhang - initial API implementation
 *    Peter Enescu - initial implementation, test, docs
 *
 * Project OpenOPFV - Optimal Power Flow Visualizer – 7/20/2019
 * Module ElectricVehicleController.cpp
 *
 ***********************************************************************/

#include "ElectricVehicleController.h"
#include "BusController.h"

/******************************
 basic functions
 ******************************/

// default constructor
ElectricVehicleController::ElectricVehicleController(ElectricVehicle *ev,
                                                     int numberOfSlots,
                                                     time_type timeSlotLengthInMinutes) :
LoadController(ev, numberOfSlots) {
    _slotLengthInMinutes = timeSlotLengthInMinutes;
}

// copy constructor
ElectricVehicleController::ElectricVehicleController(const ElectricVehicleController &ev) : LoadController(ev) {
    _plugInTimeSlotId = ev._plugInTimeSlotId;
    _deadlineTimeSlotId = ev._deadlineTimeSlotId;
    _slotLengthInMinutes = ev._slotLengthInMinutes;
}

// destructor
ElectricVehicleController::~ElectricVehicleController() {
}

// assignment
void ElectricVehicleController::operator=(const ElectricVehicleController &ev) {
    LoadController::operator=(ev);
    _plugInTimeSlotId = ev._plugInTimeSlotId;
    _deadlineTimeSlotId = ev._deadlineTimeSlotId;
    _slotLengthInMinutes = ev._slotLengthInMinutes;
}

// print
ostream &operator<<(ostream &cout, const ElectricVehicleController *ev) {
    return cout;
}


/******************************
 optimization functions
 ******************************/

// compute tentative power consumption, _oldValueArray[timeSlotId] unchanged
void ElectricVehicleController::attemptPowerOverHorizon(const double &stepSize) {
    // must satisfy _deadlineTimeSlotId > _plugInTimeSlotId >= 0
    if (_deadlineTimeSlotId <= _plugInTimeSlotId ||
        _plugInTimeSlotId < 0 ||
        _deadlineTimeSlotId > _valueArray.size()) {
        std::cout << "Unexpected time slot information to optimize over!" << std::endl;
        return;
    }
    
    // get gradient from location bus
    vector<double> grad(_valueArray.size(), 0.0);
    for (int timeSlotId = 0; timeSlotId < grad.size(); timeSlotId ++)
        grad[timeSlotId] = _locationBus->_gradient[timeSlotId][_phaseIndicesInLocationBus[0]].real();
    
    // move along the negative gradient direction by step size
    vector<double> tentativeProfileInChargingSlots(_deadlineTimeSlotId - _plugInTimeSlotId, 0.0);
    for (int timeSlotId = _plugInTimeSlotId; timeSlotId < _deadlineTimeSlotId; timeSlotId ++) {
        tentativeProfileInChargingSlots[timeSlotId - _plugInTimeSlotId] = _oldValueArray[timeSlotId]._power[0].real() - stepSize * grad[timeSlotId];
    }
    
    // projection back to the feasible set
    // get the range of moving
    double minRate = tentativeProfileInChargingSlots[0];
    double maxRate = tentativeProfileInChargingSlots[0];
    for (int timeSlotId = 1; timeSlotId < tentativeProfileInChargingSlots.size(); timeSlotId ++) {
        double rate = tentativeProfileInChargingSlots[timeSlotId];
        if (rate < minRate)
            minRate = rate;
        if (rate > maxRate)
            maxRate = rate;
    }
    double maxChargingRate = ((ElectricVehicle *)_load)->_maxChargingRate;
    double minChargingRate = 0.0;
    double energyRequest = ((ElectricVehicle *)_load)->_futureEnergyRequest * 60 / _slotLengthInMinutes;
    double maxMove = maxChargingRate - minRate;
    double minMove = minChargingRate - maxRate;
    
    // modified bijection method to speed up projection
    vector<double> tentativeProfileAfterProjection(tentativeProfileInChargingSlots);
    while (maxMove > minMove + 1e-4) {
        double move = (maxMove + minMove) / 2.0;
        double sumRate = 0.0;
        int activeLowerBound = 0.0;
        int activeUpperBound = 0.0;
        
        // get the profile after moving with step size move
        for (int timeSlotId = 0; timeSlotId < tentativeProfileInChargingSlots.size(); timeSlotId ++) {
            double rate = move + tentativeProfileInChargingSlots[timeSlotId];
            if (rate < minChargingRate) {
                rate = minChargingRate;
                activeLowerBound ++;
            }
            else if (rate > maxChargingRate) {
                rate = maxChargingRate;
                activeUpperBound ++;
            }
            tentativeProfileAfterProjection[timeSlotId] = rate;
            sumRate += rate;
        }
        
        // find how to upate maxMove and minMove
        if (sumRate == energyRequest) {
            maxMove = move;
            minMove = move;
        }
        else if (sumRate < energyRequest)
            minMove = move + (energyRequest - sumRate) / (tentativeProfileInChargingSlots.size() - activeUpperBound);
        else
            maxMove = move - (sumRate - energyRequest) / (tentativeProfileInChargingSlots.size() - activeLowerBound);
    }
    
    // set tentativeProfileInChargingSlots
    double move = (maxMove + minMove) / 2.0;
    for (int timeSlotId = 0; timeSlotId < tentativeProfileInChargingSlots.size(); timeSlotId ++) {
        double rate = move + tentativeProfileInChargingSlots[timeSlotId];
        if (rate < minChargingRate)
            rate = minChargingRate;
        if (rate > maxChargingRate)
            rate = maxChargingRate;
        tentativeProfileInChargingSlots[timeSlotId] = rate;
    }
    
    // write back to _valueArray
    for (int timeSlotId = _plugInTimeSlotId; timeSlotId < _deadlineTimeSlotId; timeSlotId ++) {
        _valueArray[timeSlotId]._power[0] = tentativeProfileInChargingSlots[timeSlotId - _plugInTimeSlotId];
    }
}
