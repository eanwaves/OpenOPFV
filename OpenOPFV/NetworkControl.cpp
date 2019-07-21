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
 * Module NetworkControl.cpp
 *
 ***********************************************************************/

#include "NetworkControl.h"
#include "PhotoVoltaicController.h"
#include "ElectricVehicleController.h"

/******************************
 basic functions
 ******************************/

// default constructor
NetworkControl::NetworkControl() {
    _substationVoltage = 1.0;
    _quadCoef = 1.0;
    _linCoef = 0.0;
}

// copy constructor
NetworkControl::NetworkControl(const NetworkControl &control) :
_buses(control._buses),
_lines(control._lines),
_busToControllerHashTable(control._busToControllerHashTable),
_enabledInFastControl(control._enabledInFastControl),
_enabledInSlowControl(control._enabledInSlowControl),
_numberOfSlots(control._numberOfSlots),
_slotLengthInMinutes(control._slotLengthInMinutes),
_quadCoef(control._quadCoef),
_linCoef(control._linCoef),
_busPhaseIndicesInRoot(control._busPhaseIndicesInRoot),
_muLower(control._muLower),
_muUpper(control._muUpper),
_stepSize(control._stepSize),
_oldObjectiveValue(control._oldObjectiveValue),
_substationVoltage(control._substationVoltage) {
}

// clear allocated spaces
void NetworkControl::clear() {
    for (int busId = 0; busId < _buses.size(); busId ++)
        delete _buses[busId];
    _buses.clear();
    
    for (int lineId = 0; lineId < _lines.size(); lineId ++)
        delete _lines[lineId];
    _lines.clear();
    
    _busToControllerHashTable.clear();
    
    _enabledInFastControl.clear();
    _enabledInSlowControl.clear();
    
    _busPhaseIndicesInRoot.clear();
}

// deconstructor
NetworkControl::~NetworkControl() {
    clear();
}

// assignment
void NetworkControl::operator=(const NetworkControl &control) {
    _buses = control._buses;
    _lines = control._lines;
    _busToControllerHashTable = control._busToControllerHashTable;
    _enabledInFastControl = control._enabledInFastControl;
    _enabledInSlowControl = control._enabledInSlowControl;
    _numberOfSlots = control._numberOfSlots;
    _slotLengthInMinutes = control._slotLengthInMinutes;
    _quadCoef = control._quadCoef;
    _linCoef = control._linCoef;
    _busPhaseIndicesInRoot = control._busPhaseIndicesInRoot;
    _muLower = control._muLower;
    _muUpper = control._muUpper;
    _stepSize = control._stepSize;
    _oldObjectiveValue = control._oldObjectiveValue;
    _substationVoltage = control._substationVoltage;
}

// print
ostream &operator<<(ostream &cout, const NetworkControl &control) {
    // print bus information
    for (int busId = 0; busId < control._buses.size(); busId ++) {
        BusController *busController = control._buses[busId];
        Bus *bus = busController->_bus;
        cout << "bus " << bus->name() << " (" << bus->phase() << "):\n";
        // from line
        if (bus->type() == SUBSTATION) {
            cout << "\tis substation\n";
        }
        else {
            cout << "\tfrom line " << busController->_fromLine->_line->name();
        }
        // to lines
        cout << "\tto lines";
        for (int lineId = 0; lineId < busController->_toLineArray.size(); lineId ++) {
            cout << " " << busController->_toLineArray[lineId]->_line->name();
        }
        cout << "\n";
        // loads
        cout << "\thas loads";
        for (int loadId = 0; loadId < busController->_loadArray.size(); loadId ++) {
            cout << ' ' << busController->_loadArray[loadId]->_load->name();
        }
        cout << "\n";
    }
    
    // print line information
    for (int lineId = 0; lineId < control._lines.size(); lineId ++) {
        LineController *lineController = control._lines[lineId];
        Line *line = lineController->_line;
        cout << "line " << line->name() << " (" << line->phase() << "):\n";
        // from bus
        cout << "\tfrom bus " << lineController->_fromBus->_bus->name();
        cout << " to bus " << lineController->_toBus->_bus->name() << "\n";
    }
    
    return cout;
}

void NetworkControl::printPowerFlow(int timeSlotId, const string &mode) {
    ofstream file;
    if (mode.compare("append") == 0)
        file.open("/Users/ganlingwen/Desktop/powerFlowFromControl.txt", std::ofstream::app);
    else
        file.open("/Users/ganlingwen/Desktop/powerFlowFromControl.txt");
    
    for (int lineId = 0; lineId < _lines.size(); lineId ++) {
        LineController *lineController = _lines[lineId];
        Line *line = lineController->_line;
        file << "line (" << line->name() << ", " << line->phase() << ")\n";
        BusController *fromBusController = lineController->_fromBus;
        Bus *fromBus = fromBusController->_bus;
        BusController *toBusController = lineController->_toBus;
        Bus *toBus = toBusController->_bus;
        file << "\tfrom bus (" << fromBus->name() << ", " << fromBus->phase() << ")\n";
        file << "\tto bus (" << toBus->name() << ", " << toBus->phase() << ")\n";
        file << "\tvoltage at to bus is " << toBusController->_voltages[timeSlotId] << '\n';
        file << "\tcurrent is " << lineController->_currentArray[timeSlotId] << '\n';
        file << "\timpedance is " << lineController->_impedance << '\n';
        ColumnVector<complex_type> voltage = toBusController->_voltages[timeSlotId] + lineController->_impedance * lineController->_currentArray[timeSlotId];
        file << "\tvoltage at from bus is        " << fromBusController->_voltages[timeSlotId] << '\n';
        file << "\tvoltage at from bus should be " << voltage << '\n';
    }
    
    for (int busId = 1; busId < _buses.size(); busId ++) {
        BusController *bus = _buses[busId];
        ColumnVector<complex_type> current(bus->_bus->phase());
        vector<LineController *> toLines = bus->_toLineArray;
        for (int lineId = 0; lineId < toLines.size(); lineId ++) {
            LineController *line = toLines[lineId];
            current.addToIndices(line->_currentArray[timeSlotId], line->_phaseIndicesInFromBus);
        }
        current += bus->_aggregateLoads[timeSlotId]._admittance * bus->_voltages[timeSlotId];
        current = current - bus->_fromLine->_currentArray[timeSlotId];
        file << "bus (" << bus->_bus->name() << ", " << bus->_bus->phase() << ")\n";
        file << "\tvoltage is " << bus->_voltages[timeSlotId] << '\n';
        file << "\tcurrent is " << current << '\n';
        ColumnVector<complex_type> power(bus->_bus->phase());
        for (int i = 0; i < power.size(); i ++) {
            power[i] = -bus->_voltages[timeSlotId][i] * std::conj(current[i]);
        }
        file << "\tpower is    " << bus->_aggregateLoads[timeSlotId]._power << "\n";
        file << "\tit should be" << power << '\n';
    }
    
    file.close();
}

