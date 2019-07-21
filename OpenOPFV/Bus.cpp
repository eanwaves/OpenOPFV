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
 * Module Bus.cpp
 *
 ***********************************************************************/

#include "Bus.h"
#include "Line.h"
#include "Load.h"

/******************************
 basic functions
 ******************************/

// default constructor
Bus::Bus(const string &name,
         const string &phase,
         const BusType &type) :
Element(name, phase),
_type(type),
_shunt(phase),
_aggregateLoad(phase),
_voltage(phase),
_fromLine(NULL) {
}

// copy constructor
Bus::Bus(const Bus &bus) :
Element(bus),
_type(bus._type),
_coordinate(bus._coordinate),
_shunt(bus._shunt),
_aggregateLoad(bus._aggregateLoad),
_voltage(bus._voltage),
_fromLine(bus._fromLine),
_toLineArray(bus._toLineArray),
_loadArray(bus._loadArray),
_phaseIndicesInParentBus(bus._phaseIndicesInParentBus) {
}

// release allocated spaces
void Bus::clear() {
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        delete _loadArray[loadId];
    }
}

// deconstructor
Bus::~Bus() {
    clear();
}

// assignment
void Bus::operator=(const Bus &bus) {
    Element::operator=(bus);
    _type = bus._type;
    _coordinate = bus._coordinate;
    _shunt = bus._shunt;
    _aggregateLoad = bus._aggregateLoad;
    _voltage = bus._voltage;
    _fromLine = bus._fromLine;
    _toLineArray = bus._toLineArray;
    _loadArray = bus._loadArray;
    _phaseIndicesInParentBus = bus._phaseIndicesInParentBus;
}

// print
ostream &operator<<(ostream &cout, const Bus *bus) {
    cout << "bus " << bus->name() << '\n';
    cout << "\tphase " << bus->phase() << '\n';
    if (bus->type() != SUBSTATION) {
        cout << "\tfrom line " << bus->_fromLine->name() << '\n';
    }
    else {
        cout << "\tis substation\n";
    }
    cout << "\tto lines\n";
    for (int i = 0; i < bus->_toLineArray.size(); i ++)
        cout << "\t\t" << bus->_toLineArray[i]->name() << '\n';
    cout << "\thave loads\n";
    for (int i = 0; i < bus->loadArray().size(); i ++)
        cout << "\t\t" << bus->_loadArray[i]->name() << '\n';
    return cout;
}


/******************************
 accessor functions
 ******************************/

// getter functions
BusType Bus::type() const {
    return _type;
}

Coordinate Bus::coordinate() const {
    return _coordinate;
}

SquareMatrix<complex_type> Bus::shunt() const {
    return _shunt;
}

LoadValue Bus::aggregateLoad() const {
    return _aggregateLoad;
}

ColumnVector<complex_type> Bus::voltage() const {
    return _voltage;
}

Line *Bus::fromLine() const {
    return _fromLine;
}

vector<Line *> Bus::toLineArray() const {
    return _toLineArray;
}

vector<Load *> Bus::loadArray() const {
    return _loadArray;
}

vector<int> Bus::phaseIndicesInParentBus() const {
    return _phaseIndicesInParentBus;
}

// setter functions
void Bus::setPhase(const string &phase) {
    if (_phase.compare(phase) == 0 || !validPhase(phase))
        return;
    
    _phase = phase;
    _aggregateLoad = LoadValue(phase);
    _voltage = ColumnVector<complex_type>(phase);
}

void Bus::setType(const BusType &type) {
    _type = type;
}

void Bus::setCoordinate(const Coordinate &coordinate) {
    _coordinate = coordinate;
}

void Bus::setShunt(const SquareMatrix<complex_type> &shunt) {
    _shunt = shunt;
}

void Bus::setAggregateLoad(const LoadValue &aggregateLoad) {
    _aggregateLoad = aggregateLoad;
}

void Bus::setVoltage(const ColumnVector<complex_type> &voltage) {
    _voltage = voltage;
}

void Bus::setFromLine(Line *fromLine) {
    _fromLine = fromLine;
}

void Bus::addAToLine(Line *toLine) {
    _toLineArray.push_back(toLine);
}

void Bus::addALoad(Load *load) {
    _loadArray.push_back(load);
}

