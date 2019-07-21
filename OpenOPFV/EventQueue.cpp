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
 * Module EventQueue.cpp
 *
 ***********************************************************************/

#include "EventQueue.h"
#include "Load.h"

/******************************
 basic functions
 ******************************/

// default constructor
EventQueue::EventQueue() {
    _endTime = 1e100;
}

// copy constructor
EventQueue::EventQueue(const EventQueue &events) : _eventQueue(events._eventQueue), _endTime(events._endTime) {
}

// release allocated spaces
void EventQueue::clear() {
    while (!_eventQueue.empty()) {
        Event event = _eventQueue.top();
        _eventQueue.pop();
        if (event._action == ADD_A_LOAD) {
            Load *load = (Load *)(event._parameter);
            delete load;
        }
    }
}

// deconstructor
EventQueue::~EventQueue() {
    clear();
}

// assignment
void EventQueue::operator=(const EventQueue &events) {
    _eventQueue = events._eventQueue;
    _endTime = events._endTime;
}

// print
ostream &operator<<(ostream &cout, const EventQueue &events) {
    cout << "A total of " << events._eventQueue.size() << "events\n";
    return cout;
}


/******************************
 accessor functions
 ******************************/

time_type EventQueue::endTime() const {
    return _endTime;
}

void EventQueue::setEndTime(const time_type &endTime) {
    _endTime = endTime;
}

// return true if event queue is empty
bool EventQueue::empty() const {
    return _eventQueue.empty();
}

// get the earliest event
Event EventQueue::nextEvent() {
    Event result = _eventQueue.top();
    _eventQueue.pop();
    return result;
}

// insert an event
void EventQueue::push(const Event &event) {
    if (event._time < _endTime) {
        _eventQueue.push(event);
    }
}