void NetworkControl::printGradient(int timeSlotId, ostream &cout) {
    for (int busId = 0; busId < _buses.size(); busId ++) {
        BusController *bus = _buses[busId];
        cout << "bus " << bus->_bus->name() << " (" << bus->_bus->phase() << ")\n";
        cout << "\tgradient = " << bus->_gradient[timeSlotId] << '\n';
    }
}

void NetworkControl::printGradientOverHorizon(ostream &cout) {
    for (int busId = 0; busId < _buses.size(); busId ++) {
        BusController *bus = _buses[busId];
        cout << "\tbus " << bus->_bus->name() << " (" << bus->_bus->phase() << ")\n";
        for (int timeSlotId = 0; timeSlotId < bus->_aggregateLoads.size(); timeSlotId ++) {
            cout << "\tgradient[" << timeSlotId << "] = " << bus->_gradient[timeSlotId] << '\n';
        }
    }
}

void NetworkControl::printPowerConsumption(int timeSlotId, ostream &cout) {
    for (int busId = 0; busId < _buses.size(); busId ++) {
        BusController *bus = _buses[busId];
        for (int loadId = 0; loadId < bus->_loadArray.size(); loadId ++) {
            LoadController *load = bus->_loadArray[loadId];
            if (load->_load->type() == PHOTOVOLTAIC) {
                cout << "\tload " << load->_load->name() << " (" << load->_load->phase() << ")\n";
                ColumnVector<complex_type> power = load->_valueArray[timeSlotId]._power;
                cout << "\t\tpower consumption = " << power << ", norm = " << norm(power) << '\n';
            }
        }
    }
}

struct EVControlData {
    string _name;
    vector<double> _realPower;
};

void NetworkControl::printSlowControlResult(ostream &cout) {
    vector<EVControlData> evInPhaseA, evInPhaseB, evInPhaseC;
    for (int busId = 0; busId < _buses.size(); busId ++) {
        BusController *bus = _buses[busId];
        for (int loadId = 0; loadId < bus->_loadArray.size(); loadId ++) {
            LoadController *load = bus->_loadArray[loadId];
            if (load->_load->type() != ELECTRIC_VEHICLE)
                continue;
            EVControlData data;
            data._name = load->_load->name();
            for (int timeSlotId = 0; timeSlotId < load->_valueArray.size(); timeSlotId ++)
                data._realPower.push_back(load->_valueArray[timeSlotId]._power[0].real());
            switch (load->_load->phase()[0]) {
                case 'a':
                    evInPhaseA.push_back(data);
                    break;
                case 'b':
                    evInPhaseB.push_back(data);
                    break;
                case 'c':
                    evInPhaseC.push_back(data);
                    break;
                default:
                    break;
            }
        }
    }
    
    cout << '\t' << "phase A:\n";
    cout << "\t\tsubstation";
    for (int evIndex = 0; evIndex < evInPhaseA.size(); evIndex ++) {
        EVControlData data = evInPhaseA[evIndex];
        cout << "\t" << data._name;
        cout << '\n';
    }
    for (int timeSlotId = 0; timeSlotId < _buses[0]->_aggregateLoads.size(); timeSlotId ++) {
        cout << "\t\t" << _buses[0]->_aggregateLoads[timeSlotId]._power[0].real();
        for (int evIndex = 0; evIndex < evInPhaseA.size(); evIndex ++)
            cout << '\t' << evInPhaseA[evIndex]._realPower[timeSlotId];
        cout << '\n';
    }
    
    cout << '\t' << "phase B:\n";
    cout << "\t\tsubstation";
    for (int evIndex = 0; evIndex < evInPhaseB.size(); evIndex ++) {
        EVControlData data = evInPhaseB[evIndex];
        cout << "\t" << data._name;
        cout << '\n';
    }
    for (int timeSlotId = 0; timeSlotId < _buses[0]->_aggregateLoads.size(); timeSlotId ++) {
        cout << "\t\t" << _buses[0]->_aggregateLoads[timeSlotId]._power[1].real();
        for (int evIndex = 0; evIndex < evInPhaseB.size(); evIndex ++)
            cout << '\t' << evInPhaseB[evIndex]._realPower[timeSlotId];
        cout << '\n';
    }
    
    cout << '\t' << "phase C:\n";
    cout << "\t\tsubstation";
    for (int evIndex = 0; evIndex < evInPhaseC.size(); evIndex ++) {
        EVControlData data = evInPhaseC[evIndex];
        cout << "\t" << data._name;
        cout << '\n';
    }
    for (int timeSlotId = 0; timeSlotId < _buses[0]->_aggregateLoads.size(); timeSlotId ++) {
        cout << "\t\t" << _buses[0]->_aggregateLoads[timeSlotId]._power[2].real();
        for (int evIndex = 0; evIndex < evInPhaseC.size(); evIndex ++)
            cout << '\t' << evInPhaseC[evIndex]._realPower[timeSlotId];
        cout << '\n';
    }
}

