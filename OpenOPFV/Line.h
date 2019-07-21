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
 * Module Line.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__Line__
#define __OptimalPowerFlowVisualization__Line__

#include "DataType.h"
#include "Element.h"

class Bus;

class Line : public Element {
protected:
    /******************************
     edge description
     ******************************/
    LineType _type;                                 // line type
    SquareMatrix<complex_type> _impedance;          // line impedance
    ColumnVector<complex_type> _current;            // line current
    
    
    /******************************
     topology infomation
     ******************************/
    Bus *_fromBus;                                  // upstream bus
    Bus *_toBus;                                    // downstream bus
    vector<int> _phaseIndicesInFromBus;             // phase indices in from bus
    
    
public:
    /******************************
     basic functions
     ******************************/
    Line(const string &name = "line",
         const string &phase = "abc",
         const LineType &type = LINE);              // default constructor
    Line(const Line &line);                         // copy constructor
    virtual ~Line();                                // deconstructor
    virtual void operator=(const Line &line);       // assignment
    friend ostream &operator<<(ostream &cout,
                               const Line *line);   // print
    
    
    /******************************
     accessor functions
     ******************************/
    LineType type() const;
    SquareMatrix<complex_type> impedance() const;
    ColumnVector<complex_type> current() const;
    
    Bus *fromBus() const;
    Bus *toBus() const;
    vector<int> phaseIndicesInFromBus() const;
    
    
    virtual void setPhase(const string &phase);
    void setType(const LineType &type);
    void setImpedance(const SquareMatrix<complex_type> &impedance);
    void setCurrent(const ColumnVector<complex_type> &current);
    
    void setFromBus(Bus *fromBus);
    void setToBus(Bus *toBus);

    
    /******************************
     initializers
     ******************************/
    void initPhaseIndicesInFromBus();               // initialize phase indices at from bus
};

#endif /* defined(__OptimalPowerFlowVisualization__Line__) */
