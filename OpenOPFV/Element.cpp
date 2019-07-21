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
 *    Peter Enescu - initial implementation, documentation and test
 *
 * Project OpenOPFV - Optimal Power Flow Visualizer – 7/20/2019
 * Module Element.cpp
 *
 ***********************************************************************/

#include "Element.h"

/******************************
 basic functions
 ******************************/

// defulat constructor
Element::Element(const string &name,
                 const string &phase)
: _name(name), _phase(phase) {
    if (!validPhase()) {
        _phase = "a";
        std::cout << "Invalid phase, set to a by default" << std::endl;
    }
}

// copy constructor
Element::Element(const Element &element)
: _name(element._name), _phase(element._phase) {
}

// deconstructor
Element::~Element() {
}

// assignment
void Element::operator=(const Element &element) {
    _name = element._name;
    _phase = element._phase;
}

// print
ostream &operator<<(ostream &cout, const Element *element) {
    cout << element->name() << ' ' << element->phase();
    return cout;
}


/******************************
 accessor functions
 ******************************/
string Element::name() const {
    return _name;
}

string Element::phase() const {
    return _phase;
}

void Element::setName(const string &name) {
    _name = name;
}

void Element::setPhase(const string &phase) {
    _phase = phase;
}


/******************************
 other functions
 ******************************/

// return true if phase is valid
bool Element::validPhase() const {
    return validPhase(_phase);
}

// return false is phase is inconsistent with some fields
bool Element::validPhase(const string &phase) const {
    if (phase.empty())  return false;
    if (phase[0] < 'a') return false;
    
    for (int i = 1; i < phase.size(); i ++) {
        if (phase[i] <= phase[i-1])
            return false;
    }
    return phase.back() <= 'c';
}