void NetworkControl::printViolatedVoltages(ostream &cout) {
    for (int busId = 0; busId < _buses.size(); busId ++) {
        if (! _buses[busId]->_hasVoltageConstraint)
            continue;
        BusController *bus = _buses[busId];
        for (int phaseId = 0; phaseId < bus->_bus->phase().size(); phaseId ++) {
            bool voltageViolation = false;
            for (int timeSlotId = 0; timeSlotId < bus->_voltages.size(); timeSlotId ++) {
                complex_type voltage = bus->_voltages[timeSlotId][phaseId];
                double voltageMagnitude = std::sqrt(std::norm(voltage));
                if (voltageMagnitude > 1.05 ||
                    voltageMagnitude < 0.95)
                    voltageViolation = true;
            }
            if ( voltageViolation ) {
                std::cout << "\tbus " << bus->_bus->name();
                std::cout << ", phase " << bus->_bus->phase()[phaseId] << '\n';
                for (int timeSlotId = 0; timeSlotId < bus->_voltages.size(); timeSlotId ++) {
                    complex_type voltage = bus->_voltages[timeSlotId][phaseId];
                    double voltageMagnitude = std::sqrt(std::norm(voltage));
                    std::cout << "\t\ttime " << timeSlotId;
                    std::cout << ", voltage " << voltageMagnitude;
                    std::cout << ", gradient " << bus->_gradient[timeSlotId][phaseId].real() << '\n';
                }
            }
        }
    }
}


/******************************
 initializer
 ******************************/

// set numberOfSlotsInSlowControl
// must be called before running initalize(model)
void NetworkControl::setNumberOfSlots(int numberOfSlots) {
    _numberOfSlots = numberOfSlots;
}

// set substation voltage
void NetworkControl::setSubstationVoltage(const double &substationVoltage) {
    _substationVoltage = substationVoltage;
}

// initialize networkControl according to networkModel
void NetworkControl::initialize(const NetworkModel &model) {
    // set up network description
    _substationVoltage = model._substationVoltage;
    for (int busId = 0; busId < model.numberOfBuses(); busId ++) {
        addABus( model.getBusByIndex(busId) );
    }
    
    for (int lineId = 0; lineId < model.numberOfLines(); lineId ++) {
        addALine( model.getLineByIndex(lineId) );
    }
    
    // set up algorithm parameters
    _quadCoef = 1.0;
    _linCoef = 0.0;
    string phase = _buses[0]->_bus->phase();
    string rootPhase = _buses[0]->_bus->phase();
    for (int busId = 0; busId < _buses.size(); busId ++) {
        string busPhase = _buses[busId]->_bus->phase();
        vector<int> phaseLoc;
        int phaseIdAtRoot = 0;
        for (int phaseIdAtBus = 0; phaseIdAtBus < busPhase.size(); phaseIdAtBus ++) {
            while (phaseIdAtRoot < rootPhase.size() &&
                   rootPhase[phaseIdAtRoot] != busPhase[phaseIdAtBus] ) {
                phaseIdAtRoot ++;
            }
            if (phaseIdAtRoot == rootPhase.size()) {
                std::cout << "Bus phases must be inside root phases!" << std::endl;
            }
            else {
                phaseLoc.push_back(phaseIdAtRoot);
                phaseIdAtRoot ++;
            }
        }
        _busPhaseIndicesInRoot.push_back(phaseLoc);
    }
}

// add a bus
void NetworkControl::addABus(Bus *bus) {
    if (bus->type() == SUBSTATION ||
        bus->type() == HOUSE ||
        bus->type() == BUS) {
        BusController *control = new BusController(bus, _numberOfSlots);
        _buses.push_back(control);
        _busToControllerHashTable[bus] = control;
        
        vector<Load *> loads = bus->loadArray();
        for (int loadId = 0; loadId < loads.size(); loadId ++)
            addALoad(loads[loadId]);
    }
    
    else
        std::cout << "Unexpected bus type when constructing networkControl!" << std::endl;
}

// add a line
void NetworkControl::addALine(Line *line) {
    if (line->type() == LINE) {
        LineController *control = new LineController(line, _numberOfSlots);
        _lines.push_back(control);
        
        BusController *fromBus = _busToControllerHashTable[line->fromBus()];
        control->_fromBus = fromBus;
        fromBus->_toLineArray.push_back(control);
        
        BusController *toBus = _busToControllerHashTable[line->toBus()];
        control->_toBus = toBus;
        toBus->_fromLine = control;
    }
    
    else
        std::cout << "Unexpected line type when constructing networkControl!" << std::endl;
}

// add a load
void NetworkControl::addALoad(Load *load) {
    LoadController *control = NULL;
    if (load->type() == BASELOAD) {
        control = new LoadController(load, _numberOfSlots);
    }
    else if (load->type() == PHOTOVOLTAIC) {
        control = new PhotoVoltaicController((PhotoVoltaic *)load, _numberOfSlots);
    }
    else if (load->type() == ELECTRIC_VEHICLE) {
        control = new ElectricVehicleController((ElectricVehicle *)load, _numberOfSlots, _slotLengthInMinutes);
    }
    else {
        std::cout << "Unexpected load type when constructing networkControl!" << std::endl;
    }
    
    BusController *bus = _busToControllerHashTable[load->locationBus()];
    control->_locationBus = bus;
    bus->addALoad(control);
}

// delete a load
void NetworkControl::deleteALoad(Load *load) {
    BusController *bus = _busToControllerHashTable[load->locationBus()];
    bus->deleteALoad(load);
}


/******************************
 event execution
 ******************************/

