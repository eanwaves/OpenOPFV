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
 * Module NetworkModel.cpp
 *
 ***********************************************************************/

#include "NetworkModel.h"
#include "FutureData.h"

/******************************
 basic functions
 ******************************/

// default constructor
NetworkModel::NetworkModel() {
    _substationVoltage = 1.0;
}

// copy constructor
NetworkModel::NetworkModel(const NetworkModel &model) :
_buses(model._buses),
_lines(model._lines),
_busNameToPointerHashTable(model._busNameToPointerHashTable) {
    _substationVoltage = model._substationVoltage;
}

// release allocated memory
void NetworkModel::clear() {
    // release buses and loads on the buses
    for (int busId = 0; busId < numberOfBuses(); busId ++) {
        delete _buses[busId];
    }
    _buses.clear();
    
    // release lines
    for (int lineId = 0; lineId < numberOfLines(); lineId ++) {
        delete _lines[lineId];
    }
    _lines.clear();
    
    _busNameToPointerHashTable.clear();
}

// deconstructor
NetworkModel::~NetworkModel() {
    clear();
}

// assignment
void NetworkModel::operator=(const NetworkModel &model) {
    _buses = model._buses;
    _lines = model._lines;
    _busNameToPointerHashTable = model._busNameToPointerHashTable;
    _substationVoltage = model._substationVoltage;
}

// print
ostream &operator<<(ostream &cout, const NetworkModel &model) {
    // print bus information
    for (int busId = 0; busId < model.numberOfBuses(); busId ++) {
        Bus *bus = model.getBusByIndex(busId);
        cout << "bus " << bus->name() << " (" << bus->phase() << "):\n";
        // from line
        if (bus->type() == SUBSTATION) {
            cout << "\tis substation\n";
        }
        else {
            cout << "\tfrom line " << bus->fromLine()->name();
        }
        // to lines
        cout << "\tto lines";
        for (int lineId = 0; lineId < bus->toLineArray().size(); lineId ++) {
            cout << " " << bus->toLineArray()[lineId]->name();
        }
        cout << "\n";
        // loads
        cout << "\thas loads";
        for (int loadId = 0; loadId < bus->loadArray().size(); loadId ++) {
            cout << ' ' << bus->loadArray()[loadId]->name();
        }
        cout << "\n";
    }
    
    // print line information
    for (int lineId = 0; lineId < model.numberOfLines(); lineId ++) {
        Line *line = model.getLineByIndex(lineId);
        cout << "line " << line->name() << " (" << line->phase() << "):\n";
        // from bus
        cout << "\tfrom bus " << line->fromBus()->name();
        cout << " to bus " << line->toBus()->name() << "\n";
    }
    
    return cout;
}

void NetworkModel::printPowerFlow(const string &mode) {
    ofstream file;
    if (mode.compare("append") == 0)
        file.open("/Users/ganlingwen/Desktop/powerFlowFromModel.txt", std::ofstream::app);
    else
        file.open("/Users/ganlingwen/Desktop/powerFlowFromModel.txt");
    
    for (int lineId = 0; lineId < numberOfLines(); lineId ++) {
        Line *line = _lines[lineId];
        file << "line (" << line->name() << ", " << line->phase() << ")\n";
        Bus *fromBus = line->fromBus();
        Bus *toBus = line->toBus();
        file << "\tfrom bus (" << fromBus->name() << ", " << fromBus->phase() << ")\n";
        file << "\tto bus (" << toBus->name() << ", " << toBus->phase() << ")\n";
        file << "\tvoltage at to bus is " << toBus->voltage() << '\n';
        file << "\tcurrent is " << line->current() << '\n';
        file << "\timpedance is " << line->impedance() << '\n';
        ColumnVector<complex_type> voltage = toBus->voltage() + line->impedance() * line->current();
        file << "\tvoltage at from bus is        " << fromBus->voltage() << '\n';
        file << "\tvoltage at from bus should be " << voltage << '\n';
    }
    
    for (int busId = 1; busId < numberOfBuses(); busId ++) {
        Bus *bus = _buses[busId];
        ColumnVector<complex_type> current(bus->phase());
        vector<Line *> toLines = bus->toLineArray();
        for (int lineId = 0; lineId < toLines.size(); lineId ++) {
            Line *line = toLines[lineId];
            current.addToIndices(line->current(), line->phaseIndicesInFromBus());
        }
        current += bus->aggregateLoad()._admittance * bus->voltage();
        current = current - bus->fromLine()->current();
        file << "bus (" << bus->name() << ", " << bus->phase() << ")\n";
        file << "\tvoltage is " << bus->voltage() << '\n';
        file << "\tcurrent is " << current << '\n';
        ColumnVector<complex_type> power(bus->phase());
        for (int i = 0; i < power.size(); i ++) {
            power[i] = -bus->voltage()[i] * std::conj(current[i]);
        }
        file << "\tpower is    " << bus->aggregateLoad()._power << "\n";
        file << "\tit should be" << power << '\n';
    }
    
    file.close();
}


