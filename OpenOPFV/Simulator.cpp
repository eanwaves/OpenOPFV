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
 * Module Simulator.cpp
 *
 ***********************************************************************/

#include "Simulator.h"

/******************************
 basic functions
 ******************************/

// default constructor
Simulator::Simulator() : _currentTimeInMinutes(-1.0) {
}

// copy constructor
Simulator::Simulator(const Simulator &simulator) :
_currentTimeInMinutes(simulator._currentTimeInMinutes),
_futureData(simulator._futureData),
_loadPredictor(simulator._loadPredictor),
_networkModel(simulator._networkModel),
_networkControl(simulator._networkControl),
_eventQueue(simulator._eventQueue),

_inputFolderName(simulator._inputFolderName),
_nextLoadFileName(simulator._nextLoadFileName),
_numberOfSlotPerLoadFile(simulator._numberOfSlotPerLoadFile),
_loadFileSlotLengthInMinutes(simulator._loadFileSlotLengthInMinutes),

_simulationHorizonInDays(simulator._simulationHorizonInDays),
_controlObjective(simulator._controlObjective),

_enableFastControl(simulator._enableFastControl),
_fastControlPeriodInSeconds(simulator._fastControlPeriodInSeconds),
_enablePhotoVoltaicFastControl(simulator._enablePhotoVoltaicFastControl),
_enableElectricVehicleFastControl(simulator._enableElectricVehicleFastControl),
_enablePoolPumpFastControl(simulator._enablePoolPumpFastControl),
_enableAirConditionerFastControl(simulator._enableAirConditionerFastControl),
_enableCapacitorFastControl(simulator._enableCapacitorFastControl),

_enableSlowControl(simulator._enableSlowControl),
_slowControlPeriodInMinutes(simulator._slowControlPeriodInMinutes),
_predictionWindowInHours(simulator._predictionWindowInHours),
_enablePhotoVoltaicSlowControl(simulator._enablePhotoVoltaicSlowControl),
_enableElectricVehicleSlowControl(simulator._enableElectricVehicleSlowControl),
_enablePoolPumpSlowControl(simulator._enablePoolPumpSlowControl),
_enableAirConditionerSlowControl(simulator._enableAirConditionerSlowControl),
_enableCapacitorSlowControl(simulator._enableCapacitorSlowControl),

_stop(simulator._stop) {
}

// release allocated spaces
void Simulator::clear() {
    _futureData.clear();
    _loadPredictor.clear();
    _networkModel.clear();
    _networkControl.clear();
    _eventQueue.clear();
    _currentTimeInMinutes = 0.0;
}

// deconstructor
Simulator::~Simulator() {
    clear();
}

// assignment
void Simulator::operator=(const Simulator &simulator) {
    _currentTimeInMinutes = simulator._currentTimeInMinutes;
    _futureData = simulator._futureData;
    _loadPredictor = simulator._loadPredictor;
    _networkModel = simulator._networkModel;
    _networkControl = simulator._networkControl;
    _eventQueue = simulator._eventQueue;
    
    _inputFolderName = simulator._inputFolderName;
    _nextLoadFileName = simulator._nextLoadFileName;
    _numberOfSlotPerLoadFile = simulator._numberOfSlotPerLoadFile;
    _loadFileSlotLengthInMinutes = simulator._loadFileSlotLengthInMinutes;
    
    _simulationHorizonInDays = simulator._simulationHorizonInDays;
    _controlObjective = simulator._controlObjective;
    
    _enableFastControl = simulator._enableFastControl;
    _fastControlPeriodInSeconds = simulator._fastControlPeriodInSeconds;
    _enablePhotoVoltaicFastControl = simulator._enablePhotoVoltaicFastControl;
    _enableElectricVehicleFastControl = simulator._enableElectricVehicleFastControl;
    _enablePoolPumpFastControl = simulator._enablePoolPumpFastControl;
    _enableAirConditionerFastControl = simulator._enableAirConditionerFastControl;
    _enableCapacitorFastControl = simulator._enableCapacitorFastControl;
    
    _enableSlowControl = simulator._enableSlowControl;
    _slowControlPeriodInMinutes = simulator._slowControlPeriodInMinutes;
    _predictionWindowInHours = simulator._predictionWindowInHours;
    _enablePhotoVoltaicSlowControl = simulator._enablePhotoVoltaicSlowControl;
    _enableElectricVehicleSlowControl = simulator._enableElectricVehicleSlowControl;
    _enablePoolPumpSlowControl = simulator._enablePoolPumpSlowControl;
    _enableAirConditionerSlowControl = simulator._enableAirConditionerSlowControl;
    _enableCapacitorSlowControl = simulator._enableCapacitorSlowControl;
    
    _stop = simulator._stop;
}

// print
ostream &operator<<(ostream &cout, Simulator *simulator) {
    cout << simulator->networkModel();
    return cout;
}

