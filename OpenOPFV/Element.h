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
 * Module Element.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__Element__
#define __OptimalPowerFlowVisualization__Element__

#include "DataType.h"

class Element {
protected:
    /******************************
     element description
     ******************************/
    string _name;           // element name
    string _phase;          // element phase
    
    
public:
    /******************************
     basic functions
     ******************************/
    Element(const string &name = "element",
            const string &phase = "a");                 // default constructor
    Element(const Element &element);                    // copy constructor
    virtual ~Element();                                 // default constructor
    virtual void operator=(const Element &element);     // assignment
    friend ostream &operator<<(ostream &cout,
                               const Element *element); // print
    
    
    /******************************
     accessor functions
     ******************************/
    string name() const;
    string phase() const;
    void setName(const string &name);
    virtual void setPhase(const string &phase);
    
    
    /******************************
     other functions
     ******************************/
    bool validPhase() const;                            // return true if phase is valid
    bool validPhase(const string &phase) const;         // return true if phase is valid
};

#endif /* defined(__OptimalPowerFlowVisualization__Element__) */
