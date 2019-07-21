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
 * Module BusController.cpp
 *
 ***********************************************************************/

#include "BusController.h"
#include "bus.h"
#include "LineController.h"
#include "LoadController.h"
#include "Load.h"

/******************************
 basic functions
 ******************************/

// default constructor
BusController::BusController(Bus *bus, int numberOfSlots) :
_bus(bus),
_shunt(bus->shunt()),
_aggregateLoads(numberOfSlots, bus->aggregateLoad()),
_voltages(numberOfSlots, bus->voltage()),
_fromLine(NULL),
_phaseIndicesInParentBus(bus->phaseIndicesInParentBus()),
_oldAggregateLoads(numberOfSlots, bus->aggregateLoad()) {
    // decide on voltage constraints
    if (_bus->type() == HOUSE) {
        _hasVoltageConstraint = true;
        _voltageMin = 0.95;
        _voltageMax = 1.05;
    }
    else {
        _hasVoltageConstraint = false;
    }
    
    // decide on algorighm variables
    string phase = bus->phase();
    _beta = ColumnVector<complex_type>(phase);
    for (int phaseId = 0; phaseId < phase.size(); phaseId ++) {
        double angle = - M_PI * 2 / 3 * (phase[phaseId] - 'a');
        _beta[phaseId] = complex_type(cos(angle), sin(angle));
    }
    _sumDown.assign(numberOfSlots, ColumnVector<double>(phase));
    _sumUp.assign(numberOfSlots, ColumnVector<complex_type>(phase));
    _gradient.assign(numberOfSlots, ColumnVector<complex_type>(phase));
}

// copy constructor
BusController::BusController(const BusController &controller) :
_bus(controller._bus),
_shunt(controller._shunt),
_aggregateLoads(controller._aggregateLoads),
_voltages(controller._voltages),
_fromLine(controller._fromLine),
_toLineArray(controller._toLineArray),
_loadArray(controller._loadArray),
_phaseIndicesInParentBus(controller._phaseIndicesInParentBus),
_hasVoltageConstraint(controller._hasVoltageConstraint),
_voltageMin(controller._voltageMin),
_voltageMax(controller._voltageMax),
_beta(controller._beta),
_sumDown(controller._sumDown),
_sumUp(controller._sumUp),
_gradient(controller._gradient),
_oldAggregateLoads(controller._oldAggregateLoads) {
}

// release allocated spaces
void BusController::clear() {
    _aggregateLoads.clear();
    _voltages.clear();
    _toLineArray.clear();
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        delete _loadArray[loadId];
    }
    _loadArray.clear();
    _phaseIndicesInParentBus.clear();
    _sumDown.clear();
    _sumUp.clear();
    _gradient.clear();
    _oldAggregateLoads.clear();
}


// deconstructor
BusController::~BusController() {
    clear();
}

// assignment
void BusController::operator=(const BusController &controller) {
    _bus = controller._bus;
    _shunt = controller._shunt;
    _aggregateLoads = controller._aggregateLoads;
    _voltages = controller._voltages;
    
    _fromLine = controller._fromLine;
    _toLineArray = controller._toLineArray;
    _loadArray = controller._loadArray;
    _phaseIndicesInParentBus = controller._phaseIndicesInParentBus;
    
    _hasVoltageConstraint = controller._hasVoltageConstraint;
    _voltageMin = controller._voltageMin;
    _voltageMax = controller._voltageMax;
    
    _beta = controller._beta;
    _sumDown = controller._sumDown;
    _sumUp = controller._sumUp;
    _gradient = controller._gradient;
    _oldAggregateLoads = controller._oldAggregateLoads;
}

// print
ostream &operator<<(ostream &cout, const BusController *controller) {
    cout << controller->_bus;
    return cout;
}


/******************************
 accessor functions
 ******************************/

// add a load
void BusController::addALoad(LoadController *load) {
    _loadArray.push_back(load);
}

// delete a load
void BusController::deleteALoad(LoadController *load) {
    vector<LoadController *>::iterator it = _loadArray.begin();
    while (it != _loadArray.end() &&
           (*it) != load) {
        it ++;
    }
    if (it == _loadArray.end()) {
        std::cout << "cannot delete load!" << std::endl;
    }
    else {
        _loadArray.erase(it);
    }
}

