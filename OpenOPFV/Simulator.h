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
 * Module Simulator.h
 *
 ***********************************************************************/

#ifndef __OptimalPowerFlowVisualization__Simulator__
#define __OptimalPowerFlowVisualization__Simulator__

#include "DataType.h"
#include "FutureData.h"
#include "LoadPredictor.h"
#include "NetworkModel.h"
#include "NetworkControl.h"
#include "EventQueue.h"
#include "PhotoVoltaic.h"
#include "ElectricVehicle.h"

class Simulator {
private:
    /******************************
     simulator description
     ******************************/
    time_type _currentTimeInMinutes;        // current time in minutes
    FutureData _futureData;                 // store certain future data in memory
    LoadPredictor _loadPredictor;           // predict the base load for model predictive control
    NetworkModel _networkModel;             // real-time real data stays here, real power flow run here
    NetworkControl _networkControl;         // control data stays here, control algorithm run here
    EventQueue _eventQueue;                 // event queue that maintains data flow
    
    
    /******************************
     data source
     ******************************/
    
    // input data specifications
    string _inputFolderName;                // name of the input folder that stores network data
    string _nextLoadFileName;               // name of the file, that stores the next piece of load data
    int _numberOfSlotPerLoadFile;           // number of time slots per load file
    time_type _loadFileSlotLengthInMinutes; // length of slot length in minutes
    
    // simulation length and control objective
    int _simulationHorizonInDays;           // length of the simulation in days
    ControlObjective _controlObjective;     // objective of control
    
    // fast control specification
    bool _enableFastControl;                    // set to true if want to do fast time scale control
    time_type _fastControlPeriodInSeconds;      // the period of fast control, in seconds
    bool _enablePhotoVoltaicFastControl;        // set to true if control photovoltaic inverters in fast control
    bool _enableElectricVehicleFastControl;     // set to true if control electric vehicles in fast control
    bool _enablePoolPumpFastControl;            // set to true if control pool pumps in fast control
    bool _enableAirConditionerFastControl;      // set to true if control air conditioners in fast control
    bool _enableCapacitorFastControl;           // set to true if control capacitors in fast control
    
    // slow control specification
    bool _enableSlowControl;                    // set to true if want to do slow time scale control
    time_type _slowControlPeriodInMinutes;      // the period of slow control, in minutes
    time_type _predictionWindowInHours;         // the length of prediction window, in hours
    bool _enablePhotoVoltaicSlowControl;        // set to true if control photovoltaic inverters in slow control
    bool _enableElectricVehicleSlowControl;     // set to true if control electric vehicles in slow control
    bool _enablePoolPumpSlowControl;            // set to true if control pool pumps in slow control
    bool _enableAirConditionerSlowControl;      // set to true if control air conditioners in slow control
    bool _enableCapacitorSlowControl;           // set to true if control capacitors in slow control
    
    bool _stop;                                 // set to true if aiming to terminate simulation
    
    
public:
    /******************************
     basic functions
     ******************************/
    
    // default constructor
    Simulator();
    
    // copy constructor
    Simulator(const Simulator &simulator);
    
    // release allocated spaces
    void clear();
    
    // deconstructor
    ~Simulator();
    
    // assignment
    virtual void operator=(const Simulator &simulator);
    
    // print
    friend ostream &operator<<(ostream &cout, const Simulator &simulator);
    void printSubstationPowerInjectionOverHorizon(ostream &cout);
    
    
    /******************************
     basic functions
     ******************************/
    
    // getter functions
    time_type currentTimeInMinutes() const;
    FutureData *futureData();
    LoadPredictor *loadPredictor();
    NetworkModel *networkModel();
    NetworkControl *networkControl();
    EventQueue *eventQueue();
    
    string inputFolderName() const;
    string nextLoadFileName() const;
    int numberOfSlotPerLoadFile() const;
    time_type loadFileSlotLengthInMinutes() const;
    
    int simulationHorizonInDays() const;
    ControlObjective controlObjective() const;
    
    bool enableFastControl() const;
    time_type fastControlPeriodInSeconds() const;
    bool enablePhotoVoltaicFastControl() const;
    bool enableElectricVehicleFastControl() const;
    bool enablePoolPumpFastControl() const;
    bool enableAirConditionerFastControl() const;
    bool enableCapacitorFastControl() const;
    
    bool enableSlowControl() const;
    time_type slowControlPeriodInMinutes() const;
    time_type predictionWindowInHours() const;
    bool enablePhotoVoltaicSlowControl() const;
    bool enableElectricVehicleSlowControl() const;
    bool enablePoolPumpSlowControl() const;
    bool enableAirConditionerSlowControl() const;
    bool enableCapacitorSlowControl() const;
    
    bool stop() const;
    
    // setter functions
    void setCurrentTimeInMinutes(const time_type &currentTimeInMinutes);
    
    void setInputFolderName(const string &inputFolderName);
    void setNextLoadFileName(const string &nextLoadFileName);
    void setNumberOfSlotPerLoadFile(const int &numberOfSlotPerLoadFile);
    void setLoadFileSlotLengthInMinutes(const time_type &loadFileSlotLengthInMinutes);
    
