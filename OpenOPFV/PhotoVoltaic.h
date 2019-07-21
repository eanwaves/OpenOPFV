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
 * Module PhotoVoltaic.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__PhotoVoltaic__
#define __OptimalPowerFlowVisualization__PhotoVoltaic__

#include "Load.h"

class PhotoVoltaic : public Load {
protected:
    /******************************
     photovoltaic description
     ******************************/
    double _nameplate;                          // nameplate of the photovoltaic inverter
    
    
public:
    /******************************
     basic functions
     ******************************/
    PhotoVoltaic(const string &name = "photovoltaic",
                 const string &phase = "a",
                 const LoadType &loadType = PHOTOVOLTAIC);      // default constructor
    PhotoVoltaic(const PhotoVoltaic &pv);                       // copy constructor
    virtual ~PhotoVoltaic();                                    // destructor
    virtual void operator=(const PhotoVoltaic &pv);             // assignment
    friend ostream &operator<<(ostream &cout,
                               const PhotoVoltaic *pv);         // print
    
    
    /******************************
     accessor functions
     ******************************/
    double nameplate() const;
    void setNameplate(const double &nameplate);
    
    
    /******************************
     other functions
     ******************************/
    
    // fetch real data from future data when a new timestamp comes
    virtual void fetchRealTimeData(const time_type &oldTimeInMinutes,
                                   const time_type &newTimeInMinutes,
                                   FutureData *futureData);
};

#endif /* defined(__OptimalPowerFlowVisualization__PhotoVoltaic__) */