/******************************
 accessor functions
 ******************************/

// getter functions
int NetworkModel::numberOfBuses() const {
    return int( _buses.size() );
}

Bus *NetworkModel::getBusByIndex(const int &index) const {
    return _buses[index];
}

Bus *NetworkModel::getBusByName(const string &busName) {
    return _busNameToPointerHashTable[busName];
}

int NetworkModel::numberOfLines() const {
    return int( _lines.size() );
}

Line *NetworkModel::getLineByIndex(const int &index) const {
    return _lines[index];
}

vector<Line *> NetworkModel::lines() const {
    return _lines;
}

// setter functions
void NetworkModel::addABus(Bus *bus) {
    _buses.push_back(bus);
    if (_busNameToPointerHashTable.find(bus->name()) != _busNameToPointerHashTable.end() )
        std::cout << "Duplicate bus names exist!" << std::endl;
    else
        _busNameToPointerHashTable[bus->name()] = bus;
}

void NetworkModel::addALine(Line *line) {
    _lines.push_back(line);
}

void NetworkModel::addALoad(Load *load) {
    if (load->locationBus() == NULL) {
        std::cout << "Must specify where a load locates!" << std::endl;
        return;
    }
    
    load->locationBus()->addALoad(load);
}

void NetworkModel::deleteALoad(Load *load) {
    load->locationBus()->deleteALoad(load);
    delete load;
}


/******************************
 event execution
 ******************************/

// fetch real data from future data when a new timestamp comes
void NetworkModel::fetchRealTimeData(const time_type &oldTimeInMinutes,
                                     const time_type &newTimeInMinutes,
                                     FutureData *futureData) {
    for (int busId = 0; busId < numberOfBuses(); busId ++)
        _buses[busId]->fetchRealTimeData(oldTimeInMinutes, newTimeInMinutes, futureData);
}


/******************************
 power flow computation
 ******************************/

// sort _buses and _lines according to breadth first search
// must be called before performing "backward-forward sweep"
void NetworkModel::sortBusAndLineByBreadthFirstSearch() {
    // declare new orders
    vector<Bus *> buses;
    vector<Line *> lines;
    
    // find the root --- substation
    deque<Bus *> busToVisit;
    vector<Bus *>::iterator it = _buses.begin();
    while (it != _buses.end()) {
        if ((*it)->type() == SUBSTATION) {
            buses.push_back(*it);
            busToVisit.push_back(*it);
            it = _buses.erase(it);
        }
        else {
            it ++;
        }
    }
    if (buses.size() != 1) {
        std::cout << "Too many or 0 substations found!" << std::endl;
        exit(1);
    }
    
    // add a bus and line till BFS is finished
    while (!busToVisit.empty()) {
        
        // get the current visiting bus
        Bus *bus = busToVisit.front();
        busToVisit.pop_front();
        
        // find if there are other buses connected to "bus"
        vector<Line *>::iterator it = _lines.begin();
        while (it != _lines.end()) {
            
            // if a line connects from "bus"
            if ((*it)->fromBus() == bus) {
                // add a bus and line to sorted array
                lines.push_back(*it);
                Bus *nextBus = (*it)->toBus();
                buses.push_back(nextBus);
                // remove counterpart in original copy
                it = _lines.erase(it);
                vector<Bus *>::iterator busIt = _buses.begin();
                while (busIt != _buses.end()) {
                    if ((*busIt) == nextBus) {
                        busIt = _buses.erase(busIt);
                    }
                    else {
                        busIt ++;
                    }
                }
                // update to visit queue
                busToVisit.push_back(nextBus);
            }
            
            // if a line connects to "bus"
            else if ((*it)->toBus() == bus) {
                // add a bus and line to sorted array
                Bus *nextBus = (*it)->fromBus();
                (*it)->setToBus(nextBus);
                (*it)->setFromBus(bus);
                lines.push_back(*it);
                buses.push_back(nextBus);
                // remove counterpart in origianl copy
                it = _lines.erase(it);
                vector<Bus *>::iterator busIt = _buses.begin();
                while (busIt != _buses.end()) {
                    if ((*busIt) == nextBus) {
                        busIt = _buses.erase(busIt);
                    }
                    else {
                        busIt ++;
                    }
                }
                // update to visit queue
                busToVisit.push_back(nextBus);
            }
            
            // otherwise irrelevant
            else {
                it ++;
            }
        }
    }
    
    // update original _buses and _lines
    if (!_buses.empty() || !_lines.empty()) {
        std::cout << "Cannot do BFS correctly!" << std::endl;
        exit(1);
    }
    _buses = buses;
    _lines = lines;
    
    // change phaseIn***Bus
    for (int lineId = 0; lineId < numberOfLines(); lineId ++) {
        Line *line = _lines[lineId];
        line->initPhaseIndicesInFromBus();
        line->fromBus()->addAToLine(line);
        line->toBus()->setFromLine(line);
        line->toBus()->initPhaseIndicesInParentBus();
    }
}

