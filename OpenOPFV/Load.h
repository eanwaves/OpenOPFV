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
 * Module Load.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__Load__
#define __OptimalPowerFlowVisualization__Load__

#include "DataType.h"
#include "Element.h"

class Bus;
class FutureData;

class Load : public Element {
protected:
    /******************************
     load description
     ******************************/
    LoadType _type;                                 // load type
    LoadValue _value;                               // load value
    
    
    /******************************
     topology infomation
     ******************************/
    Bus *_locationBus;                              // location of the load
    vector<int> _phaseIndicesInLocationBus;         // if parent bus has phase abc
    
    
public:
    /******************************
     basic functions
     ******************************/
    Load(const string &name = "load",
         const string &phase = "a",
         const LoadType &type = BASELOAD);          // default constructor
    Load(const Load &load);                         // copy constructor
    virtual ~Load();                                // deconstructor
    virtual void operator=(const Load &load);       // assignment
    friend ostream &operator<<(ostream &cout,
                               const Load *load);   // print
    
    
    /******************************
     accessor functions
     ******************************/
    LoadType type() const;
    LoadValue value() const;
    
    Bus *locationBus() const;
    vector<int> phaseIndicesInLocationBus() const;
    
    virtual void setPhase(const string &phase);
    void setType(const LoadType &type);
    void setValue(const LoadValue &value);
    
    void setLocationBus(Bus *locationBus);
    
    
    /******************************
     initializers
     ******************************/
    void initPhaseIndicesInLocationBus();           // initialize phase indices at location bus
    
    
    /******************************
     other functions
     ******************************/
    
    // fetch real data from future data when a new timestamp comes
    virtual void fetchRealTimeData(const time_type &oldTimeInMinutes,
                                   const time_type &newTimeInMinutes,
                                   FutureData *futureData);
};

#endif /* defined(__OptimalPowerFlowVisualization__Load__) */