void Simulator::printSubstationPowerInjectionOverHorizon(ostream &cout) {
    BusController *busController = _networkControl._buses[0];
    for (int phaseId = 0; phaseId < busController->_bus->phase().size(); phaseId ++) {
        cout << "\tphase " << busController->_bus->phase()[phaseId] << ':';
        for (int timeSlotId = 0; timeSlotId < busController->_aggregateLoads.size(); timeSlotId ++)
            cout << '\t' << busController->_aggregateLoads[timeSlotId]._power[phaseId].real();
        cout << '\n';
    }
}


/******************************
 basic functions
 ******************************/

// getter functions
time_type Simulator::currentTimeInMinutes() const {
    return _currentTimeInMinutes;
}

FutureData *Simulator::futureData() {
    return &_futureData;
}

LoadPredictor *Simulator::loadPredictor() {
    return &_loadPredictor;
}

NetworkModel *Simulator::networkModel() {
    return &_networkModel;
}

NetworkControl *Simulator::networkControl() {
    return &_networkControl;
}

EventQueue *Simulator::eventQueue() {
    return &_eventQueue;
}

string Simulator::inputFolderName() const {
    return _inputFolderName;
}

string Simulator::nextLoadFileName() const {
    return _nextLoadFileName;
}

int Simulator::numberOfSlotPerLoadFile() const {
    return _numberOfSlotPerLoadFile;
}

time_type Simulator::loadFileSlotLengthInMinutes() const {
    return _loadFileSlotLengthInMinutes;
}

int Simulator::simulationHorizonInDays() const {
    return _simulationHorizonInDays;
}

ControlObjective Simulator::controlObjective() const {
    return _controlObjective;
}

bool Simulator::enableFastControl() const {
    return _enableFastControl;
}

time_type Simulator::fastControlPeriodInSeconds() const {
    return _fastControlPeriodInSeconds;
}

bool Simulator::enablePhotoVoltaicFastControl() const {
    return _enablePhotoVoltaicFastControl;
}

bool Simulator::enableElectricVehicleFastControl() const {
    return _enableElectricVehicleFastControl;
}

bool Simulator::enablePoolPumpFastControl() const {
    return _enablePoolPumpFastControl;
}

bool Simulator::enableAirConditionerFastControl() const {
    return _enableAirConditionerFastControl;
}

bool Simulator::enableCapacitorFastControl() const {
    return _enableCapacitorFastControl;
}

bool Simulator::enableSlowControl() const {
    return _enableSlowControl;
}

time_type Simulator::slowControlPeriodInMinutes() const {
    return _slowControlPeriodInMinutes;
}

time_type Simulator::predictionWindowInHours() const {
    return _predictionWindowInHours;
}

bool Simulator::enablePhotoVoltaicSlowControl() const {
    return _enablePhotoVoltaicSlowControl;
}

bool Simulator::enableElectricVehicleSlowControl() const {
    return _enableElectricVehicleSlowControl;
}

bool Simulator::enablePoolPumpSlowControl() const {
    return _enablePoolPumpSlowControl;
}

bool Simulator::enableAirConditionerSlowControl() const {
    return _enableAirConditionerSlowControl;
}

bool Simulator::enableCapacitorSlowControl() const {
    return _enableCapacitorSlowControl;
}

bool Simulator::stop() const {
    return _stop;
}

void Simulator::setCurrentTimeInMinutes(const time_type &currentTimeInMinutes) {
    _currentTimeInMinutes = currentTimeInMinutes;
}

void Simulator::setInputFolderName(const string &inputFolderName) {
    _inputFolderName = inputFolderName;
}

void Simulator::setNextLoadFileName(const string &nextLoadFileName) {
    _nextLoadFileName = nextLoadFileName;
}

void Simulator::setNumberOfSlotPerLoadFile(const int &numberOfSlotPerLoadFile) {
    _numberOfSlotPerLoadFile = numberOfSlotPerLoadFile;
}

void Simulator::setLoadFileSlotLengthInMinutes(const time_type &loadFileSlotLengthInMinutes) {
    _loadFileSlotLengthInMinutes = loadFileSlotLengthInMinutes;
}

void Simulator::setSimulationHorizonInDays(const int &simulationHorizonInDays) {
    _simulationHorizonInDays = simulationHorizonInDays;
}

void Simulator::setControlObjective(const ControlObjective &controlObjective) {
    _controlObjective = controlObjective;
}

void Simulator::setEnableFastControl(const bool &enableFastControl) {
    _enableFastControl = enableFastControl;
}

void Simulator::setFastControlPeriodInSeconds(const time_type &fastControlPeriodInSeconds) {
    _fastControlPeriodInSeconds = fastControlPeriodInSeconds;
}

void Simulator::setEnablePhotoVoltaicFastControl(const bool &enablePhotoVoltaicFastControl) {
    _enablePhotoVoltaicFastControl = enablePhotoVoltaicFastControl;
}

void Simulator::setEnableElectricVehicleFastControl(const bool &enableElectricVehicleFastControl) {
    _enableElectricVehicleFastControl = enableElectricVehicleFastControl;
}

void Simulator::setEnablePoolPumpFastControl(const bool &enablePoolPumpFastControl) {
    _enablePoolPumpFastControl = enablePoolPumpFastControl;
}

