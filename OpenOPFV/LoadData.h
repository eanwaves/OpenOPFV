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
 * Module LoadData.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__LoadData__
#define __OptimalPowerFlowVisualization__LoadData__

#include "LoadValue.h"

/******************************
 base class for load data
 ******************************/
struct LoadData {
    time_type _timeInMinutes;               // time stamp of load data
    LoadData(const time_type &timeInMinutes = 0);
    virtual ~LoadData();
    virtual LoadData *interpolate(LoadData *data,
                                  const time_type &time) = 0;
};


/******************************
 derived class for base load
 ******************************/
struct BaseLoadData : public LoadData {
    LoadValue _loadValue;                   // load value
    BaseLoadData(const time_type &timeInMinutes = 0,
                 const LoadValue &value = LoadValue());
    BaseLoadData(const string &phase,
                 const time_type &timeInMinutes = 0);
    virtual ~BaseLoadData();
    virtual LoadData *interpolate(LoadData *data,
                                  const time_type &time);
};


/******************************
 derived class for photovoltaic
 ******************************/
struct PhotoVoltaicData : public LoadData {
    double _realPower;          // real power
    PhotoVoltaicData(const time_type &timeInMinutes = 0,
                     const double &realPower = 0.0);
    virtual ~PhotoVoltaicData();
    virtual LoadData *interpolate(LoadData *data,
                                  const time_type &time);
};

#endif /* defined(__OptimalPowerFlowVisualization__LoadData__) */
