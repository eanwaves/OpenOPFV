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
 * Module FutureData.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__FutureData__
#define __OptimalPowerFlowVisualization__FutureData__

#include "DataType.h"

class FutureData {
private:
    /******************************
     future data storage
     ******************************/
    
    // map load name to future data profile
    unordered_map<string, vector<LoadData *>> _loadNameToFutureData;
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    FutureData();
    
    // copy constructor
    FutureData(const FutureData &futureData);
    
    // release allocated memory
    void clear();
    
    // deconstructor
    ~FutureData();
    
    // assignment
    void operator=(const FutureData &futureData);
    
    // print
    friend ostream &operator<<(ostream &cout, const FutureData &futureData);
    
    
    /******************************
     accessor functions
     ******************************/
    
    // fetch future data for "load"
    // at time stamps in timeArrayInMinutes
    vector<LoadData *> fetchFutureDataForLoad(const string &loadName,
                                              const LoadType &loadType,
                                              const vector<time_type> &timeArrayInMinutes);
    
    // fetch future data for "load" at time "time"
    LoadData *fetchFutureDataForLoad(const string &loadName,
                                     const LoadType &loadType,
                                     const time_type &timeInMinutes);
    
    // add an array of future data for a load
    void addFutureDataForLoad(const string &name,
                              const vector<LoadData *> &loadDataArray);
    
    // add a future data for a load
    void addFutureLoadForLoad(const string &name,
                            LoadData *loadData);
    
    
    /******************************
     other functions
     ******************************/
    
    // release future data for a load till certain time
    void releaseFutureDataForLoadTillTime(const string &name,
                                          const time_type &endTimeInMinutes);
};

#endif /* defined(__OptimalPowerFlowVisualization__FutureData__) */