void Simulator::setEnableAirConditionerFastControl(const bool &enableAirConditionerFastControl) {
    _enableAirConditionerFastControl = enableAirConditionerFastControl;
}

void Simulator::setEnableCapacitorFastControl(const bool &enableCapacitorFastControl) {
    _enableCapacitorFastControl = enableCapacitorFastControl;
}

void Simulator::setEnableSlowControl(const bool &enableSlowControl) {
    _enableSlowControl = enableSlowControl;
}

void Simulator::setSlowControlPeriodInMinutes(const time_type &slowControlPeriodInMinutes) {
    _slowControlPeriodInMinutes = slowControlPeriodInMinutes;
}

void Simulator::setPredictionWindowInHours(const time_type &predictionWindowInHours) {
    _predictionWindowInHours = predictionWindowInHours;
}

void Simulator::setEnablePhotoVoltaicSlowConotrol(const bool &enablePhotoVoltaicSlowControl) {
    _enablePhotoVoltaicSlowControl = enablePhotoVoltaicSlowControl;
}

void Simulator::setEnableElectricVehicleSlowControl(const bool &enableElectricVehicleSlowControl) {
    _enableElectricVehicleSlowControl = enableElectricVehicleSlowControl;
}

void Simulator::setEnablePoolPumpSlowControl(const bool &enablePoolPumpSlowControl) {
    _enablePoolPumpSlowControl = enablePoolPumpSlowControl;
}

void Simulator::setEnableAirConditionerSlowControl(const bool &enableAirConditionerSlowControl) {
    _enableAirConditionerSlowControl = enableAirConditionerSlowControl;
}

void Simulator::setEnableCapacitorSlowControl(const bool &enableCapacitorSlowControl) {
    _enableCapacitorSlowControl = enableCapacitorSlowControl;
}

void Simulator::setStop(const bool &stop) {
    _stop = stop;
}


/******************************
 initialization functions
 ******************************/

// network model: load network data
// future data: load future data
// network controller: set up controllers for devices
// event queue: initialize event queue (ev arrival and departure, fast control and slow control scheduling, update network status)
// load predicator: initialize load predicator
void Simulator::initialize() {
    clear();
    
    initNetworkTopology();
    _networkModel._substationVoltage = 1.0;
    
    setNextLoadFileName("loadOnDay1.txt");
    initLoadData();
    
    initNetworkControl();

    initLoadPredicator();
     
    _eventQueue.setEndTime(_simulationHorizonInDays * 1440.0);
    initControlSchedule();
    
    _networkModel.initVoltage();
    
    
    // comment out this section if do not need to verify the correctness of networkController
    /*
     std::ofstream file;
     file.open("/Users/ganlingwen/Desktop/modelFile.txt");
     file << _networkModel;
     file.close();
     file.open("/Users/ganlingwen/Desktop/controlFile.txt");
     file << _networkControl;
     file.close();*/
}

// load network topology (buses and lines) using "network.txt"
void Simulator::initNetworkTopology() {
    std::ifstream file(_inputFolderName + "/network.txt");
    string str;
    file >> str;
    if (!file.good() ||
        str.compare("<network>") != 0) {
        std::cout << "Failed to open file!" << std::endl;
        return;
    }
    
    // parse network topology input file
    file >> str;
    while ( str.compare("</network>") != 0 ) {
        if (str.compare("<substation>") == 0) {
            Bus *substation = parseSubstation(file);
            _networkModel.addABus(substation);
        }
        else if (str.compare("<bus>") == 0) {
            Bus *bus = parseBus(file);
            _networkModel.addABus(bus);
        }
        else if (str.compare("<house>") == 0) {
            Bus *bus = parseHouse(file);
            _networkModel.addABus(bus);
        }
        else if (str.compare("<line>") == 0) {
            Line *line = parseLine(file);
            _networkModel.addALine(line);
        }
        file >> str;
    }
    
    file.close();
    _networkModel.sortBusAndLineByBreadthFirstSearch();
}