// do fast control
void NetworkControl::fastControl() {
    fastControlInitialize();
    fastControlOuterLoop();
    applyControl();
}

// do slow control
void NetworkControl::slowControl(time_type time) {
    slowControlInitialize(time);
    slowControlOuterLoop();
    applyControl();
    // printSlowControlResult(std::cout);
}

// apply control result
void NetworkControl::applyControl() {
    for (int busId = 0; busId < _buses.size(); busId ++) {
        BusController *bus = _buses[busId];
        for (int loadId = 0; loadId < bus->_loadArray.size(); loadId ++) {
            LoadController *load = bus->_loadArray[loadId];
            load->_load->setValue( load->_valueArray[0] );
        }
        _buses[busId]->_bus->setVoltage(_buses[busId]->_voltages[0]);
    }
}


/******************************
 power flow computation
 ******************************/

// initialize the voltages
// must be called before performing computePowerFlow
void NetworkControl::initVoltageAtTime(int timeSlotId) {
    // initialize substation voltage
    BusController *substation = _buses[0];
    string phase = substation->_bus->phase();
    ColumnVector<complex_type> voltage(phase);
    for (int phaseId = 0; phaseId < phase.size(); phaseId ++) {
        double angle = - M_PI * 2 / 3 * (phase[phaseId] - 'a');
        voltage[phaseId] = _substationVoltage * complex_type(cos(angle), sin(angle));
    }
    substation->_voltages[timeSlotId] = voltage;
    
    // initialize the voltage at other buses
    for (int busId = 1; busId < _buses.size(); busId ++) {
        _buses[busId]->initVoltageAtTime(timeSlotId);
    }
}

void NetworkControl::initVoltageOverHorizon() {
    for (int timeSlotId = 0; timeSlotId < _numberOfSlots; timeSlotId ++) {
        initVoltageAtTime(timeSlotId);
    }
}

// compute power flow with internal model
void NetworkControl::computePowerFlowAtTime(int timeSlotId,
                                            const int &maxIteration,
                                            const double &updateSizeThreshold) {
    // initialize voltage if necessary
    if (_buses[0]->_voltages[timeSlotId][0].real() < 0.5)
        initVoltageAtTime(timeSlotId);
    
    // compute total load on non-substation buses
    for (int busId = 1; busId < _buses.size(); busId ++) {
        _buses[busId]->computeAggregateLoadOnSelfAtTime(timeSlotId);
    }
    
    // doing backward-forward sweep until maxIteration hit or updateSize small
    int iteration = 0;
    double updateSize = updateSizeThreshold + 1.0;
    while (iteration < maxIteration && updateSize >= updateSizeThreshold) {
        updateSize = 0.0;
        // backward sweep to update currents
        for (int busId = int( _buses.size() - 1 ); busId > 0; busId --) {
            double updateSizeTmp = _buses[busId]->computeCurrentOnFromLineAtTime(timeSlotId);
            if (updateSizeTmp > updateSize)
                updateSize = updateSizeTmp;
        }
        // forward sweep to update voltages
        for (int busId = 1; busId < _buses.size(); busId ++) {
            double updateSizeTmp = _buses[busId]->computeVoltageOnSelfAtTime(timeSlotId);
            if (updateSizeTmp > updateSize)
                updateSize = updateSizeTmp;
        }
        iteration ++;
        
        // comment out this section if do not need to check power flow computation
        /*
         std::cout << "iteration " << iteration <<", updateSize = " << updateSize << std::endl;
         if (iteration == 1)
         printPowerFlow(timeSlotId, "overwrite");
         else
         printPowerFlow(timeSlotId, "append");
         */
    }
    
    // compute substation power injection
    BusController *substation = _buses[0];
    string phase = substation->_bus->phase();
    ColumnVector<complex_type> voltage = substation->_voltages[timeSlotId];
    
    ColumnVector<complex_type> current(phase);
    current.reset();
    vector<LineController *> toLines = substation->_toLineArray;
    for (int lineId = 0; lineId < toLines.size(); lineId ++) {
        LineController *line = toLines[lineId];
        current.addToIndices(line->_currentArray[timeSlotId], line->_phaseIndicesInFromBus);
    }
    
    ColumnVector<complex_type> power(phase);
    for (int phaseId = 0; phaseId < phase.size(); phaseId ++) {
        power[phaseId] = voltage[phaseId] * ( std::conj(current[phaseId]) );
    }
    
    LoadValue loadValue = substation->_aggregateLoads[timeSlotId];
    loadValue._power = power;
    substation->_aggregateLoads[timeSlotId] = loadValue;
}

void NetworkControl::computePowerFlowOverHorizon(const int &maxIteration,
                                                 const double &updateSizeThreshold) {
    for (int timeSlotId = 0; timeSlotId < _numberOfSlots; timeSlotId ++) {
        computePowerFlowAtTime(timeSlotId, maxIteration, updateSizeThreshold);
    }
}


/******************************
 gradient estimation
 ******************************/

