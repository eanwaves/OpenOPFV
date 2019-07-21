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
 * Module PhotoVoltaicController.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__PhotoVoltaicController__
#define __OptimalPowerFlowVisualization__PhotoVoltaicController__

#include "LoadController.h"

class PhotoVoltaic;

class PhotoVoltaicController : public LoadController {
public:
    /******************************
     photovoltaic description
     ******************************/
    double _nameplate;
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    PhotoVoltaicController(PhotoVoltaic *pv = NULL, int numberOfSlots = 1);
    
    // copy constructor
    PhotoVoltaicController(const PhotoVoltaicController &controller);
    
    // deconstructor
    virtual ~PhotoVoltaicController();
    
    // assignment
    virtual void operator=(const PhotoVoltaicController &controller);
    
    // print
    friend ostream &operator<<(ostream &cout,
                               const PhotoVoltaicController *controller);
    
    
    /******************************
     optimization functions
     ******************************/
    
    // compute tentative power consumption, _oldValueArray[timeSlotId] unchanged
    virtual void attemptPowerAtTime(const double &stepSize, const int &timeSlotId);
    virtual void attemptPowerOverHorizon(const double &stepSize);
};

#endif /* defined(__OptimalPowerFlowVisualization__PhotoVoltaicController__) */