// network model: load device data
// event queue: initialize ev arrival and departure
// future data: load future data
void Simulator::initLoadData() {
    // read input file configuration
    std::ifstream file(_inputFolderName + "/info.txt");
    string str;
    file >> str;
    if (!file.good() ||
        str.compare("<info>") != 0) {
        std::cout << "Failed to open file!" << std::endl;
        return;
    }
    
    file >> str;
    while ( str.compare("</info>") != 0 ) {
        if (str.compare("<numSlotPerFile>") == 0) {
            _numberOfSlotPerLoadFile = parseValue<int>(file, "numSlotPerFile");
        }
        else if (str.compare("<numMinutePerSlot>") == 0) {
            _loadFileSlotLengthInMinutes = parseValue<double>(file, "numMinutePerSlot");
        }
        file >> str;
    }
    file.close();
    
    
    // read load input file
    file.open(_inputFolderName + '/' + _nextLoadFileName);
    file >> str;
    if (!file.good() ||
        str.compare("<LoadFile>") != 0) {
        std::cout << "Failed to open load file!" << std::endl;
        return;
    }
    
    // set next load file
    setNextLoadFile();
    
    // parse load data
    file >> str;
    while (str.compare("</LoadFile>") != 0) {
        if (str.compare("<baseLoad>") == 0) {
            vector<LoadData *> loadDataArray;
            Load *load = parseBaseLoad(file, loadDataArray);
            _networkModel.addALoad(load);
            _futureData.addFutureDataForLoad(load->name(), loadDataArray);
        }
        else if (str.compare("<photovoltaic>") == 0) {
            vector<LoadData *> loadDataArray;
            Load *load = parsePhotoVoltaic(file, loadDataArray);
            _networkModel.addALoad(load);
            _futureData.addFutureDataForLoad(load->name(), loadDataArray);
        }
        else if (str.compare("<electricVehicle>") == 0) {
            ElectricVehicle *ev = parseElectricVehicle(file);
            // init ev arrival event
            Event event;
            event._time = ev->_plugInTime;
            event._action = ADD_A_LOAD;
            event._parameter = (void *)ev;
            _eventQueue.push(event);
            // init ev departure event
            event._time = ev->_deadline;
            event._action = DELETE_A_LOAD;
            _eventQueue.push(event);
        }
        file >> str;
    }
}

void Simulator::addLoadData() {
    // read load input file
    std::ifstream file;
    file.open(_inputFolderName + '/' + _nextLoadFileName);
    string str;
    file >> str;
    if (!file.good() ||
        str.compare("<LoadFile>") != 0) {
        std::cout << "Failed to open load file!" << std::endl;
        return;
    }
    
    // set next load file
    setNextLoadFile();
    
    // parse load data
    int dayIndex = int(_currentTimeInMinutes / 1440.0) + 1;
    file >> str;
    while (str.compare("</LoadFile>") != 0) {
        if (str.compare("<baseLoad>") == 0) {
            vector<LoadData *> loadDataArray;
            Load *load = parseBaseLoad(file, loadDataArray);
            for (int timeSlotId = 0; timeSlotId < loadDataArray.size(); timeSlotId ++)
                loadDataArray[timeSlotId]->_timeInMinutes += 1440.0 * dayIndex;
            _futureData.addFutureDataForLoad(load->name(), loadDataArray);
            delete load;
        }
        else if (str.compare("<photovoltaic>") == 0) {
            vector<LoadData *> loadDataArray;
            Load *load = parsePhotoVoltaic(file, loadDataArray);
            for (int timeSlotId = 0; timeSlotId < loadDataArray.size(); timeSlotId ++)
                loadDataArray[timeSlotId]->_timeInMinutes += 1440.0 * dayIndex;
            _futureData.addFutureDataForLoad(load->name(), loadDataArray);
            delete load;
        }
        else if (str.compare("<electricVehicle>") == 0) {
            ElectricVehicle *ev = parseElectricVehicle(file);
            ev->_plugInTime += dayIndex * 1440;
            ev->_deadline += dayIndex * 1440;
            // init ev arrival event
            Event event;
            event._time = ev->_plugInTime;
            event._action = ADD_A_LOAD;
            event._parameter = (void *)ev;
            _eventQueue.push(event);
            // init ev departure event
            event._time = ev->_deadline;
            event._action = DELETE_A_LOAD;
            _eventQueue.push(event);
        }
        file >> str;
    }
}

void Simulator::setNextLoadFile() {
    if (_nextLoadFileName.compare("loadOnDay1.txt") == 0)
        _nextLoadFileName = "loadOnDay2.txt";
    else if (_nextLoadFileName.compare("loadOnDay2.txt") == 0)
        _nextLoadFileName = "loadOnDay3.txt";
    else if (_nextLoadFileName.compare("loadOnDay3.txt") == 0)
        _nextLoadFileName = "loadOnDay1.txt";
    else {
        std::cout << "Unexpected loadFileName " << _nextLoadFileName << std::endl;
        exit(1);
    }
}

// network controller: initialize controller
void Simulator::initNetworkControl() {
    // init _numberOfSlots, which is necessary in constructing the buses and lines
    if (_enableSlowControl) {
        int numberOfSlotsInSlowControl = int(_predictionWindowInHours * 60 / _slowControlPeriodInMinutes);
        _networkControl.setNumberOfSlots(numberOfSlotsInSlowControl);
        _networkControl._slotLengthInMinutes = _slowControlPeriodInMinutes;
    }
    else
        _networkControl.setNumberOfSlots(1);
    
    // construct _enabledInFastControl and _enabledInSlowControl
    if (_enableFastControl) {
        if (_enablePhotoVoltaicFastControl)
            _networkControl._enabledInFastControl.insert(PHOTOVOLTAIC);
        if (_enableElectricVehicleFastControl)
            _networkControl._enabledInFastControl.insert(ELECTRIC_VEHICLE);
        if (_enablePoolPumpFastControl)
            _networkControl._enabledInFastControl.insert(POOL_PUMP);
        if (_enableAirConditionerFastControl)
            _networkControl._enabledInFastControl.insert(AIR_CONDITIONER);
        if (_enableCapacitorFastControl)
            _networkControl._enabledInFastControl.insert(CAPACITOR);
    }
    if (_enableSlowControl) {
        if (_enablePhotoVoltaicSlowControl)
            _networkControl._enabledInSlowControl.insert(PHOTOVOLTAIC);
        if (_enableElectricVehicleSlowControl)
            _networkControl._enabledInSlowControl.insert(ELECTRIC_VEHICLE);
        if (_enablePoolPumpSlowControl)
            _networkControl._enabledInSlowControl.insert(POOL_PUMP);
        if (_enableAirConditionerSlowControl)
            _networkControl._enabledInSlowControl.insert(AIR_CONDITIONER);
        if (_enableCapacitorSlowControl)
            _networkControl._enabledInSlowControl.insert(CAPACITOR);
    }
    
    // initialize other attributes
    _networkControl.initialize(_networkModel);
}

