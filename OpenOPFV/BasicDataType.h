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
 *    Peter Enescu - initial documentation
 *    Lingwen Gan - initial flow implementation
 *
 * Project OpenOPFV - Optimal Power Flow Visualizer – 7/20/2019
 * Module declarations.h
 *
 ***********************************************************************/

#ifndef OptimalPowerFlowVisualization_BasicDataType_h
#define OptimalPowerFlowVisualization_BasicDataType_h

/******************************
 include files
 ******************************/
#include <iostream>
#include <fstream>
#include <complex>
#include <vector>
#include <deque>
#include <unordered_map>
#include <queue>
#include <unordered_set>


/******************************
 short hands
 ******************************/
using std::ostream;
using std::ofstream;
using std::string;
using std::complex;
using std::vector;
using std::deque;
using std::unordered_map;
using std::unordered_set;
using std::priority_queue;


/******************************
 common data types
 ******************************/
typedef double time_type;
typedef complex<double> complex_type;


/******************************
 used in network model
 ******************************/

// used in bus
enum BusType {SUBSTATION, HOUSE, BUS};
struct Coordinate {
    float _x;
    float _y;
    Coordinate(const float &x = 0.0, const float &y = 0.0) : _x(x), _y(y) {}
};

// used in line and load
enum LineType {LINE, TRANSFORMER};
// enum LoadType {BASELOAD, PHOTOVOLTAIC, ELECTRIC_VEHICLE, POOL_PUMP, AIR_CONDITIONER, CAPACITOR};
typedef int LoadType;
const int BASELOAD = 0;
const int PHOTOVOLTAIC = 1;
const int ELECTRIC_VEHICLE = 2;
const int POOL_PUMP = 3;
const int AIR_CONDITIONER = 4;
const int CAPACITOR = 5;


/******************************
 used in network control
 ******************************/
enum ControlObjective {MINIMIZE_L2_NORM};


/******************************
 used in event queue
 ******************************/

typedef int ActionType;
const int DELETE_A_LOAD = 0;
const int ADD_A_LOAD = 1;
const int LOAD_FUTURE_DATA = 2;
const int SLOW_CONTROL = 3;
const int FAST_CONTROL = 4;
const int UPDATE_TO_TIME = 5;

// used in event queue
class Element;
struct Event {
    time_type _time;
    Element *_target;
    ActionType _action;
    void *_parameter;
    Event(const time_type &time = 0.0,
          Element *target = NULL,
          const ActionType &action = UPDATE_TO_TIME,
          void *parameter = NULL) : _time(time), _target(target), _action(action), _parameter(parameter) {}
};
struct OrderEvent {
    bool operator()(const Event &e1,
                    const Event &e2) {
        if (e1._time != e2._time)
            return e1._time > e2._time;
        else
            return e1._action > e2._action;
    }
};
typedef priority_queue<Event, vector<Event>, OrderEvent> event_queue;

// used in event execution
enum EventReturnType {NEED_NO_ACTION, NEED_PLOTTING, EVENT_QUEUE_EMPTY};

#endif