void Bus::deleteALoad(Load *load) {
    vector<Load *>::iterator it = _loadArray.begin();
    while ((it != _loadArray.end()) &&
           (*it != load)) {
        it ++;
    }
    
    if (it != _loadArray.end()) {
        _loadArray.erase(it);
    } else {
        std::cout << "The load to be deleted cannot be found!" << std::endl;
    }
}


/******************************
 initializers
 ******************************/

// initialize phase indices in parent bus
void Bus::initPhaseIndicesInParentBus() {
    // reset indices to empty
    _phaseIndicesInParentBus.clear();
    
    // add component if exists a parent bus
    if (_fromLine) {
        Bus *parentBus = _fromLine->fromBus();          // get parent bus
        string parentBusPhase = parentBus->phase();     // get parent bus phase
        
        // for each phase of self, find corresponding index at parent bus
        int phaseIdAtParent = 0;
        for (int phaseIdAtSelf = 0; phaseIdAtSelf < _phase.size() ; phaseIdAtSelf ++) {
            
            // find the corresponding index at parent bus
            while (phaseIdAtParent < parentBusPhase.size() &&
                   parentBusPhase[phaseIdAtParent] < _phase[phaseIdAtSelf]) {
                phaseIdAtParent ++;
            }
            if (phaseIdAtParent == parentBusPhase.size() ||
                parentBusPhase[phaseIdAtParent] > _phase[phaseIdAtSelf]) {
                std::cout << "parent bus must contain the phases of children bus!" << std::endl;
                std::cout << "Parent bus: " << parentBus << std::endl;
                std::cout << "Child bus: " << this << std::endl;
                exit(1);
            }
            
            _phaseIndicesInParentBus.push_back(phaseIdAtParent);
            phaseIdAtParent ++;
        }
    }
}


/******************************
 power flow computations
 ******************************/

// initialize voltage to parent bus voltage
void Bus::initVoltage() {
    _voltage.reset();
    ColumnVector<complex_type> parentVoltage = _fromLine->fromBus()->voltage();
    _voltage.addFromIndices(parentVoltage, _phaseIndicesInParentBus);
}

// compute total load on the bus
void Bus::computeAggregateLoadOnSelf() {
    _aggregateLoad._admittance = _shunt;
    _aggregateLoad._power.reset();
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        Load *load = _loadArray[loadId];
        _aggregateLoad.addToIndices(load->value(), load->phaseIndicesInLocationBus());
    }
}

// compute current by backward sweep
double Bus::computeCurrentOnFromLine() {
    ColumnVector<complex_type> current( _phase );
    current.reset();
    
    // contributions from downstream lines
    for (int lineId = 0; lineId < _toLineArray.size(); lineId ++) {
        Line *line = _toLineArray[lineId];
        current.addToIndices(line->current(), line->phaseIndicesInFromBus());
    }
    
    // contributions from load on this bus
    current += _aggregateLoad._admittance * _voltage;
    for (int phaseId = 0; phaseId < current.size(); phaseId ++) {
        current[phaseId] += std::conj(_aggregateLoad._power[phaseId] / _voltage[phaseId]);
    }
    
    // update current on parent line and return update size
    ColumnVector<complex_type> diff = current - _fromLine->current();
    _fromLine->setCurrent(current);
    return norm(diff);
}

// compute voltage by forward sweep
double Bus::computeVoltageOnSelf() {
    // compute voltage according to Kirchoff's law
    ColumnVector<complex_type> voltage = - (_fromLine->impedance() * _fromLine->current());
    Bus *fromBus = _fromLine->fromBus();
    voltage.addFromIndices(fromBus->voltage(), _phaseIndicesInParentBus);
    
    // update voltage and return update size
    ColumnVector<complex_type> diff = voltage - _voltage;
    _voltage = voltage;
    return norm(diff);
}


/******************************
 other functions
 ******************************/

// fetch real data from future data when a new timestamp comes
void Bus::fetchRealTimeData(const time_type &oldTimeInMinutes,
                            const time_type &newTimeInMinutes,
                            FutureData *futureData) {
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        _loadArray[loadId]->fetchRealTimeData(oldTimeInMinutes,newTimeInMinutes, futureData);
    }
}
