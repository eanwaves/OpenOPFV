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
 * Module FutureData.cpp
 *
 ***********************************************************************/

#include "FutureData.h"

/******************************
 basic functions
 ******************************/

// default constructor
FutureData::FutureData() {
}

// copy constructor
FutureData::FutureData(const FutureData &futureData) :
_loadNameToFutureData(futureData._loadNameToFutureData) {
}

// release allocated memory
void FutureData::clear() {
    unordered_map<string, vector<LoadData *>>::iterator it = _loadNameToFutureData.begin();
    while (it != _loadNameToFutureData.end()) {
        int numberOfLoadData = int( it->second.size() );
        for (int dataId = 0; dataId < numberOfLoadData; dataId ++) {
            delete it->second[dataId];
        }
        it->second.clear();
        it ++;
    }
    _loadNameToFutureData.clear();
}

// deconstructor
FutureData::~FutureData() {
    clear();
}

// assignment
void FutureData::operator=(const FutureData &futureData) {
    _loadNameToFutureData = futureData._loadNameToFutureData;
}

// print
ostream &operator<<(ostream &cout, const FutureData &futureData) {
    unordered_map<string, vector<LoadData *>> hashTable = futureData._loadNameToFutureData;
    unordered_map<string, vector<LoadData *>>::iterator it = hashTable.begin();
    while (it != hashTable.end()) {
        cout << it->first << '\n';
        int numSlot = int( it->second.size() );
        for (int slotId = 0; slotId < numSlot; slotId ++) {
            cout << '\t' << it->second[slotId]->_timeInMinutes << '\n';
        }
        it ++;
    }
    return cout;
}


/******************************
 accessor functions
 ******************************/

// fetch future data for "load"
// at time stamps in timeArrayInMinutes
vector<LoadData *> FutureData::fetchFutureDataForLoad(const string &loadName,
                                                      const LoadType &loadType,
                                                      const vector<time_type> &timeArrayInMinutes) {
    vector<LoadData *> result;
    for (int timeId = 0; timeId < timeArrayInMinutes.size(); timeId ++) {
        LoadData *loadData = fetchFutureDataForLoad(loadName,
                                                    loadType,
                                                    timeArrayInMinutes[timeId]);
        result.push_back(loadData);
    }
    return result;
}

// fetch future data for "load" at time "time"
LoadData *FutureData::fetchFutureDataForLoad(const string &loadName,
                                             const LoadType &loadType,
                                             const time_type &timeInMinutes) {
    // must have future data
    if (_loadNameToFutureData[loadName].empty()) {
        std::cout << "cannot do interpolation!" << std::endl;
        return NULL;
    }
    
    // timeInMinutes must be in the future
    vector<LoadData *>::iterator it = _loadNameToFutureData[loadName].begin();
    if ( (*it)->_timeInMinutes > timeInMinutes ) {
        std::cout << "cannot do interpolation!" << std::endl;
        return NULL;
    }
    
    // if match the first entry
    if ( (*it)->_timeInMinutes == timeInMinutes ) {
        if (loadType == BASELOAD) {
            BaseLoadData *loadData = (BaseLoadData *)(*it);
            BaseLoadData *result = new BaseLoadData(*loadData);
            return result;
        }
        else if (loadType == PHOTOVOLTAIC) {
            PhotoVoltaicData *loadData = (PhotoVoltaicData *)(*it);
            PhotoVoltaicData *result = new PhotoVoltaicData(*loadData);
            return result;
        }
        else
            return NULL;
    }
    
    // timeInMinutes must be within the maximum prediction window
    if ( it == _loadNameToFutureData[loadName].end() ) {
        std::cout << "cannot do interpolation!" << std::endl;
        return NULL;
    }
    
    // find it and nextIt such that timeInMinutes is in between
    vector<LoadData *>::iterator nextIt = it + 1;
    while ( nextIt != _loadNameToFutureData[loadName].end() &&
           (*nextIt)->_timeInMinutes < timeInMinutes ) {
        it ++;
        nextIt ++;
    }
    
    // nextIt must be found
    if ( nextIt == _loadNameToFutureData[loadName].end() ) {
        std::cout << "cannot do interpolation!" << std::endl;
        return NULL;
    }
    
    if (loadType == BASELOAD) {
        BaseLoadData *loadData = (BaseLoadData *)(*it);
        return loadData->interpolate(*nextIt, timeInMinutes);
    }
    else if (loadType == PHOTOVOLTAIC) {
        PhotoVoltaicData *loadData = (PhotoVoltaicData *)(*it);
        return loadData->interpolate(*nextIt, timeInMinutes);
    }
    else
        return NULL;
}

// add an array of future data for a load
void FutureData::addFutureDataForLoad(const string &name,
                                      const vector<LoadData *> &loadDataArray) {
    _loadNameToFutureData[name].insert(_loadNameToFutureData[name].end(),
                                       loadDataArray.begin(),
                                       loadDataArray.end());
}

// add a future data for a load
void FutureData::addFutureLoadForLoad(const string &name,
                                      LoadData *loadData) {
    _loadNameToFutureData[name].push_back(loadData);
}


/******************************
 other functions
 ******************************/

// release future data for a load till certain time
void FutureData::releaseFutureDataForLoadTillTime(const string &name,
                                                  const time_type &endTimeInMinutes) {
    if (_loadNameToFutureData[name].size() >= 2) {
        vector<LoadData *>::iterator it = _loadNameToFutureData[name].begin();
        while ((*(it+1))->_timeInMinutes <= endTimeInMinutes) {
            delete (*it);
            it = _loadNameToFutureData[name].erase(it);
        }
    }
}