// compute gradients
void NetworkControl::computeGradientAtTime(int timeSlotId) {
    int numberOfBus = int( _buses.size() );
    
    // compute marginal price
    int numberOfPhasesAtRoot = int( _buses[0]->_bus->phase().size() );
    vector<double> marginalPrice(numberOfPhasesAtRoot, 0.0);
    for (int phaseId = 0; phaseId < numberOfPhasesAtRoot; phaseId ++) {
        marginalPrice[phaseId] = 2 * _quadCoef * _buses[0]->_aggregateLoads[timeSlotId]._power[phaseId].real() + _linCoef;
    }
    
    // backward sweep to compute sumDown
    for (int busId = numberOfBus - 1; busId > 0; busId --) {
        BusController *bus = _buses[busId];
        int numberOfPhasesAtBus = int( bus->_phaseIndicesInParentBus.size() );
        vector<double> price(numberOfPhasesAtBus, 0.0);
        for (int phaseId = 0; phaseId < numberOfPhasesAtBus; phaseId ++) {
            price[phaseId] = marginalPrice[_busPhaseIndicesInRoot[busId][phaseId]];
        }
        _buses[busId]->computeSumDownAtTime(_muLower, _muUpper, price, timeSlotId);
    }
    
    // forward sweep to compute sumUp
    _buses[0]->_sumUp[timeSlotId].reset();
    for (int busId = 1; busId < numberOfBus; busId ++) {
        _buses[busId]->computeSumUpAtTime(timeSlotId);
    }
    
    // compute gradient
    for (int busId = 1; busId < numberOfBus; busId ++) {
        BusController *bus = _buses[busId];
        int numberOfPhasesAtBus = int( bus->_phaseIndicesInParentBus.size() );
        vector<double> price(numberOfPhasesAtBus, 0.0);
        for (int phaseId = 0; phaseId < numberOfPhasesAtBus; phaseId ++) {
            price[phaseId] = marginalPrice[_busPhaseIndicesInRoot[busId][phaseId]];
        }
        _buses[busId]->computeGradientAtTime(price, timeSlotId);
    }
}

void NetworkControl::computeGradientOverHorizon() {
    for (int timeSlotId = 0; timeSlotId < _numberOfSlots; timeSlotId ++) {
        computeGradientAtTime(timeSlotId);
    }
}


/******************************
 line search
 ******************************/

// compute tentative power consumptions
void NetworkControl::attemptPowerAtTime(const int &timeSlotId, unordered_set<LoadType> &enabledInControl) {
    for (int busId = 1; busId < _buses.size(); busId ++)
        _buses[busId]->attemptPowerAtTime(_stepSize, timeSlotId, enabledInControl);
    computePowerFlowAtTime(timeSlotId);
}

void NetworkControl::attemptPowerOverHorizon(unordered_set<LoadType> &enabledInControl) {
    for (int busId = 1; busId < _buses.size(); busId ++)
        _buses[busId]->attemptPowerOverHorizon(_stepSize, enabledInControl);
    computePowerFlowOverHorizon();
}

// set oldValue to current power consumption
void NetworkControl::updatePowerAtTime(const int &timeSlotId, unordered_set<LoadType> &enabledInControl) {
    for (int busId = 1; busId < _buses.size(); busId ++)
        _buses[busId]->updatePowerAtTime(timeSlotId, enabledInControl);
    _buses[0]->_oldAggregateLoads[timeSlotId] = _buses[0]->_aggregateLoads[timeSlotId];
}

void NetworkControl::updatePowerOverHorizon(unordered_set<LoadType> &enabledInControl) {
    for (int busId = 1; busId < _buses.size(); busId ++)
        _buses[busId]->updatePowerOverHorizon(enabledInControl);
    _buses[0]->_oldAggregateLoads = _buses[0]->_aggregateLoads;
}

// set current power consumption to oldValue
void NetworkControl::resetPowerAtTime(const int &timeSlotId, unordered_set<LoadType> &enabledInControl) {
    for (int busId = 1; busId < _buses.size(); busId ++)
        _buses[busId]->resetPowerAtTime(timeSlotId, enabledInControl);
    computePowerFlowAtTime(timeSlotId);
}

void NetworkControl::resetPowerOverHorizon(unordered_set<LoadType> &enabledInControl) {
    for (int busId = 1; busId < _buses.size(); busId ++)
        _buses[busId]->resetPowerOverHorizon(enabledInControl);
    computePowerFlowOverHorizon();
}

// check if there is voltage vilation
bool NetworkControl::voltageViolationAtTime(const int &timeSlotId) const {
    return _buses[0]->downstreamVoltageViolationAtTime(timeSlotId);
}

bool NetworkControl::voltageViolationOverHorizon() const {
    for (int timeSlotId = 0; timeSlotId < _numberOfSlots; timeSlotId ++) {
        if (voltageViolationAtTime(timeSlotId))
            return true;
    }
    return false;
}

// compute objective value
double NetworkControl::objectiveValueAtTime(const int &timeSlotId) const {
    double result = _buses[0]->downstreamObjectiveValueAtTime(_muLower, _muUpper, timeSlotId);
    for (int phaseId = 0; phaseId < _buses[0]->_bus->phase().size(); phaseId ++) {
        double power = _buses[0]->_aggregateLoads[timeSlotId]._power[phaseId].real();
        result += _quadCoef * power * power + _linCoef * power;
    }
    return result;
}

double NetworkControl::objectiveValueOverHorizon() const {
    double result = _buses[0]->downstreamObjectiveValueOverHorizon(_muLower, _muUpper);
    for (int timeSlotId = 0; timeSlotId < _numberOfSlots; timeSlotId ++) {
        for (int phaseId = 0; phaseId < _buses[0]->_bus->phase().size(); phaseId ++) {
            double power = _buses[0]->_aggregateLoads[timeSlotId]._power[phaseId].real();
            result += _quadCoef * power * power + _linCoef * power;
        }
    }
    return result;
}

// compute power update size
double NetworkControl::updateSizeAtTime(const int &timeSlotId) const {
    return _buses[0]->downstreamUpdateSizeAtTime(timeSlotId);
}

double NetworkControl::updateSizeOverHorizon() const {
    return _buses[0]->downstreamUpdateSizeOverHorizon();
}

// compute expected objective value change
double NetworkControl::expectedObjectiveValueChangeAtTime(const int &timeSlotId) const {
    return _buses[0]->downstreamExpectedObjectiveValueChangeAtTime(timeSlotId);
}

double NetworkControl::expectedObjectiveValueChangeOverHorizon() const {
    return _buses[0]->downstreamExpectedObjectiveValueChangeOverHorizon();
}


/******************************
 solve the power flow problem
 ******************************/

