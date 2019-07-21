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
 * Module ElectricVehicle.cpp
 *
 ***********************************************************************/

#include "ElectricVehicle.h"

/******************************
 basic functions
 ******************************/

// default constructor
ElectricVehicle::ElectricVehicle(const string &name,
                                 const string &phase,
                                 const LoadType &type) : Load(name, phase, type) {
    _maxChargingRate = 0.0;
    _futureEnergyRequest = 0.0;
    _plugInTime = 0.0;
    _deadline = 0.0;
}

// copy constructor
ElectricVehicle::ElectricVehicle(const ElectricVehicle &ev) : Load(ev) {
    _maxChargingRate = ev._maxChargingRate;
    _futureEnergyRequest = ev._futureEnergyRequest;
    _plugInTime = ev._plugInTime;
    _deadline = ev._deadline;
}

// deconstructor
ElectricVehicle::~ElectricVehicle() {
}

// assignment
void ElectricVehicle::operator=(const ElectricVehicle &ev) {
    Load::operator=(ev);
    _maxChargingRate = ev._maxChargingRate;
    _futureEnergyRequest = ev._futureEnergyRequest;
    _plugInTime = ev._plugInTime;
    _deadline = ev._deadline;
}

// print
ostream &operator<<(ostream &cout, const ElectricVehicle *ev) {
    return cout;
}


/******************************
 other functions
 ******************************/

// fetch real data from future data when a new timestamp comes
void ElectricVehicle::fetchRealTimeData(const time_type &oldTimeInMinutes,
                                        const time_type &newTimeInMinutes,
                                        FutureData *futureData) {
    // update status variable --- _futureEnergyRequest
    double rate = _value._power[0].real();
    _futureEnergyRequest -= rate * (newTimeInMinutes - oldTimeInMinutes) / 60.0;
}