// load predicator
void Simulator::initLoadPredicator() {
    // to be written
}

// event queue: initialize fast and slow control scheduling and network status update
void Simulator::initControlSchedule() {
    if (_enableFastControl) {
        time_type time = 0.0;
        Element *element = NULL;
        ActionType action = FAST_CONTROL;
        void *parameter = NULL;
        Event event(time, element, action, parameter);
        _eventQueue.push(event);
    }
    
    if (_enableSlowControl) {
        time_type time = 0.0;
        Element *element = NULL;
        ActionType action = SLOW_CONTROL;
        void *parameter = NULL;
        Event event(time, element, action, parameter);
        _eventQueue.push(event);
    }
    
    time_type time = 0.0;
    Element *element = NULL;
    ActionType action = UPDATE_TO_TIME;
    void *parameter = NULL;
    Event event(time, element, action, parameter);
    _eventQueue.push(event);
    
    event._time = 1440.0 - _predictionWindowInHours * 60;
    event._action = LOAD_FUTURE_DATA;
    _eventQueue.push(event);
}


/******************************
 other functions
 ******************************/

// execute an event on top of the heap
EventReturnType Simulator::executeEvent() {
    // check stopping criteria
    if (_eventQueue.empty()) {
        _futureData.clear();
        return EVENT_QUEUE_EMPTY;
    }
    
    // get the next event to be executed
    Event event = _eventQueue.nextEvent();
    time_type time = event._time;
    ActionType action = event._action;
    void *parameter = event._parameter;
    
    // comment out this line of code if do not need to verify event queue correctness
    std::cout << "Time " << time << ", action = " << action << std::endl;
    
    // update networkModel status if time proceeds
    EventReturnType result;
    if (time > _currentTimeInMinutes) {
        result = NEED_PLOTTING;
        updateToTime(time);
    }
    else
        result = NEED_NO_ACTION;
    
    
    // handle fast control
    if (action == FAST_CONTROL) {
        _networkControl.fastControl();
        _networkModel.computePowerFlowWithGridLabD();
        event._time += fastControlPeriodInSeconds() / 60;
        _eventQueue.push(event);
        result = NEED_PLOTTING;
    }
    
    // handle slow control
    else if (action == SLOW_CONTROL) {
        _loadPredictor.makePrediction(&_networkControl, &_futureData, time, _slowControlPeriodInMinutes);
        _networkControl.slowControl(time);
        _networkModel.computePowerFlowWithGridLabD();
        // printSubstationPowerInjectionOverHorizon(std::cout);
        event._time += slowControlPeriodInMinutes();
        _eventQueue.push(event);
        result = NEED_PLOTTING;
    }
    
    // handle updateToTime
    else if (action == UPDATE_TO_TIME) {
        _networkModel.computePowerFlowWithGridLabD();
        event._time += loadFileSlotLengthInMinutes();
        _eventQueue.push(event);
    }
    
    // handle adding a load
    else if (action == ADD_A_LOAD) {
        Load *load = (Load *)parameter;
        _networkModel.addALoad(load);
        _networkControl.addALoad(load);
    }
    
    // handle deleting a load
    else if (action == DELETE_A_LOAD) {
        Load *load = (Load *)parameter;
        _networkModel.deleteALoad(load);
        _networkControl.deleteALoad(load);
    }
    
    // handle load more data
    else if (action == LOAD_FUTURE_DATA) {
        addLoadData();
        event._time += 1440;
        _eventQueue.push(event);
    }
    
    return result;
}

// update networkModel to currentTime using information from futureData and networkControl
void Simulator::updateToTime(time_type time) {
    _networkModel.fetchRealTimeData(_currentTimeInMinutes, time, &_futureData);
    _currentTimeInMinutes = time;
}

// get the current objective value
double Simulator::computeObjectiveValue() {
    // get substation power injection
    Bus *substation = _networkModel.getBusByIndex(0);
    ColumnVector<complex_type> substationPowerInjection = substation->aggregateLoad()._power;
    
    // return aggregate real power injection
    double result = 0.0;
    for (int phaseId = 0; phaseId < substationPowerInjection.size(); phaseId ++) {
        double realPower = substationPowerInjection[phaseId].real();
        result += realPower * realPower;
    }
    return result;
}