// initialize algorithm variables for fast control
void NetworkControl::fastControlInitialize() {
    // read data from real network
    // voltage
    BusController *substation = _buses[0];
    _substationVoltage = std::sqrt(std::norm(substation->_bus->voltage()[0]));
    initVoltageAtTime(0);
    
    // loads
    for (int busId = 1; busId < _buses.size(); busId ++) {
        vector<LoadController *> loads = _buses[busId]->_loadArray;
        for (int loadId = 0; loadId < loads.size(); loadId ++) {
            loads[loadId]->_valueArray[0] = loads[loadId]->_load->value();
            loads[loadId]->_oldValueArray[0] = loads[loadId]->_load->value();
        }
        _buses[busId]->computeAggregateLoadOnSelfAtTime(0);
        _buses[busId]->_oldAggregateLoads[0] = _buses[busId]->_aggregateLoads[0];
    }
    
    // initialize substation oldAggregateLoads
    computePowerFlowAtTime(0);
    substation->_oldAggregateLoads[0] = substation->_aggregateLoads[0];
}

void NetworkControl::slowControlInitialize(time_type time) {
    // read data from real network
    // voltage
    BusController *substation = _buses[0];
    _substationVoltage = std::sqrt(std::norm(substation->_bus->voltage()[0]));
    initVoltageOverHorizon();
    
    // loads
    for (int busId = 1; busId < _buses.size(); busId ++) {
        BusController *bus = _buses[busId];
        for (int loadId = 0; loadId < bus->_loadArray.size(); loadId ++) {
            LoadController *load = bus->_loadArray[loadId];
            load->_oldValueArray = load->_valueArray;
            if (load->_load->type() == ELECTRIC_VEHICLE) {
                ElectricVehicleController *evController = (ElectricVehicleController *)load;
                ElectricVehicle *ev = (ElectricVehicle *)(load->_load);
                evController->_plugInTimeSlotId = int( (ev->_plugInTime - time) / evController->_slotLengthInMinutes );
                if (evController->_plugInTimeSlotId < 0)
                    evController->_plugInTimeSlotId = 0;
                evController->_deadlineTimeSlotId = int( (ev->_deadline - time) / evController->_slotLengthInMinutes );
                if (evController->_deadlineTimeSlotId < 0)
                    evController->_deadlineTimeSlotId = 0;
            }
        }
        _buses[busId]->computeAggregateLoadOnSelfOverHorizon();
        _buses[busId]->_oldAggregateLoads = _buses[busId]->_aggregateLoads;
    }
    
    // initialize substation oldAggregateLoads
    computePowerFlowOverHorizon();
    substation->_oldAggregateLoads = substation->_aggregateLoads;
}

// outer loop of the solver
// in each iteration of the outer loop, use a different log-barrier function
// return the computed objective value
// muArray is an array of the mu values to be used in the inner loop, alpha is the backoff parameter, beta is the linearization quantification parameter, and epsilon is the threshold of power consumptions update
double NetworkControl::fastControlOuterLoop(std::vector<double> muArray, double alpha, double beta, double epsilon) {
    /******************************
     get a feasible point
     via optimizations
     ******************************/
    double voltageLowerBound = _buses.back()->_voltageMin;
    double voltageUpperBound = _buses.back()->_voltageMax;
    for (int iteration = 0; ; iteration ++) {
        double voltageMin = _substationVoltage;
        double voltageMax = _substationVoltage;
        bool voltageViolation = false;
        
        // compute current voltageMin, voltageMax, and voltageViolation
        for (int busId = 0; busId < _buses.size(); busId ++) {
            if (! _buses[busId]->_hasVoltageConstraint)
                continue;
            BusController *bus = _buses[busId];
            ColumnVector<complex_type> voltage = bus->_voltages[0];
            for (int phaseId = 0; phaseId < voltage.size(); phaseId ++) {
                double voltageMagnitude = std::sqrt(std::norm(voltage[phaseId]));
                if ( voltageMagnitude > voltageMax )
                    voltageMax = voltageMagnitude;
                if ( voltageMagnitude < voltageMin )
                    voltageMin = voltageMagnitude;
            }
        }
        if (voltageMin < voltageLowerBound)
            voltageViolation = true;
        if (voltageMax > voltageUpperBound)
            voltageViolation = true;
        
        // escape if voltage is within range
        if (!voltageViolation)  {
            for (int busId = 0; busId < _buses.size(); busId ++) {
                _buses[busId]->_voltageMin = voltageLowerBound;
                _buses[busId]->_voltageMax = voltageUpperBound;
            }
            break;
        }
        
        if (iteration == 15) {
            std::cout << "failed to find a feasible solution to start computation!" << std::endl;
            return 0.0;
        }
        
        // do optimization to drive voltage away from boundary
        voltageMin -= 0.001;
        if (voltageMin > voltageLowerBound)
            voltageMin = voltageLowerBound;
        voltageMax += 0.001;
        if (voltageMax < voltageUpperBound)
            voltageMax = voltageUpperBound;
        std::cout << "\tvoltageMin = " << voltageMin << std::endl;
        std::cout << "\tvoltageMax = " << voltageMax << std::endl;
        for (int busId = 0; busId < _buses.size(); busId ++) {
            _buses[busId]->_voltageMin = voltageMin;
            _buses[busId]->_voltageMax = voltageMax;
        }
        
        double mu = 1.0;
        _muLower = mu;
        _muUpper = mu;
        slowControlInnerLoop(alpha, beta, epsilon);
    }
    
    // set muArray to default value if empty
    if (muArray.size() == 0) {
        int numBus = (int) _buses.size();
        muArray.push_back(1.00/numBus);
        muArray.push_back(0.10/numBus);
        muArray.push_back(0.01/numBus);
        muArray.push_back(0.001/numBus);
    }
    
    // run inner loop for each of the mu values
    for (int i=0; i<muArray.size(); i++)
    {
        double mu = muArray[i];
        // printf("mu = %13.12f:\n", mu);
        _muLower = mu;
        _muUpper = mu;
        fastControlInnerLoop(alpha, beta, epsilon);
    }
    
    // compute power flow and return objective value
    _muLower = 0;
    _muUpper = 0;
    // std::cout << "\tObjective value is " << objectiveValueAtTime(0) << std::endl;
    return objectiveValueAtTime(0);
}

