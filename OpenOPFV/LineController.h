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
 * Module LineController.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__LineController__
#define __OptimalPowerFlowVisualization__LineController__

#include "DataType.h"

class Line;
class BusController;

class LineController {
public:
    /******************************
     edge description
     ******************************/
    Line *_line;                                        // the line to be controlled
    SquareMatrix<complex_type> _impedance;              // line impedance
    vector<ColumnVector<complex_type>> _currentArray;   // current at different time slots
    
    
    /******************************
     topology infomation
     ******************************/
    BusController *_fromBus;                            // upstream bus
    BusController *_toBus;                              // downstream bus
    vector<int> _phaseIndicesInFromBus;             // phase indices in from bus
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    LineController(Line *line = NULL, int numberOfSlots = 1);
    
    // copy constructor
    LineController(const LineController &controller);
    
    // deconstructor
    virtual ~LineController();
    
    // assignment
    virtual void operator=(const LineController &controller);
    
    // print
    friend ostream &operator<<(ostream &cout, const LineController *controller);
};

#endif /* defined(__OptimalPowerFlowVisualization__LineController__) */
