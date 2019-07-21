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
 * Module NetworkModel.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__NetworkModel__
#define __OptimalPowerFlowVisualization__NetworkModel__

#include "DataType.h"
#include "Bus.h"
#include "Line.h"
#include "Load.h"

class FutureData;

class NetworkModel {
private:
    /******************************
     network description
     ******************************/
    vector<Bus *> _buses;                                       // buses in the network
    vector<Line *> _lines;                                      // lines in the network
    unordered_map<string, Bus *> _busNameToPointerHashTable;    // map bus names to pointers
public:
    double _substationVoltage;
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    NetworkModel();
    
    // copy constructor
    NetworkModel(const NetworkModel &model);
    
    // release allocated memory
    void clear();
    
    // destructor
    ~NetworkModel();
    
    // assignment
    void operator=(const NetworkModel &model);
    
    // print
    friend ostream &operator<<(ostream &cout, const NetworkModel &model);
    void printPowerFlow(const string &mode = "overwrite");
    
    
    /******************************
     accessor functions
     ******************************/
    
    // getter functions
    int numberOfBuses() const;
    Bus *getBusByIndex(const int &index) const;
    Bus *getBusByName(const string &busName);
    int numberOfLines() const;
    Line *getLineByIndex(const int &index) const;
    vector<Line *> lines() const;
    
    // setter functions
    void addABus(Bus *bus);
    void addALine(Line *line);
    void addALoad(Load *load);
    void deleteALoad(Load *load);
    
    
    /******************************
     event execution
     ******************************/
    
    // fetch real data from future data when a new timestamp comes
    void fetchRealTimeData(const time_type &oldTimeInMinutes,
                           const time_type &newTimeInMinutes,
                           FutureData *futureData);
    
    
    /******************************
     power flow computation
     ******************************/
    
    // sort _buses and _lines according to breadth first search
    // must be called before performing "backward-forward sweep"
    void sortBusAndLineByBreadthFirstSearch();
    
    // initialize the voltages
    // must be called before performing computePowerFlowWithSimulator
    // and after performing sortBusAndLineByBreadthFirstSearch
    void initVoltage();
    
    // compute power flow with internal model
    void computePowerFlowWithSimulator(const int &maxIteration = 15,
                                       const double &updateSizeThreshold = 1e-6);
    
    // compute power flow with GridLabD
    void computePowerFlowWithGridLabD();
};

#endif /* defined(__OptimalPowerFlowVisualization__NetworkModel__) */
