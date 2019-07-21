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
 * Module LoadPredictor.cpp
 *
 ***********************************************************************/

#include "LoadPredictor.h"
#include "NetworkControl.h"
#include "FutureData.h"
#include "ElectricVehicle.h"

/******************************
 basic functions
 ******************************/

// default constructor
LoadPredictor::LoadPredictor() {
}

// copy constructor
LoadPredictor::LoadPredictor(const LoadPredictor &loadPredictor) {
}

// release allocated memory
void LoadPredictor::clear() {
}

// deconstructor
LoadPredictor::~LoadPredictor() {
    clear();
}

// assignment
void LoadPredictor::operator=(const LoadPredictor &loadPredictor) {
}

// print
ostream &operator<<(ostream &cout, const LoadPredictor &loadPredictor) {
    return cout;
}


/******************************
 other functions
 ******************************/

// make prediction for "control", using data from "futureData"
// prediction starts from "startTime" and lasts for "predictionWindow"
// each time slot is of length "slowControlPeriod"
void LoadPredictor::makePrediction(NetworkControl *control,
                                   FutureData *futureData,
                                   const time_type &startTimeInMinutes,
                                   const time_type &slowControlPeriodInMinutes) {
    // direct load from future data (assuming no prediction error)
    for (int busId = 0; busId < control->_buses.size(); busId ++) {
        BusController *bus = control->_buses[busId];
        for (int loadId = 0; loadId < bus->_loadArray.size(); loadId ++) {
            
            // get the load to make prediction on
            LoadController *load = bus->_loadArray[loadId];
            string name = load->_load->name();
            LoadType type = load->_load->type();
            
            switch ( type ) {
                    // if load is a baseload
                case BASELOAD: {
                    time_type time = startTimeInMinutes;
                    int timeSlotId = 0;
                    while (timeSlotId < load->_valueArray.size()) {
                        BaseLoadData *data = (BaseLoadData *) futureData->fetchFutureDataForLoad(name, type, time);
                        load->_valueArray[timeSlotId] = data->_loadValue;
                        delete data;
                        timeSlotId ++;
                        time += slowControlPeriodInMinutes;
                    }
                    break;
                }
                    
                    // if load is a photovoltaic
                case PHOTOVOLTAIC: {
                    time_type time = startTimeInMinutes;
                    int timeSlotId = 0;
                    while (timeSlotId < load->_valueArray.size()) {
                        PhotoVoltaicData *data = (PhotoVoltaicData *) futureData->fetchFutureDataForLoad(name, type, time);
                        load->_valueArray[timeSlotId].reset();
                        load->_valueArray[timeSlotId]._power[0] = - data->_realPower;
                        delete data;
                        timeSlotId ++;
                        time += slowControlPeriodInMinutes;
                    }
                    break;
                }
                    
                    // if load is an electric vehicle
                case ELECTRIC_VEHICLE: {
                    ElectricVehicle *ev = (ElectricVehicle *)load->_load;
                    double capacity = ev->_futureEnergyRequest;
                    time_type time = startTimeInMinutes;
                    int timeSlotId = 0;
                    while (timeSlotId < load->_valueArray.size()) {
                        load->_valueArray[timeSlotId].reset();
                        if (time >= ev->_plugInTime &&
                            time <= ev->_deadline) {
                            double fullRate = capacity * 60 / slowControlPeriodInMinutes;
                            if (fullRate < ev->_maxChargingRate)
                                load->_valueArray[timeSlotId]._power[0] = fullRate;
                            else
                                load->_valueArray[timeSlotId]._power[0] = ev->_maxChargingRate;
                            capacity -= load->_valueArray[timeSlotId]._power[0].real() * slowControlPeriodInMinutes / 60.0;
                            if (capacity < 0.0)
                                capacity = 0.0;
                        }
                        timeSlotId ++;
                        time += slowControlPeriodInMinutes;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}
