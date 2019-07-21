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
 *    Peter Enescu - initial implementation and test
 *
 * Project OpenOPFV - Optimal Power Flow Visualizer – 7/20/2019
 * Module PhotoVoltaic.cpp
 *
 ***********************************************************************/

#include "PhotoVoltaic.h"
#include "LoadController.h"
#include "FutureData.h"

/******************************
 basic functions
 ******************************/

// default constructor
PhotoVoltaic::PhotoVoltaic(const string &name,
                           const string &phase,
                           const LoadType &loadType) :
Load(name, phase, loadType) {
}

// copy constructor
PhotoVoltaic::PhotoVoltaic(const PhotoVoltaic &pv) :
Load(pv),
_nameplate(pv._nameplate) {
}

// deconstructor
PhotoVoltaic::~PhotoVoltaic() {
}

// assignment
void PhotoVoltaic::operator=(const PhotoVoltaic &pv) {
    Load::operator=(pv);
    _nameplate = pv._nameplate;
}

// print
ostream &operator<<(ostream &cout, const PhotoVoltaic *pv) {
    cout << (Load *)pv;
    return cout;
}


/******************************
 accessor functions
 ******************************/

double PhotoVoltaic::nameplate() const {
    return _nameplate;
}

void PhotoVoltaic::setNameplate(const double &nameplate) {
    _nameplate = nameplate;
}


/******************************
 other functions
 ******************************/

// fetch real data from future data when a new timestamp comes
void PhotoVoltaic::fetchRealTimeData(const time_type &oldTimeInMinutes,
                                     const time_type &newTimeInMinutes,
                                     FutureData *futureData) {
    PhotoVoltaicData *data = (PhotoVoltaicData *)(futureData->fetchFutureDataForLoad(_name, _type, newTimeInMinutes));
    futureData->releaseFutureDataForLoadTillTime(_name, newTimeInMinutes);
    _value._admittance = SquareMatrix<complex_type>(_phase);
    _value._admittance[0][0] = 0;
    _value._power[0] = complex_type(- data->_realPower, 0.0);
    delete data;
}
