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
 *    Peter Enescu - initial documentation
 *    Lingwen Gan - initial flow implementation
 *
 * Project OpenOPFV - Optimal Power Flow Visualizer – 7/20/2019
 * Module LoadData.cpp
 *
 ***********************************************************************/

#include "LoadData.h"

/******************************
 base class for load data
 ******************************/

LoadData::LoadData(const time_type &timeInMinutes) : _timeInMinutes(timeInMinutes) {
}

LoadData::~LoadData() {
}


/******************************
 derived class for base load
 ******************************/

BaseLoadData::BaseLoadData(const time_type &timeInMinutes,
                           const LoadValue &value) :
LoadData(timeInMinutes), _loadValue(value) {
}

BaseLoadData::BaseLoadData(const string &phase,
                           const time_type &timeInMinutes) :
LoadData(timeInMinutes), _loadValue(phase) {
}

BaseLoadData::~BaseLoadData() {
}

LoadData *BaseLoadData::interpolate(LoadData *data,
                                    const time_type &time) {
    // get time of the second entry
    time_type time2 = data->_timeInMinutes;
    if (_timeInMinutes == time2 ||
        _timeInMinutes > time ||
        time2 < time) {
        std::cout << "Invalid time stamps for interpolation!" << std::endl;
        return NULL;
    }
    
    // get value of the second entry
    BaseLoadData *data2 = (BaseLoadData *)data;
    LoadValue value2 = data2->_loadValue;
    
    // get return value
    value2 = ( value2 * (time - _timeInMinutes) + _loadValue * (time2 - time) ) / (time2 - _timeInMinutes);
    
    return new BaseLoadData(time, value2);
}


/******************************
 derived class for photovoltaic
 ******************************/

PhotoVoltaicData::PhotoVoltaicData(const time_type &timeInMinutes,
                                   const double &realPower) :
LoadData(timeInMinutes), _realPower(realPower) {
}

PhotoVoltaicData::~PhotoVoltaicData() {
}

LoadData *PhotoVoltaicData::interpolate(LoadData *data,
                                        const time_type &time) {
    // get time of the second entry
    time_type time2 = data->_timeInMinutes;
    if (_timeInMinutes == time2 ||
        _timeInMinutes > time ||
        time2 < time) {
        std::cout << "Invalid time stamps for interpolation!" << std::endl;
        return NULL;
    }
    
    // get value of the second entry
    PhotoVoltaicData *data2 = (PhotoVoltaicData *)data;
    double power2 = data2->_realPower;
    
    // get return value
    power2 = (power2 * (time - _timeInMinutes) + _realPower * (time2 - time)) / (time2 - _timeInMinutes);
    
    return new PhotoVoltaicData(time, power2);
}
