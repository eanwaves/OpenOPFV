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
 * Module LoadController.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__LoadController__
#define __OptimalPowerFlowVisualization__LoadController__

#include "DataType.h"

class Load;
class BusController;

class LoadController {
public:
    /******************************
     load description
     ******************************/
    Load *_load;
    vector<LoadValue> _valueArray;      // load value at different time slots
    
    
    /******************************
     topology information
     ******************************/
    BusController *_locationBus;
    vector<int> _phaseIndicesInLocationBus;
    vector<LoadValue> _oldValueArray;
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    LoadController(Load *load = NULL, int numberOfSlots = 1);
    
    // copy constructor
    LoadController(const LoadController &controller);
    
    // destructor
    virtual ~LoadController();
    
    // assignment
    virtual void operator=(const LoadController &controller);
    
    // print
    friend ostream &operator<<(ostream &cout,
                               const LoadController &controller);
    
    
    /******************************
     optimization functions
     ******************************/
    
    // compute tentative power consumption, _oldValueArray[timeSlotId] unchanged
    virtual void attemptPowerAtTime(const double &stepSize, const int &timeSlotId);
    virtual void attemptPowerOverHorizon(const double &stepSize);
    
    // set _oldValueArray[timeSlotId] to _valueArray[timeSlotId]
    void updatePowerAtTime(const int &timeSlotId);
    void updatePowerOverHorizon();
    
    // set _valueArray[timeSlotId] to _oldValueArray[timeSlotId] (if exists)
    void resetPowerAtTime(const int &timeSlotId);
    void resetPowerOverHorizon();
    
    // compute objective value
    virtual double objectiveValueAtTime(const int &timeSlotId);
    virtual double objectiveValueOverHorizon();
    
    // compute the expected change of the objective value
    virtual double expectedObjectiveValueChangeAtTime(const int &timeSlotId);
    virtual double expectedObjectiveValueChangeOverHorizon();
};

#endif /* defined(__OptimalPowerFlowVisualization__LoadController__) */
