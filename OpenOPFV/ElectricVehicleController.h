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
 * Module ElectricVehicleController.h
 *
 ***********************************************************************/

#ifndef __optimalpowerflowvisualization__ElectricVehicleController__
#define __optimalpowerflowvisualization__ElectricVehicleController__

#include "LoadController.h"
#include "ElectricVehicle.h"

class ElectricVehicleController : public LoadController {
public:
    /******************************
     photovoltaic description
     ******************************/
    int _plugInTimeSlotId;
    int _deadlineTimeSlotId;
    time_type _slotLengthInMinutes;
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    ElectricVehicleController(ElectricVehicle *ev,
                              int numberOfSlots,
                              time_type timeSlotLengthInMinutes);
    
    // copy constructor
    ElectricVehicleController(const ElectricVehicleController &ev);
    
    // destructor
    virtual ~ElectricVehicleController();
    
    // assignment
    virtual void operator=(const ElectricVehicleController &ev);
    
    // print
    friend ostream &operator<<(ostream &cout,
                               const ElectricVehicleController *ev);
    
    
    /******************************
     optimization functions
     ******************************/
    
    // compute tentative power consumption, _oldValueArray[timeSlotId] unchanged
    virtual void attemptPowerOverHorizon(const double &stepSize);
};

#endif /* defined(__optimalpowerflowvisualization__ElectricVehicleController__) */