double NetworkControl::slowControlOuterLoop(std::vector<double> muArray, double alpha, double beta, double epsilon) {
    double voltageLowerBound = _buses.back()->_voltageMin;
    double voltageUpperBound = _buses.back()->_voltageMax;
    
    /******************************
     get a feasible point
     via substation adjustment
     ******************************/
    // get maximum and minimum voltages
    for (;;) {
        double voltageMin = _substationVoltage;
        double voltageMax = _substationVoltage;
        for (int busId = 0; busId < _buses.size(); busId ++) {
            if (! _buses[busId]->_hasVoltageConstraint) continue;
            BusController *bus = _buses[busId];
            for (int timeSlotId = 0; timeSlotId < bus->_voltages.size(); timeSlotId ++) {
                ColumnVector<complex_type> voltage = bus->_voltages[timeSlotId];
                for (int phaseId = 0; phaseId < voltage.size(); phaseId ++) {
                    double voltageMagnitude = std::sqrt(std::norm(voltage[phaseId]));
                    if ( voltageMagnitude > voltageMax )
                        voltageMax = voltageMagnitude;
                    if ( voltageMagnitude < voltageMin )
                        voltageMin = voltageMagnitude;
                }
            }
        }
        
        // adjust substation voltage if easy fix is available
        // if voltages are within range, do nothing
        if (voltageMin >= voltageLowerBound && voltageMax <= voltageUpperBound)
            break;
        
        // if can be fixed by shifting substation voltage, then try it
        double margin = (voltageUpperBound - voltageLowerBound) - (voltageMax - voltageMin);
        if (margin > 0) {
            _substationVoltage += voltageLowerBound - voltageMin;
            if (margin > 0.01)
                _substationVoltage += 0.005;
            else
                _substationVoltage += margin / 2.0;
            initVoltageOverHorizon();
            computePowerFlowOverHorizon();
            BusController *substation = _buses[0];
            substation->_oldAggregateLoads = substation->_aggregateLoads;
            break;
        }
        
        // otherwise there is the danger of cannot find a feasible point
        std::cout << "\tvoltageMin = " << voltageMin << std::endl;
        std::cout << "\tvoltageMax = " << voltageMax << std::endl;
        std::cout << "May fail to find a feasible point!" << std::endl;
        break;
    }
    
    
    /******************************
     get a feasible point
     via subsequent optimizations
     ******************************/
    for (int iteration = 0; ; iteration ++) {
        double voltageMin = _substationVoltage;
        double voltageMax = _substationVoltage;
        bool voltageViolation = false;
        
        // compute current voltageMin, voltageMax, and voltageViolation
        for (int busId = 0; busId < _buses.size(); busId ++) {
            if (! _buses[busId]->_hasVoltageConstraint)
                continue;
            BusController *bus = _buses[busId];
            for (int timeSlotId = 0; timeSlotId < bus->_voltages.size(); timeSlotId ++) {
                ColumnVector<complex_type> voltage = bus->_voltages[timeSlotId];
                for (int phaseId = 0; phaseId < voltage.size(); phaseId ++) {
                    double voltageMagnitude = std::sqrt(std::norm(voltage[phaseId]));
                    if ( voltageMagnitude > voltageMax ) {
                        voltageMax = voltageMagnitude;
                        // std::cout << "\tbus " << bus->_bus->name();
                        // std::cout << ", at time " << timeSlotId << ", phase " << bus->_bus->phase()[phaseId];
                        // std::cout << ", voltage " << voltageMagnitude << '\n';
                    }
                    if ( voltageMagnitude < voltageMin ) {
                        voltageMin = voltageMagnitude;
                        // std::cout << "\tbus " << bus->_bus->name();
                        // std::cout << ", at time " << timeSlotId << ", phase " << bus->_bus->phase()[phaseId];
                        // std::cout << ", voltage " << voltageMagnitude << '\n';
                    }
                }
            }
        }
        if (voltageMin < voltageLowerBound)
            voltageViolation = true;
        if (voltageMax > voltageUpperBound)
            voltageViolation = true;
        
        // escape if voltage is within range
        if (!voltageViolation)  {
            for (int busId = 0; busId < _buses.size(); busId ++) {
                _buses[busId]->_voltageMin = voltageLowerBound;
                _buses[busId]->_voltageMax = voltageUpperBound;
            }
            break;
        }
        
        if (iteration == 15) {
            std::cout << "failed to find a feasible solution to start computation!" << std::endl;
            return 0.0;
        }
        
        // do optimization to drive voltage away from boundary
        voltageMin -= 0.001;
        if (voltageMin > voltageLowerBound)
            voltageMin = voltageLowerBound;
        voltageMax += 0.001;
        if (voltageMax < voltageUpperBound)
            voltageMax = voltageUpperBound;
        std::cout << "\tvoltageMin = " << voltageMin << std::endl;
        std::cout << "\tvoltageMax = " << voltageMax << std::endl;
        for (int busId = 0; busId < _buses.size(); busId ++) {
            _buses[busId]->_voltageMin = voltageMin;
            _buses[busId]->_voltageMax = voltageMax;
        }
        
        double mu = 1.0;
        _muLower = mu;
        _muUpper = mu;
        slowControlInnerLoop(alpha, beta, epsilon);
    }
    
    
    /******************************
     start gradient decent from a feasible solution
     ******************************/
    if (muArray.size() == 0) {
        int numBus = (int) _buses.size();
        muArray.push_back(1.00/numBus);
        muArray.push_back(0.10/numBus);
        muArray.push_back(0.01/numBus);
        muArray.push_back(0.001/numBus);
    }
    
    // run inner loop for each of the mu values
    for (int i=0; i<muArray.size(); i++)
    {
        double mu = muArray[i];
        // printf("\tmu = %13.12f:\n", mu);
        _muLower = mu;
        _muUpper = mu;
        slowControlInnerLoop(alpha, beta, epsilon);
    }
    
    // compute power flow and return objective value
    _muLower = 0;
    _muUpper = 0;
    // std::cout << "\tObjective value is " << objectiveValueAtTime(0) << std::endl;
    return objectiveValueAtTime(0);
}

