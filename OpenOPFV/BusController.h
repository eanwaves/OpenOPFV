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
 * Module BusController.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__BusController__
#define __OptimalPowerFlowVisualization__BusController__

#include "DataType.h"

class Bus;
class LineController;
class LoadController;
class Load;

class BusController {
public:
    /******************************
     node description
     ******************************/
    Bus *_bus;                                      // the bus to be controlled
    SquareMatrix<complex_type> _shunt;              // bus shunt
    vector<LoadValue> _aggregateLoads;              // aggregate load at different time slots
    vector<ColumnVector<complex_type>> _voltages;   // voltage at different time slots
    
    
    /******************************
     topology information
     ******************************/
    LineController *_fromLine;
    vector<LineController *> _toLineArray;
    vector<LoadController *> _loadArray;
    vector<int> _phaseIndicesInParentBus;
    
    
    /******************************
     constraint description
     ******************************/
    bool _hasVoltageConstraint;
    double _voltageMin;
    double _voltageMax;
    
    
    /******************************
     algorithm variables
     ******************************/
    ColumnVector<complex_type> _beta;               // see paper
    vector<ColumnVector<double>> _sumDown;          // see paper
    vector<ColumnVector<complex_type>> _sumUp;      // see paper
    vector<ColumnVector<complex_type>> _gradient;   // see paper
    vector<LoadValue> _oldAggregateLoads;
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    BusController(Bus *bus = NULL, int numberOfSlots = 1);
    
    // copy constructor
    BusController(const BusController &controller);
    
    // release allocated spaces
    void clear();
    
    // deconstructor
    virtual ~BusController();
    
    // assignment
    virtual void operator=(const BusController &controller);
    
    // print
    friend ostream &operator<<(ostream &cout, const BusController *controller);
    
    
    /******************************
     accessor functions
     ******************************/
    
    // add a load
    void addALoad(LoadController *load);
    
    // delete a load
    void deleteALoad(LoadController *load);
    void deleteALoad(Load *load);
    
    
    /******************************
     power flow computations
     ******************************/
    
    // initialize voltage to parent bus voltage
    void initVoltageAtTime(int timeSlotId);
    void initVoltageOverHorizon();
    
    // compute aggregate load on the bus
    void computeAggregateLoadOnSelfAtTime(int timeSlotId);
    void computeAggregateLoadOnSelfOverHorizon();
    
    // compute current by backward sweep
    double computeCurrentOnFromLineAtTime(int timeSlotId);
    double computeCurrentOnFromLineOverHorizon();
    
    // compute voltage by forward sweep
    double computeVoltageOnSelfAtTime(int timeSlotId);
    double computeVoltageOnSelfOverHorizon();
    
    
    /******************************
     compute gradient
     ******************************/
    
    // compute sumDown
    void computeSumDownAtTime(double muLower, double muUpper, const vector<double> &price, const int &timeSlotId);
    void computeSumDownOverHorizon(double muLower, double muUpper, const vector<double> &price);
    
    // compute sumUp
    void computeSumUpAtTime(const int &timeSlotId);
    void computeSumUpOverHorizon();
    
    // compute gradient
    void computeGradientAtTime(const vector<double> &price, const int &timeSlotId);
    void computeGradientOverHorizon(const vector<double> &price);
    
    
    /******************************
     update/reset/attempt power consumption
     ******************************/
    
    // compute tentative power consumption, _oldValueArray[timeSlotId] unchanged
    void attemptPowerAtTime(const double &stepSize, const int &timeSlotId, unordered_set<LoadType> &enabledInControl);
    void attemptPowerOverHorizon(const double &stepSize, unordered_set<LoadType> &enabledInControl);
    
    // set _oldValueArray[timeSlotId] to _valueArray[timeSlotId]
    // set _oldAggregateLoads[timeSlotId] to _aggregateLoads[timeSlotId]
    void updatePowerAtTime(const int &timeSlotId, unordered_set<LoadType> &enabledInControl);
    void updatePowerOverHorizon(unordered_set<LoadType> &enabledInControl);
    
    // set _valueArray[timeSlotId] to _oldValueArray[timeSlotId]
    void resetPowerAtTime(const int &timeSlotId, unordered_set<LoadType> &enabledInControl);
    void resetPowerOverHorizon(unordered_set<LoadType> &enabledInControl);
    
    // check if there is voltage violation downstream
    bool downstreamVoltageViolationAtTime(const int &timeSlotId) const;
    bool downstreamVoltageViolationOverHorizon() const;
    
    // compute aggregated downstream objective value
    // store result in _objectiveValueDownstream
    double downstreamObjectiveValueAtTime(const double &muLower, const double &muUpper, const int &timeSlotId) const;
    double downstreamObjectiveValueOverHorizon(const double &muLower, const double &muUpper) const;
    
    // compute aggregated downstream power update size
    // store result in _powerUpdateSize
    double downstreamUpdateSizeAtTime(const int &timeSlotId) const;
    double downstreamUpdateSizeOverHorizon() const;
    
    // compute expected downstream expected objective value change
    // store result in _expectedObjectiveValueUpdate
    double downstreamExpectedObjectiveValueChangeAtTime(const int &timeSlotId) const;
    double downstreamExpectedObjectiveValueChangeOverHorizon() const;
};

#endif /* defined(__OptimalPowerFlowVisualization__BusController__) */