/******************************
 parser functions
 ******************************/

// parse an attribute of type T, of a single entry
template <class T>
T Simulator::parseValue(std::ifstream &file,
                        const string &attributeName) {
    T result;
    file >> result;
    string endString;
    file >> endString;
    string expectedEndString = "</" + attributeName + ">";
    if (endString.compare(expectedEndString) != 0) {
        std::cout << "Expected to see " << expectedEndString;
        std::cout << ", but see " << endString << std::endl;
        exit(1);
    }
    return result;
}

// parse an attribute of type T, of length numEntry
template <class T>
vector<T> Simulator::parseValueArray(std::ifstream &file,
                                     const int &numEntry,
                                     const string &attributeName) {
    vector<T> result;
    result.reserve(numEntry);
    for (int i = 0; i < numEntry; i ++) {
        T entry;
        file >> entry;
        result.push_back(entry);
    }
    
    string endString;
    file >> endString;
    string expectedEndString = "</" + attributeName + ">";
    if (endString.compare(expectedEndString) != 0) {
        std::cout << "Expecting to see " << expectedEndString << ", but see " << endString << std::endl;
        exit(1);
    }
    return result;
}

// parse a complex type square matrix
SquareMatrix<complex_type> Simulator::parseComplexSquareMatrix(std::ifstream &file,
                                                               Element *element,
                                                               const string &attributeName) {
    SquareMatrix<complex_type> result(element->phase());
    int n = int( element->phase().size() );
    vector<double> parseResult = parseValueArray<double>(file, n * n, attributeName);
    for (int row = 0; row < n; row ++) {
        for (int col = 0; col < n; col ++) {
            result[row][col] = parseResult[n * row + col];
        }
    }
    return result;
}

// parse a complex type column vector
ColumnVector<complex_type> Simulator::parseComplexColumnVector(std::ifstream &file,
                                                               Element *element,
                                                               const string &attributeName) {
    ColumnVector<complex_type> result(element->phase());
    int n = int( element->phase().size() );
    vector<double> parseResult = parseValueArray<double>(file, n, attributeName);
    for (int index = 0; index < n; index ++) {
        result[index] = parseResult[index];
    }
    return result;
}

// parse a real type column vector
ColumnVector<double> Simulator::parseRealColumnVector(std::ifstream &file,
                                                      Element *element,
                                                      const string &attributeName) {
    ColumnVector<double> result( element->phase() );
    int n = int( element->phase().size() );
    vector<double> parseResult = parseValueArray<double>(file, n, attributeName);
    for (int index = 0; index < n; index ++) {
        result[index] = parseResult[index];
    }
    return result;
}

// parse to find the pointer of a bus
Bus *Simulator::parseBusPointer(std::ifstream &file,
                                const string &attributeName) {
    string busName = parseValue<string>(file, attributeName);
    return _networkModel.getBusByName(busName);
}

// parse coordinate
Coordinate Simulator::parseCoordinate(std::ifstream &file) {
    vector<double> result = parseValueArray<double>(file, 2, "coordinate");
    return Coordinate(result[0], result[1]);
}

// parse admittance
SquareMatrix<complex_type> Simulator::parseAdmittance(std::ifstream &file,
                                                      Bus *bus) {
    return parseComplexSquareMatrix(file, bus, "b") * complex_type(0.0, 1.0);
}

// parse a derived bus type
Bus *Simulator::parseSubstation(std::ifstream &file) {
    Bus *bus = new Bus();
    bus->setType(SUBSTATION);
    
    // read attributes
    string str;
    file >> str;
    while (str.compare("</substation>") != 0) {
        if (str.compare("<name>") == 0) {
            string name = parseValue<string>(file, "name");
            bus->setName(name);
        } else if (str.compare("<phase>") == 0) {
            string phase = parseValue<string>(file, "phase");
            bus->setPhase(phase);
        } else if (str.compare("<coordinate>") == 0) {
            Coordinate coordinate = parseCoordinate(file);
            bus->setCoordinate(coordinate);
        } else if (str.compare("<b>") == 0) {
            SquareMatrix<complex_type> admittance = parseAdmittance(file, bus);
            bus->setShunt(admittance);
        }
        file >> str;
    }
    
    return bus;
}

Bus *Simulator::parseBus(std::ifstream &file) {
    Bus *bus = new Bus();
    bus->setType(BUS);
    
    // read attributes
    string str;
    file >> str;
    while (str.compare("</bus>") != 0) {
        if (str.compare("<name>") == 0) {
            string name = parseValue<string>(file, "name");
            bus->setName(name);
        } else if (str.compare("<phase>") == 0) {
            string phase = parseValue<string>(file, "phase");
            bus->setPhase(phase);
        } else if (str.compare("<coordinate>") == 0) {
            Coordinate coordinate = parseCoordinate(file);
            bus->setCoordinate(coordinate);
        } else if (str.compare("<b>") == 0) {
            SquareMatrix<complex_type> admittance = parseAdmittance(file, bus);
            bus->setShunt(admittance);
        }
        file >> str;
    }
    
    return bus;
}

