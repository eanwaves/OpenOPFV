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
 * Module Load.cpp
 *
 ***********************************************************************/

#include "Load.h"
#include "Bus.h"
#include "FutureData.h"

/******************************
 basic functions
 ******************************/

// default constructor
Load::Load(const string &name,
           const string &phase,
           const LoadType &type) :
Element(name, phase),
_type(type),
_value(phase),
_locationBus(NULL) {
}

// copy constructor
Load::Load(const Load &load) :
Element(load),
_type(load._type),
_value(load._value),
_locationBus(load._locationBus),
_phaseIndicesInLocationBus(load._phaseIndicesInLocationBus) {
}

// deconstructor
Load::~Load() {
}

// assignment
void Load::operator=(const Load &load) {
    Element::operator=(load);
    _type = load._type;
    _value = load._value;
    _locationBus = load._locationBus;
    _phaseIndicesInLocationBus = load._phaseIndicesInLocationBus;
}

// print
ostream &operator<<(ostream &cout, const Load *load) {
    cout << (Element *)load;
    return cout;
}


/******************************
 accessor functions
 ******************************/

LoadType Load::type() const {
    return _type;
}

LoadValue Load::value() const {
    return _value;
}

Bus *Load::locationBus() const {
    return _locationBus;
}

vector<int> Load::phaseIndicesInLocationBus() const {
    return _phaseIndicesInLocationBus;
}

void Load::setPhase(const string &phase) {
    if (phase.compare(_phase) == 0 || !validPhase(phase))
        return;
    
    _phase = phase;
    _value = LoadValue(_phase);
}

void Load::setType(const LoadType &type) {
    _type = type;
}

void Load::setValue(const LoadValue &value) {
    _value = value;
}

void Load::setLocationBus(Bus *locationBus) {
    _locationBus = locationBus;
}


/******************************
 initializers
 ******************************/

// initialize phase indices at location bus
void Load::initPhaseIndicesInLocationBus() {
    // reset indices to empty
    _phaseIndicesInLocationBus.clear();
    
    string locationBusPhase = _locationBus->phase();    // get location bus phase
    
    // for each phase of self, find corresponding index at location bus
    int phaseIdAtLocation = 0;
    for (int phaseIdAtSelf = 0; phaseIdAtSelf < _phase.size() ; phaseIdAtSelf ++) {
        
        // find the corresponding index at parent bus
        while (phaseIdAtLocation < locationBusPhase.size() &&
               locationBusPhase[phaseIdAtLocation] < _phase[phaseIdAtSelf]) {
            phaseIdAtLocation ++;
        }
        if (phaseIdAtLocation == locationBusPhase.size() ||
            locationBusPhase[phaseIdAtLocation] > _phase[phaseIdAtSelf]) {
            std::cout << "location bus must contain the phases of load!" << std::endl;
            std::cout << "Location bus: " << _locationBus << std::endl;
            std::cout << "Load:         " << this << std::endl;
            exit(1);
        }
        
        _phaseIndicesInLocationBus.push_back(phaseIdAtLocation);
        phaseIdAtLocation ++;
    }
}


/******************************
 other functions
 ******************************/

// fetch real data from future data when a new timestamp comes
void Load::fetchRealTimeData(const time_type &oldTimeInMinutes,
                             const time_type &newTimeInMinutes,
                             FutureData *futureData) {
    BaseLoadData *data = (BaseLoadData *)(futureData->fetchFutureDataForLoad(_name, _type, newTimeInMinutes));
    futureData->releaseFutureDataForLoadTillTime(_name, newTimeInMinutes);
    _value = data->_loadValue;
    delete data;
}