// inner loop of the solver
// in each iteration of the inner loop, do gradient decent to solve the OPF problem with a fixed log barrier function
// return the number of iterations used
// muLow is the mu value used in log(v-vMin), muUpp is the mu value used in log(vMax-v), alpha is the backoff parameter, beta is the linearization quantification parameter, and epsilon is the threshold of power consumptions update
int NetworkControl::fastControlInnerLoop(double alpha, double beta, double epsilon) {
    // update till improvements get too small
    int iteration = 1;
    while ( sizeof("Take a step") )
    {
        // printf("    Iteration %3d, ", iteration++);
        
        // compute objective value
        if ( voltageViolationAtTime(0) == true ) {
            std::cout << "Network::inner_loop---Must start with a feasible point!" << std::endl;
            return 1;
        }
        _oldObjectiveValue = objectiveValueAtTime(0);
        // printf("value_old = %+-7.6f, ", _oldObjectiveValue);
        
        // compute gradient
        computeGradientAtTime(0);
        
        // intialize step size
        _stepSize = 1.0;
        
        // line search
        while ( sizeof("Determine a step size") )
        {
            // attempt a step size
            attemptPowerAtTime(0, _enabledInFastControl);
            
            // if voltage violation happens, back off step size
            if ( voltageViolationAtTime(0) == true ) {
                _stepSize *= alpha;
                // std::cout << "voltage violation back off step size to " << _stepSize << std::endl;
                continue;
            }
            double newObjectiveValue = objectiveValueAtTime(0);
            
            // if update too small, prepare for return
            if (updateSizeAtTime(0) < epsilon)// ||
                // fabs(newObjectiveValue - _oldObjectiveValue) / fabs(_oldObjectiveValue) < 1e-7)
            {
                if (newObjectiveValue < _oldObjectiveValue)
                    updatePowerAtTime(0, _enabledInFastControl);
                else
                    resetPowerAtTime(0, _enabledInFastControl);
                
                // printf("stepSize = %5.4f, value = %+-7.6f\n", _stepSize, (newObjectiveValue < _oldObjectiveValue ? newObjectiveValue : _oldObjectiveValue));
                return iteration;
            }
            
            // if value is big, back off step size
            if (newObjectiveValue > _oldObjectiveValue + beta * expectedObjectiveValueChangeAtTime(0)) {
                _stepSize *= alpha;
                // std::cout << "not progressing back off step size to " << _stepSize << std::endl;
            }
            
            // else update power
            else
            {
                updatePowerAtTime(0, _enabledInFastControl);
                // printf("stepSize = %5.4f, value = %+-7.6f\n", _stepSize, newObjectiveValue);
                break;
            }
            
        }
    }
}

int NetworkControl::slowControlInnerLoop(double alpha, double beta, double epsilon) {
    // update till improvements get too small
    int iteration = 1;
    while ( sizeof("Take a step") )
    {
        // printf("\t\tIteration %3d, ", iteration++);
        
        // compute objective value
        if ( voltageViolationOverHorizon() == true ) {
            std::cout << "Network::inner_loop---Must start with a feasible point!" << std::endl;
            return 1;
        }
        _oldObjectiveValue = objectiveValueOverHorizon();
        // printf("value_old = %+-7.6f, ", _oldObjectiveValue);
        
        // compute gradient
        computeGradientOverHorizon();
        // printViolatedVoltages(std::cout);
        
        // intialize step size
        _stepSize = 1.0;
        
        // line search
        while ( sizeof("Determine a step size") )
        {
            // attempt a step size
            attemptPowerOverHorizon(_enabledInSlowControl);
            
            // if voltage violation happens, back off step size
            if ( voltageViolationOverHorizon() == true ) {
                _stepSize *= alpha;
                // std::cout << "voltage violation back off step size to " << _stepSize << '\t';
                continue;
            }
            double newObjectiveValue = objectiveValueOverHorizon();
            
            // if update too small, prepare for return
            if (updateSizeOverHorizon() < epsilon)// ||
                //fabs(newObjectiveValue - _oldObjectiveValue) / fabs(_oldObjectiveValue) < 1e-7)
            {
                if (newObjectiveValue < _oldObjectiveValue)
                    updatePowerOverHorizon(_enabledInSlowControl);
                else
                    resetPowerOverHorizon(_enabledInSlowControl);
                
                // printf("stepSize = %5.4f, value = %+-7.6f\n", _stepSize, (newObjectiveValue < _oldObjectiveValue ? newObjectiveValue : _oldObjectiveValue));
                return iteration;
            }
            
            // if value is big, back off step size
            if (newObjectiveValue > _oldObjectiveValue + beta * expectedObjectiveValueChangeOverHorizon()) {
                _stepSize *= alpha;
                // std::cout << "not progressing back off step size to " << _stepSize << '\t';
            }
            
            // else update power
            else {
                updatePowerOverHorizon(_enabledInSlowControl);
                // printf("stepSize = %5.4f, value = %+-7.6f\n", _stepSize, newObjectiveValue);
                break;
            }
            
        }
    }
}
