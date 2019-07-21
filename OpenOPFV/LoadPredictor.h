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
 * Module LoadPredictor.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__LoadPredictor__
#define __OptimalPowerFlowVisualization__LoadPredictor__

#include "DataType.h"

class NetworkControl;
class FutureData;

class LoadPredictor {
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    LoadPredictor();
    
    // copy constructor
    LoadPredictor(const LoadPredictor &loadPredictor);
    
    // release allocated memory
    void clear();
    
    // deconstructor
    ~LoadPredictor();
    
    // assignment
    virtual void operator=(const LoadPredictor &loadPredictor);
    
    // print
    friend ostream &operator<<(ostream &cout, const LoadPredictor &loadPredictor);
    
    
    /******************************
     event handle
     ******************************/
    
    // make prediction for "control", using data from "futureData"
    // prediction starts from "startTime" and lasts for "predictionWindow"
    // each time slot is of length "slowControlPeriod"
    void makePrediction(NetworkControl *control,
                        FutureData *futureData,
                        const time_type &startTimeInMinutes,
                        const time_type &slowControlPeriodInMinutes);
};

#endif /* defined(__OptimalPowerFlowVisualization__LoadPredictor__) */