void BusController::deleteALoad(Load *load) {
    vector<LoadController *>::iterator it = _loadArray.begin();
    while (it != _loadArray.end() &&
           (*it)->_load != load) {
        it ++;
    }
    if (it == _loadArray.end()) {
        std::cout << "cannot delete load!" << std::endl;
    }
    else {
        _loadArray.erase(it);
    }
}


/******************************
 power flow computations
 ******************************/

// initialize voltage to parent bus voltage
void BusController::initVoltageAtTime(int timeSlotId) {
    _voltages[timeSlotId].reset();
    ColumnVector<complex_type> parentVoltage = _fromLine->_fromBus->_voltages[timeSlotId];
    _voltages[timeSlotId].addFromIndices(parentVoltage, _phaseIndicesInParentBus);
}

void BusController::initVoltageOverHorizon() {
    for (int timeId = 0; timeId < _voltages.size(); timeId ++) {
        initVoltageAtTime(timeId);
    }
}

// compute aggregate load on the bus
void BusController::computeAggregateLoadOnSelfAtTime(int timeSlotId) {
    _aggregateLoads[timeSlotId]._admittance = _shunt;
    _aggregateLoads[timeSlotId]._power.reset();
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        LoadController *load = _loadArray[loadId];
        _aggregateLoads[timeSlotId].addToIndices(load->_valueArray[timeSlotId], load->_phaseIndicesInLocationBus);
    }
}

void BusController::computeAggregateLoadOnSelfOverHorizon() {
    for (int timeSlotId = 0; timeSlotId < _aggregateLoads.size(); timeSlotId ++) {
        computeAggregateLoadOnSelfAtTime(timeSlotId);
    }
}

// compute current by backward sweep
double BusController::computeCurrentOnFromLineAtTime(int timeSlotId) {
    ColumnVector<complex_type> current( _bus->phase() );
    current.reset();
    
    // contributions from downstream lines
    for (int lineId = 0; lineId < _toLineArray.size(); lineId ++) {
        LineController *line = _toLineArray[lineId];
        current.addToIndices(line->_currentArray[timeSlotId], line->_phaseIndicesInFromBus);
    }
    
    // contributions from load on this bus
    current += _aggregateLoads[timeSlotId]._admittance * _voltages[timeSlotId];
    for (int phaseId = 0; phaseId < current.size(); phaseId ++) {
        current[phaseId] += std::conj(_aggregateLoads[timeSlotId]._power[phaseId] / _voltages[timeSlotId][phaseId]);
    }
    
    // update current on parent line and return update size
    ColumnVector<complex_type> diff = current - _fromLine->_currentArray[timeSlotId];
    _fromLine->_currentArray[timeSlotId] = current;
    return norm(diff);
}

double BusController::computeCurrentOnFromLineOverHorizon() {
    double updateSize = 0.0;
    for (int timeSlotId = 0; timeSlotId < _aggregateLoads.size(); timeSlotId ++) {
        double thisUpdateSize = computeCurrentOnFromLineAtTime(timeSlotId);
        if (updateSize < thisUpdateSize)
            updateSize = thisUpdateSize;
    }
    return updateSize;
}

// compute voltage by forward sweep
double BusController::computeVoltageOnSelfAtTime(int timeSlotId) {
    // compute voltage according to Kirchoff's law
    ColumnVector<complex_type> voltage = - (_fromLine->_impedance * _fromLine->_currentArray[timeSlotId]);
    BusController *fromBus = _fromLine->_fromBus;
    voltage.addFromIndices(fromBus->_voltages[timeSlotId], _phaseIndicesInParentBus);
    
    // update voltage and return update size
    ColumnVector<complex_type> diff = voltage - _voltages[timeSlotId];
    _voltages[timeSlotId] = voltage;
    return norm(diff);
}

double BusController::computeVoltageOnSelfOverHorizon() {
    double updateSize = 0.0;
    for (int timeSlotId = 0; timeSlotId < _aggregateLoads.size(); timeSlotId ++) {
        double thisUpdateSize = computeVoltageOnSelfAtTime(timeSlotId);
        if (updateSize < thisUpdateSize)
            updateSize = thisUpdateSize;
    }
    return updateSize;
}


