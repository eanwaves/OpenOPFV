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
 * Module NetworkControl.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__NetworkControl__
#define __OptimalPowerFlowVisualization__NetworkControl__

#include "DataType.h"
#include "NetworkModel.h"
#include "BusController.h"
#include "LineController.h"
#include "LoadController.h"
#include "LoadPredictor.h"

class NetworkControl {
public:
    /******************************
     network description
     ******************************/
    vector<BusController *> _buses;                                     // buses in the network
    vector<LineController *> _lines;                                    // lines in the network
    unordered_map<Bus *, BusController *> _busToControllerHashTable;    // map bus to controllers
    
    
    /******************************
     network controller description
     ******************************/
    unordered_set<LoadType> _enabledInFastControl;
    unordered_set<LoadType> _enabledInSlowControl;
    int _numberOfSlots;                             // number of slots in slow control
    time_type _slotLengthInMinutes;                 // length of each time slot
    double _quadCoef;                               // quadratic coefficient in objective function
    double _linCoef;                                // linear coefficient in objective function
    
    
    /******************************
     algorithm variables
     ******************************/
    vector<vector<int>> _busPhaseIndicesInRoot;     // position of bus indices at the root node
    double _muLower, _muUpper;                      // in log barrier function
    double _stepSize;
    double _oldObjectiveValue;
    double _substationVoltage;
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    NetworkControl();
    
    // copy constructor
    NetworkControl(const NetworkControl &control);
    
    // clear allocated spaces
    void clear();
    
    // deconstructor
    ~NetworkControl();
    
    // assignment
    void operator=(const NetworkControl &control);
    
    // print
    friend ostream &operator<<(ostream &cout, const NetworkControl &control);
    void printPowerFlow(int timeSlotId = 0, const string &mode = "overwrite");
    void printGradient(int timeSlotId = 0, ostream &cout = std::cout);
    void printGradientOverHorizon(ostream &cout);
    void printPowerConsumption(int timeSlotId = 0, ostream &cout = std::cout);
    void printSlowControlResult(ostream &cout = std::cout);
    void printViolatedVoltages(ostream &cout);
    
    
    /******************************
     initializer
     ******************************/
    
    // set numberOfSlots
    void setNumberOfSlots(int numberOfSlots);
    
    // set substation voltage
    void setSubstationVoltage(const double &substationVoltage);
    
    // initialize networkControl according to networkModel
    void initialize(const NetworkModel &model);
    
    // add a bus
    void addABus(Bus *bus);
    
    // add a line
    void addALine(Line *line);
    
    // add a load
    void addALoad(Load *load);
    
    // delete a load
    void deleteALoad(Load *load);
    
    
    /******************************
     event execution
     ******************************/
    
    // do fast control
    void fastControl();
    
    // do slow control
    void slowControl(time_type time);
    
    // apply control outcome
    void applyControl();
    
    
    /******************************
     power flow computation
     ******************************/
    
    // initialize the voltages
    // must be called before performing computePowerFlow
    void initVoltageAtTime(int timeSlotId);
    void initVoltageOverHorizon();
    
    // compute power flow with internal model
    void computePowerFlowAtTime(int timeSlotId,
                                const int &maxIteration = 15,
                                const double &updateSizeThreshold = 1e-6);
    void computePowerFlowOverHorizon(const int &maxIteration = 15,
                                     const double &updateSizeThreshold = 1e-6);
    
    
    /******************************
     gradient estimation
     ******************************/
    
    // compute gradients
    void computeGradientAtTime(int timeSlotId);
    void computeGradientOverHorizon();
    
    
    /******************************
     line search
     ******************************/
    
    // compute tentative power consumptions
    void attemptPowerAtTime(const int &timeSlotId, unordered_set<LoadType> &enabledInControl);
    void attemptPowerOverHorizon(unordered_set<LoadType> &enabledInControl);
    
    // set oldValue to current power consumption
    void updatePowerAtTime(const int &timeSlotId, unordered_set<LoadType> &enabledInControl);
    void updatePowerOverHorizon(unordered_set<LoadType> &enabledInControl);
    
    // set current power consumption to oldValue
    void resetPowerAtTime(const int &timeSlotId, unordered_set<LoadType> &enabledInControl);
    void resetPowerOverHorizon(unordered_set<LoadType> &enabledInControl);
    
    // check if there is voltage vilation
    bool voltageViolationAtTime(const int &timeSlotId) const;
    bool voltageViolationOverHorizon() const;
    
    // compute objective value
    double objectiveValueAtTime(const int &timeSlotId) const;
    double objectiveValueOverHorizon() const;
    
    // compute power update size
    double updateSizeAtTime(const int &timeSlotId) const;
    double updateSizeOverHorizon() const;
    
    // compute expected objective value change
    double expectedObjectiveValueChangeAtTime(const int &timeSlotId) const;
    double expectedObjectiveValueChangeOverHorizon() const;
    
    
    /******************************
     solve the power flow problem
     ******************************/
    
    // initialize algorithm variables for fast/slow control
    void fastControlInitialize();
    void slowControlInitialize(time_type time);
    
    // outer loop of the solver
    // in each iteration of the outer loop, use a different log-barrier function
    // return the computed objective value
    // muArray is an array of the mu values to be used in the inner loop, alpha is the backoff parameter, beta is the linearization quantification parameter, and epsilon is the threshold of power consumptions update
    double fastControlOuterLoop(std::vector<double> muArray = std::vector<double>(), double alpha = 0.5, double beta = 0.5, double epsilon = 1e-4);
    double slowControlOuterLoop(std::vector<double> muArray = std::vector<double>(), double alpha = 0.5, double beta = 0.5, double epsilon = 1e-4);
    
    // inner loop of the solver
    // in each iteration of the inner loop, do gradient decent to solve the OPF problem with a fixed log barrier function
    // return the number of iterations used
    // muLow is the mu value used in log(v-vMin), muUpp is the mu value used in log(vMax-v), alpha is the backoff parameter, beta is the linearization quantification parameter, and epsilon is the threshold of power consumptions update
    int fastControlInnerLoop(double alpha, double beta, double epsilon);
    int slowControlInnerLoop(double alpha, double beta, double epsilon);
};

#endif /* defined(__OptimalPowerFlowVisualization__NetworkControl__) */
