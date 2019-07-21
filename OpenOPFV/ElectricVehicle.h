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
 * Module ElectricVehicle.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__ElectricVehicle__
#define __OptimalPowerFlowVisualization__ElectricVehicle__

#include "Load.h"

class ElectricVehicle : public Load {
public:
    /******************************
     Electric vehicle description
     ******************************/
    double _maxChargingRate;            // in per unit power
    double _futureEnergyRequest;        // in per unit power * hour
    time_type _plugInTime;
    time_type _deadline;
    
    
public:
    /******************************
     basic functions
     ******************************/
    ElectricVehicle(const string &name = "electricVehicle",
                    const string &phase = "a",
                    const LoadType &type = ELECTRIC_VEHICLE);   // default constructor
    ElectricVehicle(const ElectricVehicle &ev);                 // copy constructor
    virtual ~ElectricVehicle();                                 // deconstructor
    virtual void operator=(const ElectricVehicle &ev);          // assignment
    friend ostream &operator<<(ostream &cout,
                               const ElectricVehicle *ev);      // print
    
    
    /******************************
     other functions
     ******************************/
    
    // fetch real data from future data when a new timestamp comes
    virtual void fetchRealTimeData(const time_type &oldTimeInMinutes,
                                   const time_type &newTimeInMinutes,
                                   FutureData *futureData);
    
};

#endif /* defined(__OptimalPowerFlowVisualization__ElectricVehicle__) */
