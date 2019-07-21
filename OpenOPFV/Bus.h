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
 * Module Bus.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__Bus__
#define __OptimalPowerFlowVisualization__Bus__

#include "DataType.h"
#include "Element.h"

class Line;
class Load;
class FutureData;

class Bus : public Element {
protected:
    /******************************
     node description
     ******************************/
    BusType _type;                                  // bus type
    Coordinate _coordinate;                         // bus coordinate
    SquareMatrix<complex_type> _shunt;              // bus shunt
    LoadValue _aggregateLoad;                       // bus load
    ColumnVector<complex_type> _voltage;            // bus voltage
    
    
    /******************************
     topology infomation
     ******************************/
    Line *_fromLine;                                // upstream line of the bus
    vector<Line *> _toLineArray;                    // downstream line of the bus
    vector<Load *> _loadArray;                      // loads on the bus
    vector<int> _phaseIndicesInParentBus;           // if parent bus has phase abc
                                                    // this bus has phase bc
                                                    // then vector = {1, 2}
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    Bus(const string &name = "bus",
        const string &phase = "abc",
        const BusType &type = BUS);
    
    // copy constructor
    Bus(const Bus &bus);
    
    // release allocated spaces
    void clear();
    
    // deconstructor
    virtual ~Bus();
    
    // assignmet
    virtual void operator=(const Bus &bus);
    
    // print
    friend ostream &operator<<(ostream &cout,
                               const Bus *bus);
    
    
    /******************************
     accessor functions
     ******************************/
    
    // getter functions
    BusType type() const;
    Coordinate coordinate() const;
    SquareMatrix<complex_type> shunt() const;
    LoadValue aggregateLoad() const;
    ColumnVector<complex_type> voltage() const;
    
    Line *fromLine() const;
    vector<Line *> toLineArray() const;
    vector<Load *> loadArray() const;
    vector<int> phaseIndicesInParentBus() const;
    
    // setter functions
    virtual void setPhase(const string &phase);
    void setType(const BusType &type);
    void setCoordinate(const Coordinate &coordinate);
    void setShunt(const SquareMatrix<complex_type> &shunt);
    void setAggregateLoad(const LoadValue &aggregateLoad);
    void setVoltage(const ColumnVector<complex_type> &voltage);
    
    void setFromLine(Line *fromLine);
    void addAToLine(Line *toLine);
    void addALoad(Load *load);
    void deleteALoad(Load *load);
    
    
    /******************************
     initializers
     ******************************/
    void initPhaseIndicesInParentBus();     // initialize phase indices in parent bus
    
    
    /******************************
     power flow computations
     ******************************/
    void initVoltage();                     // initialize voltage to parent bus voltage
    void computeAggregateLoadOnSelf();      // compute aggregate load on the bus
    double computeCurrentOnFromLine();      // compute current by backward sweep
    double computeVoltageOnSelf();          // compute voltage by forward sweep
    
    
    /******************************
     other functions
     ******************************/
    
    // fetch real data from future data when a new timestamp comes
    virtual void fetchRealTimeData(const time_type &oldTimeInMinutes,
                                   const time_type &newTimeInMinutes,
                                   FutureData *futureData);
};

#endif /* defined(__OptimalPowerFlowVisualization__Bus__) */