    void setSimulationHorizonInDays(const int &simulationHorizonInDays);
    void setControlObjective(const ControlObjective &controlObjective);
    
    void setEnableFastControl(const bool &enableFastControl);
    void setFastControlPeriodInSeconds(const time_type &fastControlPeriodInSeconds);
    void setEnablePhotoVoltaicFastControl(const bool &enablePhotoVoltaicFastControl);
    void setEnableElectricVehicleFastControl(const bool &enableElectricVehicleFastControl);
    void setEnablePoolPumpFastControl(const bool &enablePoolPumpFastControl);
    void setEnableAirConditionerFastControl(const bool &enableAirConditionerFastControl);
    void setEnableCapacitorFastControl(const bool &enableCapacitorFastControl);
    
    void setEnableSlowControl(const bool &enableSlowControl);
    void setSlowControlPeriodInMinutes(const time_type &slowControlPeriodInMinutes);
    void setPredictionWindowInHours(const time_type &predictionWindowInHours);
    void setEnablePhotoVoltaicSlowConotrol(const bool &enablePhotoVoltaicSlowControl);
    void setEnableElectricVehicleSlowControl(const bool &enableElectricVehicleSlowControl);
    void setEnablePoolPumpSlowControl(const bool &enablePoolPumpSlowControl);
    void setEnableAirConditionerSlowControl(const bool &enableAirConditionerSlowControl);
    void setEnableCapacitorSlowControl(const bool &enableCapacitorSlowControl);
    
    void setStop(const bool &stop);
    
    
    /******************************
     initialization functions
     ******************************/
    
    // network model: load network data
    // future data: load future data
    // network controller: set up controllers for devices
    // event queue: initialize event queue (ev arrival and departure, fast control and slow control scheduling, update network status)
    // load predicator: initialize load predicator
    void initialize();
    
    // load network topology (buses and lines) using "network.txt"
    void initNetworkTopology();
    
    // network model: load device data
    // event queue: initialize ev arrival and departure
    // future data: load future data
    void initLoadData();
    void addLoadData();
    void setNextLoadFile();
    
    // network controller: initialize controller
    void initNetworkControl();
    
    // load predicator
    void initLoadPredicator();
    
    // event queue: initialize fast and slow control scheduling and network status update
    void initControlSchedule();
    
    
    /******************************
     other functions
     ******************************/
    
    // execute an event on top of the heap
    EventReturnType executeEvent();
    
    // update networkModel to currentTime using information from futureData and networkControl
    void updateToTime(time_type time);
    
    // get the current objective value
    double computeObjectiveValue();
    
    
    /******************************
     parser functions
     ******************************/
    
    // parse an attribute of type T, of a single entry
    template <class T>
    T parseValue(std::ifstream &file,
                 const string &attributeName);
    
    // parse an attribute of type T, of length numEntry
    template <class T>
    vector<T> parseValueArray(std::ifstream &file,
                              const int &numEntry,
                              const string &attributeName);
    
    // parse a complex type square matrix
    SquareMatrix<complex_type> parseComplexSquareMatrix(std::ifstream &file,
                                                        Element *element,
                                                        const string &attributeName);
    
    // parse a complex type column vector
    ColumnVector<complex_type> parseComplexColumnVector(std::ifstream &file,
                                                        Element *element,
                                                        const string &attributeName);
    
    // parse a real type column vector
    ColumnVector<double> parseRealColumnVector(std::ifstream &file,
                                               Element *element,
                                               const string &attributeName);
    
    // parse to find the pointer of a bus
    Bus *parseBusPointer(std::ifstream &file,
                         const string &attributeName);
    
    // parse coordinate
    Coordinate parseCoordinate(std::ifstream &file);
    
    // parse admittance
    SquareMatrix<complex_type> parseAdmittance(std::ifstream &file,
                                               Bus *bus);
    
    // parse a derived bus type
    Bus *parseSubstation(std::ifstream &file);      // substation
    Bus *parseBus(std::ifstream &file);             // bus
    Bus *parseHouse(std::ifstream &file);           // house
    
    // parse impedance
    SquareMatrix<complex_type> parseImpedance(std::ifstream &file,
                                              Line *line);
    
    // parse a derived line type
    Line *parseLine(std::ifstream &file);           // parse a line
    
    // parse complex power consumption
    ColumnVector<complex_type> parsePower(std::ifstream &file,
                                          Load *load);
    
    // parse a derived load data type
    time_type timeStringToTimeType(string timeInString);
    BaseLoadData *parseBaseLoadData(std::ifstream &file,
                                    Load *load);            // base load
    PhotoVoltaicData *parsePhotoVoltaicData(std::ifstream &file);   // photovoltaic
    
    // parse a derived load type
    Load *parseBaseLoad(std::ifstream &file,
                        vector<LoadData *> &loadDataArray);         // base load
    PhotoVoltaic *parsePhotoVoltaic(std::ifstream &file,
                                    vector<LoadData *> &loadDataArray);     // photovoltaic
    ElectricVehicle *parseElectricVehicle(std::ifstream &file);                // electric vehicle
};

#endif /* defined(__OptimalPowerFlowVisualization__Simulator__) */