/******************************
 compute gradient
 ******************************/

// compute sumDown
void BusController::computeSumDownAtTime(double muLower, double muUpper, const vector<double> &price, const int &timeSlotId) {
    // compute local contribution
    ColumnVector<complex_type> result = _aggregateLoads[timeSlotId]._admittance * _beta;
    for (int i = 0; i < result.size(); i ++) {
        result[i] *= std::conj(_beta[i]);
        _sumDown[timeSlotId][i] = result[i].real() * price[i];
    }
    
    if (_hasVoltageConstraint) {
        for (int i = 0; i < result.size(); i ++) {
            _sumDown[timeSlotId][i] -= muLower / ( std::norm(_voltages[timeSlotId][i]) - _voltageMin * _voltageMin );
            _sumDown[timeSlotId][i] += muUpper / (_voltageMax * _voltageMax - std::norm(_voltages[timeSlotId][i]));
        }
    }
    
    // add contributions from downstream buses
    for (int lineId = 0; lineId < _toLineArray.size(); lineId ++) {
        BusController *toBus = _toLineArray[lineId]->_toBus;
        _sumDown[timeSlotId].addToIndices(toBus->_sumDown[timeSlotId], toBus->_phaseIndicesInParentBus);
    }
}

void BusController::computeSumDownOverHorizon(double muLower, double muUpper, const vector<double> &price) {
    for (int timeSlotId = 0; timeSlotId < _sumDown.size(); timeSlotId ++) {
        computeSumDownAtTime(muLower, muUpper, price, timeSlotId);
    }
}

// compute sumUp
void BusController::computeSumUpAtTime(const int &timeSlotId) {
    // compute local contribution
    int numberOfPhases = int( _phaseIndicesInParentBus.size() );
    ColumnVector<complex_type> result(numberOfPhases);
    for (int phaseId = 0; phaseId < numberOfPhases; phaseId ++) {
        result[phaseId] = _beta[phaseId] * _sumDown[timeSlotId][phaseId];
    }
    result = _fromLine->_impedance.hermitian() * result;
    for (int phaseId = 0; phaseId < numberOfPhases; phaseId ++) {
        result[phaseId] *= std::conj(_beta[phaseId]);
    }
    _sumUp[timeSlotId] = result;
    
    // add contribution from parent bus
    _sumUp[timeSlotId].addFromIndices(_fromLine->_fromBus->_sumUp[timeSlotId], _phaseIndicesInParentBus);
}

void BusController::computeSumUpOverHorizon() {
    for (int timeSlotId = 0; timeSlotId < _sumUp.size(); timeSlotId ++) {
        computeSumUpAtTime(timeSlotId);
    }
}

// compute gradient
void BusController::computeGradientAtTime(const vector<double> &price, const int &timeSlotId) {
    int numberOfPhases = int( _phaseIndicesInParentBus.size() );
    for (int phaseId = 0; phaseId < numberOfPhases; phaseId ++) {
        _gradient[timeSlotId][phaseId] = price[phaseId] - std::conj(_sumUp[timeSlotId][phaseId]) * 2.0;
    }
}

void BusController::computeGradientOverHorizon(const vector<double> &price) {
    for (int timeSlotId = 0; timeSlotId < _gradient.size(); timeSlotId ++) {
        computeGradientAtTime(price, timeSlotId);
    }
}


/******************************
 update/reset/attempt power consumption
 ******************************/

// compute tentative power consumption, _oldValueArray[timeSlotId] unchanged
void BusController::attemptPowerAtTime(const double &stepSize, const int &timeSlotId, unordered_set<LoadType> &enabledInControl) {
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        LoadController *load = _loadArray[loadId];
        if ( enabledInControl.find(load->_load->type()) != enabledInControl.end() )
            load->attemptPowerAtTime(stepSize, timeSlotId);
    }
    computeAggregateLoadOnSelfAtTime(timeSlotId);
}