Bus *Simulator::parseHouse(std::ifstream &file) {
    Bus *bus = new Bus();
    bus->setType(HOUSE);
    
    // read attributes
    string str;
    file >> str;
    while (str.compare("</house>") != 0) {
        if (str.compare("<name>") == 0) {
            string name = parseValue<string>(file, "name");
            bus->setName(name);
        } else if (str.compare("<phase>") == 0) {
            string phase = parseValue<string>(file, "phase");
            bus->setPhase(phase);
        } else if (str.compare("<coordinate>") == 0) {
            Coordinate coordinate = parseCoordinate(file);
            bus->setCoordinate(coordinate);
        } else if (str.compare("<b>") == 0) {
            SquareMatrix<complex_type> admittance = parseAdmittance(file, bus);
            bus->setShunt(admittance);
        }
        file >> str;
    }
    
    return bus;
}

// parse impedance
SquareMatrix<complex_type> Simulator::parseImpedance(std::ifstream &file,
                                                     Line *line) {
    SquareMatrix<complex_type> result(line->phase());
    int n = int( line->phase().size() );
    vector<double> rArray = parseValueArray<double>(file, n * n, "r");
    string str;
    file >> str;
    if (str.compare("<x>") != 0) {
        std::cout << "expect to see <x> after </r>";
        exit(1);
    }
    vector<double> xArray = parseValueArray<double>(file, n * n, "x");
    for (int row = 0; row < n; row ++) {
        for (int col = 0; col < n; col ++)
            result[row][col] = complex_type(rArray[n * row + col], xArray[n * row + col]);
    }
    return result;
}

// parse a derived line type
Line *Simulator::parseLine(std::ifstream &file) {
    Line *line = new Line();
    line->setType(LINE);
    
    // read attributes
    string str;
    file >> str;
    while (str.compare("</line>") != 0) {
        if (str.compare("<name>") == 0) {
            string name = parseValue<string>(file, "name");
            line->setName(name);
        }
        else if (str.compare("<phase>") == 0) {
            string phase = parseValue<string>(file, "phase");
            line->setPhase(phase);
        }
        else if (str.compare("<fromBusName>") == 0) {
            Bus *fromBus = parseBusPointer(file, "fromBusName");
            line->setFromBus(fromBus);
        }
        else if (str.compare("<toBusName>") == 0) {
            Bus *toBus = parseBusPointer(file, "toBusName");
            line->setToBus(toBus);
        }
        else if (str.compare("<r>") == 0) {
            SquareMatrix<complex_type> impedance = parseImpedance(file, line);
            line->setImpedance(impedance);
        }
        file >> str;
    }
    
    return line;
}

// parse complex power consumption
ColumnVector<complex_type> Simulator::parsePower(std::ifstream &file,
                                                 Load *load) {
    ColumnVector<complex_type> result( load->phase() );
    int n = int( load->phase().size() );
    vector<double> pArray = parseValueArray<double>(file, n, "p");
    string str;
    file >> str;
    if (str.compare("<q>") != 0) {
        std::cout << "expect to see <q> after </p>!" << std::endl;
        exit(1);
    }
    vector<double> qArray = parseValueArray<double>(file, n, "q");
    for (int index = 0; index < n; index ++) {
        result[index] = complex_type(pArray[index], qArray[index]);
    }
    return result;
}

// parse a derived load data type
BaseLoadData *Simulator::parseBaseLoadData(std::ifstream &file,
                                           Load *load) {
    BaseLoadData *loadData = new BaseLoadData(load->phase());
    int n = int( load->phase().size() );
    vector<double> pArray;
    vector<double> qArray;
    
    string str;
    file >> str;
    while (str.compare("</LoadData>") != 0) {
        if (str.compare("<time>") == 0) {
            loadData->_timeInMinutes = parseValue<double>(file, "time");
        }
        else if (str.compare("<p>") == 0) {
            pArray = parseValueArray<double>(file, n, "p");
        }
        else if (str.compare("<q>") == 0) {
            qArray = parseValueArray<double>(file, n, "q");
        }
        file >> str;
    }
    
    LoadValue loadValue = loadData->_loadValue;
    for (int i = 0; i < n; i ++) {
        loadValue._power[i] = complex_type(pArray[i], qArray[i]) * 0.5;
        loadValue._admittance[i][i] = complex_type(pArray[i], -qArray[i]) * 0.5;
    }
    loadData->_loadValue = loadValue;
    
    return loadData;
}

PhotoVoltaicData *Simulator::parsePhotoVoltaicData(std::ifstream &file) {
    PhotoVoltaicData *loadData = new PhotoVoltaicData();
    
    string str;
    file >> str;
    while (str.compare("</LoadData>") != 0) {
        if (str.compare("<time>") == 0) {
            loadData->_timeInMinutes = parseValue<double>(file, "time");
        }
        else if (str.compare("<p>") == 0) {
            loadData->_realPower = parseValue<double>(file, "p");
        }
        file >> str;
    }
    
    return loadData;
}

