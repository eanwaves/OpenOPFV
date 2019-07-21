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
 * Module EventQueue.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__EventQueue__
#define __OptimalPowerFlowVisualization__EventQueue__

#include "DataType.h"

class EventQueue {
private:
    /******************************
     event queue description
     ******************************/
    event_queue _eventQueue;                // events
    time_type _endTime;                     // time to terminate simulation
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    EventQueue();
    
    // copy constructor
    EventQueue(const EventQueue &events);
    
    // release allocated spaces
    void clear();
    
    // deconstructor
    ~EventQueue();
    
    // assignment
    void operator=(const EventQueue &events);
    
    // print
    friend ostream &operator<<(ostream &cout, const EventQueue &events);
    
    
    /******************************
     accessor functions
     ******************************/
    
    time_type endTime() const;
    void setEndTime(const time_type &endTime);
    
    // return true if event queue is empty
    bool empty() const;
    
    // get the earliest event
    Event nextEvent();
    
    // insert an event
    void push(const Event &event);
};

#endif /* defined(__OptimalPowerFlowVisualization__EventQueue__) */