void BusController::attemptPowerOverHorizon(const double &stepSize, unordered_set<LoadType> &enabledInControl) {
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        LoadController *load = _loadArray[loadId];
        if (enabledInControl.find(load->_load->type()) != enabledInControl.end())
            load->attemptPowerOverHorizon(stepSize);
    }
    computeAggregateLoadOnSelfOverHorizon();
}

// set _oldValueArray[timeSlotId] to _valueArray[timeSlotId]
// set _oldAggregateLoads[timeSlotId] to _aggregateLoads[timeSlotId]
void BusController::updatePowerAtTime(const int &timeSlotId, unordered_set<LoadType> &enabledInControl) {
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        LoadController *load = _loadArray[loadId];
        if (enabledInControl.find(load->_load->type()) != enabledInControl.end())
            load->updatePowerAtTime(timeSlotId);
    }
    computeAggregateLoadOnSelfAtTime(timeSlotId);
    _oldAggregateLoads[timeSlotId] = _aggregateLoads[timeSlotId];
}

void BusController::updatePowerOverHorizon(unordered_set<LoadType> &enabledInControl) {
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        LoadController *load = _loadArray[loadId];
        if (enabledInControl.find(load->_load->type()) != enabledInControl.end())
            load->updatePowerOverHorizon();
    }
    computeAggregateLoadOnSelfOverHorizon();
    _oldAggregateLoads = _aggregateLoads;
}

// set _valueArray[timeSlotId] to _oldValueArray[timeSlotId]
void BusController::resetPowerAtTime(const int &timeSlotId, unordered_set<LoadType> &enabledInControl) {
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        LoadController *load = _loadArray[loadId];
        if (enabledInControl.find(load->_load->type()) != enabledInControl.end())
            load->resetPowerAtTime(timeSlotId);
    }
    computeAggregateLoadOnSelfAtTime(timeSlotId);
    // _oldAggregateLoads[timeSlotId] = _aggregateLoads[timeSlotId];
}

void BusController::resetPowerOverHorizon(unordered_set<LoadType> &enabledInControl) {
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        LoadController *load = _loadArray[loadId];
        if (enabledInControl.find(load->_load->type()) != enabledInControl.end())
            load->resetPowerOverHorizon();
    }
    computeAggregateLoadOnSelfOverHorizon();
    // _oldAggregateLoads = _aggregateLoads;
}

// check if there is voltage violation downstream
bool BusController::downstreamVoltageViolationAtTime(const int &timeSlotId) const {
    // check local bus
    if (_hasVoltageConstraint) {
        for (int phaseId = 0; phaseId < _phaseIndicesInParentBus.size(); phaseId ++) {
            double voltageMagnitude = std::sqrt( std::norm( _voltages[timeSlotId]._data[phaseId] ) );
            if (voltageMagnitude < _voltageMin ||
                voltageMagnitude > _voltageMax) {
                std::cout << "bus " << _bus->name() << " (" << _bus->phase()[phaseId] << ")\n";
                std::cout << "\tvoltage = " << voltageMagnitude << " at time slot with index " << timeSlotId << '\n';
                return true;
            }
        }
    }
    
    // check downstream buses
    for (int lineId = 0; lineId < _toLineArray.size(); lineId ++) {
        if (_toLineArray[lineId]->_toBus->downstreamVoltageViolationAtTime(timeSlotId))
            return true;
    }
    
    return false;
}

bool BusController::downstreamVoltageViolationOverHorizon() const {
    for (int timeSlotId = 0; timeSlotId < _aggregateLoads.size(); timeSlotId ++) {
        if (downstreamVoltageViolationAtTime(timeSlotId) == true)
            return true;
    }
    return false;
}