// initialize the voltages
// must be called before performing computePowerFlowWithSimulator
// and after performing sortBusAndLineByBreadthFirstSearch
void NetworkModel::initVoltage() {
    // initialize the substation voltage
    Bus *substation = _buses[0];
    string phase = substation->phase();
    ColumnVector<complex_type> voltage(phase);
    for (int phaseId = 0; phaseId < phase.size(); phaseId ++) {
        double angle = - M_PI * 2 / 3 * (phase[phaseId] - 'a');
        voltage[phaseId] = _substationVoltage * complex_type(cos(angle), sin(angle));
    }
    substation->setVoltage(voltage);
    
    // initialize the voltage at other buses
    for (int busId = 1; busId < numberOfBuses(); busId ++) {
        _buses[busId]->initVoltage();
    }
}

// compute power flow with internal model
void NetworkModel::computePowerFlowWithSimulator(const int &maxIteration,
                                                 const double &updateSizeThreshold) {
    // initialize voltage if necessary
    if (_buses[0]->voltage()[0].real() < 0.5)
        initVoltage();
    
    // compute total load on non-substation buses
    for (int busId = 1; busId < numberOfBuses(); busId ++) {
        _buses[busId]->computeAggregateLoadOnSelf();
    }
    
    // doing backward-forward sweep until maxIteration hit or updateSize small
    int iteration = 0;
    double updateSize = updateSizeThreshold + 1.0;
    while (iteration < maxIteration && updateSize >= updateSizeThreshold) {
        updateSize = 0.0;
        // backward sweep to update currents
        for (int busId = int(_buses.size() - 1); busId > 0; busId --) {
            double updateSizeTmp = _buses[busId]->computeCurrentOnFromLine();
            if (updateSizeTmp > updateSize)
                updateSize = updateSizeTmp;
        }
        // forward sweep to update voltages
        for (int busId = 1; busId < _buses.size(); busId ++) {
            double updateSizeTmp = _buses[busId]->computeVoltageOnSelf();
            if (updateSizeTmp > updateSize)
                updateSize = updateSizeTmp;
        }
        iteration ++;
        
        // comment out this section if do not need to check power flow computation
        /*
        std::cout << "iteration " << iteration <<", updateSize = " << updateSize << std::endl;
        if (iteration == 1)
            printPowerFlow("overwrite");
        else
            printPowerFlow("append");
         */
    }
    
    // compute substation power injection
    Bus *substation = _buses[0];
    ColumnVector<complex_type> voltage = substation->voltage();
    
    ColumnVector<complex_type> current = ColumnVector<complex_type>(substation->phase());
    current.reset();
    vector<Line *> toLines = substation->toLineArray();
    for (int lineId = 0; lineId < toLines.size(); lineId ++) {
        Line *line = toLines[lineId];
        current.addToIndices(line->current(), line->phaseIndicesInFromBus());
    }
    
    ColumnVector<complex_type> power(substation->phase());
    for (int phaseId = 0; phaseId < substation->phase().size(); phaseId ++) {
        power[phaseId] = voltage[phaseId] * (std::conj(current[phaseId]));
    }
    
    LoadValue loadValue = substation->aggregateLoad();
    loadValue._power = power;
    substation->setAggregateLoad(loadValue);
}

// compute power flow with GridLabD
void NetworkModel::computePowerFlowWithGridLabD() {
    computePowerFlowWithSimulator();
}