// parse a derived load type
Load *Simulator::parseBaseLoad(std::ifstream &file,
                               vector<LoadData *> &loadDataArray) {
    Load *load = new Load();
    load->setType(BASELOAD);
    
    // read attributes
    string str;
    file >> str;
    while (str.compare("</baseLoad>") != 0) {
        if (str.compare("<locationBusName>") == 0) {
            Bus *locationBus = parseBusPointer(file, "locationBusName");
            load->setLocationBus(locationBus);
        }
        else if (str.compare("<name>") == 0) {
            string name = parseValue<string>(file, "name");
            load->setName(name);
        }
        else if (str.compare("<phase>") == 0) {
            string phase = parseValue<string>(file, "phase");
            load->setPhase(phase);
        }
        else if (str.compare("<LoadData>") == 0) {
            LoadData *loadData = parseBaseLoadData(file, load);
            loadDataArray.push_back(loadData);
        }
        file >> str;
    }
    
    load->initPhaseIndicesInLocationBus();
    return load;
}

PhotoVoltaic *Simulator::parsePhotoVoltaic(std::ifstream &file,
                                           vector<LoadData *> &loadDataArray) {
    PhotoVoltaic *load = new PhotoVoltaic();
    load->setType(PHOTOVOLTAIC);
    
    // read attributes
    string str;
    file >> str;
    while (str.compare("</photovoltaic>")) {
        if (str.compare("<locationBusName>") == 0) {
            Bus *locationBus = parseBusPointer(file, "locationBusName");
            load->setLocationBus(locationBus);
        }
        else if (str.compare("<name>") == 0) {
            string name = parseValue<string>(file, "name");
            load->setName(name);
        }
        else if (str.compare("<phase>") == 0) {
            string phase = parseValue<string>(file, "phase");
            load->setPhase(phase);
        }
        else if (str.compare("<nameplate>") == 0) {
            double nameplate = parseValue<double>(file, "nameplate");
            load->setNameplate(nameplate);
        }
        else if (str.compare("<LoadData>") == 0) {
            LoadData *loadData = parsePhotoVoltaicData(file);
            loadDataArray.push_back(loadData);
        }
        file >> str;
    }
    
    load->initPhaseIndicesInLocationBus();
    return load;
}

time_type Simulator::timeStringToTimeType(string timeInString) {
    size_t seperationId = timeInString.find(':');
    string hourInString = timeInString.substr(0, seperationId);
    time_type hour = std::stod(hourInString);
    string minuteInString = timeInString.substr(seperationId + 1, timeInString.size() - hourInString.size() - 1);
    time_type minute = std::stod(minuteInString);
    return hour * 60 + minute;
}

ElectricVehicle *Simulator::parseElectricVehicle(std::ifstream &file) {
    ElectricVehicle *load = new ElectricVehicle();
    load->setType(ELECTRIC_VEHICLE);
    
    // read attributes
    string str;
    file >> str;
    while (str.compare("</electricVehicle>")) {
        if (str.compare("<locationBusName>") == 0) {
            Bus *locationBus = parseBusPointer(file, "locationBusName");
            load->setLocationBus(locationBus);
        }
        else if (str.compare("<name>") == 0) {
            string name = parseValue<string>(file, "name");
            load->setName(name);
        }
        else if (str.compare("<phase>") == 0) {
            string phase = parseValue<string>(file, "phase");
            load->setPhase(phase);
        }
        else if (str.compare("<maxRate>") == 0) {
            double maxRate = parseValue<double>(file, "maxRate");
            load->_maxChargingRate = maxRate;
        }
        else if (str.compare("<capacity>") == 0) {
            double capacity = parseValue<double>(file, "capacity");
            load->_futureEnergyRequest = capacity;
        }
        else if (str.compare("<arrivalTime>") == 0) {
            string arrivalTimeInString = parseValue<string>(file, "arrivalTime");
            time_type plugInTime = timeStringToTimeType(arrivalTimeInString);
            int slotId = int(plugInTime / _slowControlPeriodInMinutes);
            if (slotId * _slowControlPeriodInMinutes < plugInTime) {
                slotId ++;
            }
            plugInTime = slotId * _slowControlPeriodInMinutes;
            load->_plugInTime = plugInTime;
        }
        else if (str.compare("<duration>") == 0) {
            string durationInString = parseValue<string>(file, "duration");
            time_type duration = timeStringToTimeType(durationInString);
            int numberOfSlots = int(duration / _slowControlPeriodInMinutes);
            if (numberOfSlots * _slowControlPeriodInMinutes < duration)
                numberOfSlots ++;
            duration = numberOfSlots * _slowControlPeriodInMinutes;
            load->_deadline = duration;
        }
        file >> str;
    }
    
    load->initPhaseIndicesInLocationBus();
    load->_deadline += load->_plugInTime;
    return load;
}