// compute aggregated downstream objective value
// store result in _objectiveValueDownstream
double BusController::downstreamObjectiveValueAtTime(const double &muLower, const double &muUpper, const int &timeSlotId) const {
    double result = 0.0;

    // local contribution from loads
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        result += _loadArray[loadId]->objectiveValueAtTime(timeSlotId);
    }
    
    // local contribution from voltages
    if (_hasVoltageConstraint) {
        double voltageMinSquare = _voltageMin * _voltageMin;
        double voltageMaxSquare = _voltageMax * _voltageMax;
        for (int phaseId = 0; phaseId < _phaseIndicesInParentBus.size(); phaseId ++) {
            double voltageSquare = std::norm( _voltages[timeSlotId]._data[phaseId] );
            result -= muLower * log(voltageSquare - voltageMinSquare) + muUpper * log(voltageMaxSquare - voltageSquare);
        }
    }
    
    // contribution from downstream buses
    for (int lineId = 0; lineId < _toLineArray.size(); lineId ++) {
        BusController *toBus = _toLineArray[lineId]->_toBus;
        result += toBus->downstreamObjectiveValueAtTime(muLower, muUpper, timeSlotId);
    }
    
    return result;
}

double BusController::downstreamObjectiveValueOverHorizon(const double &muLower, const double &muUpper) const {
    double result = 0.0;
    
    // local contribution from loads
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        result += _loadArray[loadId]->objectiveValueOverHorizon();
    }
    
    // local contribution from voltages
    if (_hasVoltageConstraint) {
        double voltageMinSquare = _voltageMin * _voltageMin;
        double voltageMaxSquare = _voltageMax * _voltageMax;
        for (int timeSlotId = 0; timeSlotId < _aggregateLoads.size(); timeSlotId ++) {
            for (int phaseId = 0; phaseId < _phaseIndicesInParentBus.size(); phaseId ++) {
                double voltageSquare = std::norm( _voltages[timeSlotId]._data[phaseId] );
                result -= muLower * log(voltageSquare - voltageMinSquare) + muUpper * log(voltageMaxSquare - voltageSquare);
            }
        }
    }
    
    // contribution from downstream buses
    for (int lineId = 0; lineId < _toLineArray.size(); lineId ++) {
        BusController *toBus = _toLineArray[lineId]->_toBus;
        result += toBus->downstreamObjectiveValueOverHorizon(muLower, muUpper);
    }
    
    return result;
}

// compute aggregated downstream power update size
// store result in _powerUpdateSize
double BusController::downstreamUpdateSizeAtTime(const int &timeSlotId) const {
    double result = 0.0;
    
    // local size
    // if (_bus->type() != SUBSTATION)
        result = norm(_aggregateLoads[timeSlotId]._power - _oldAggregateLoads[timeSlotId]._power);
    
    // downstream size;
    for (int lineId = 0; lineId < _toLineArray.size(); lineId ++) {
        double updateSize = _toLineArray[lineId]->_toBus->downstreamUpdateSizeAtTime(timeSlotId);
        if (result < updateSize)
            result = updateSize;
    }
    
    return result;
}

double BusController::downstreamUpdateSizeOverHorizon() const {
    double result = 0.0;
    for (int timeSlotId = 0; timeSlotId < _aggregateLoads.size(); timeSlotId ++) {
        double updateSize = downstreamUpdateSizeAtTime(timeSlotId);
        if (result < updateSize)
            result = updateSize;
    }
    return result;
}

// compute expected downstream expected objective value change
// store result in _expectedObjectiveValueUpdate
double BusController::downstreamExpectedObjectiveValueChangeAtTime(const int &timeSlotId) const {
    double result = 0.0;
    
    // local contribution
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        result += _loadArray[loadId]->expectedObjectiveValueChangeAtTime(timeSlotId);
    }
    
    // downstream contributions
    for (int lineId = 0; lineId < _toLineArray.size(); lineId ++) {
        result += _toLineArray[lineId]->_toBus->downstreamExpectedObjectiveValueChangeAtTime(timeSlotId);
    }
    
    return result;
}

double BusController::downstreamExpectedObjectiveValueChangeOverHorizon() const {
    double result = 0.0;
    
    // local contribution
    for (int loadId = 0; loadId < _loadArray.size(); loadId ++) {
        result += _loadArray[loadId]->expectedObjectiveValueChangeOverHorizon();
    }
    
    // downstream contributions
    for (int lineId = 0; lineId < _toLineArray.size(); lineId ++) {
        result += _toLineArray[lineId]->_toBus->downstreamExpectedObjectiveValueChangeOverHorizon();
    }
    
    return result;
}
