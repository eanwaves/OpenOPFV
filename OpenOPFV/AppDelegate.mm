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
 * Module AppDelegate.m
 *
 ***********************************************************************/

#import "AppDelegate.h"
#import "Simulator.h"
#import "OPFNetworkView.h"
#import "OPFResultView.h"

@interface AppDelegate () {
    Simulator _simulator;
}

@property (weak) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSButton *networkName;
@property (weak) IBOutlet NSPopUpButton *simulationHorizonInDays;
@property (weak) IBOutlet NSPopUpButton *controlObjective;

@property (weak) IBOutlet NSButton *enableFastControl;
@property (weak) IBOutlet NSPopUpButton *fastControlPeriodInSeconds;
@property (weak) IBOutlet NSButton *enablePhotoVoltaicFastControl;
@property (weak) IBOutlet NSButton *enableElectricVehicleFastControl;
@property (weak) IBOutlet NSButton *enablePoolPumpFastControl;
@property (weak) IBOutlet NSButton *enableAirConditionerFastControl;
@property (weak) IBOutlet NSButton *enableCapacitorFastControl;

@property (weak) IBOutlet NSButton *enableSlowControl;
@property (weak) IBOutlet NSPopUpButton *slowControlPeriodInMinutes;
@property (weak) IBOutlet NSPopUpButton *predictionWindowInHours;
@property (weak) IBOutlet NSButton *enablePhotoVoltaicSlowControl;
@property (weak) IBOutlet NSButton *enableElectricVehicleSlowControl;
@property (weak) IBOutlet NSButton *enablePoolPumpSlowControl;
@property (weak) IBOutlet NSButton *enableAirConditionerSlowControl;
@property (weak) IBOutlet NSButton *enableCapacitorSlowControl;

@property (weak) IBOutlet OPFNetworkView *networkView;
@property (weak) IBOutlet NSButton *stop;
@property (weak) IBOutlet OPFResultView *resultView;
@property (weak) IBOutlet NSButton *run;

- (IBAction)selectNetwork:(id)sender;
- (IBAction)selectSimulationHorizonInDays:(id)sender;
- (IBAction)selectControlObjective:(id)sender;

- (IBAction)checkEnableFastControl:(id)sender;
- (IBAction)selectFastControlPeriodInSeconds:(id)sender;
- (IBAction)checkEnablePhotoVoltaicFastControl:(id)sender;
- (IBAction)checkEnableElectricVehicleFastControl:(id)sender;
- (IBAction)checkEnablePoolPumpFastControl:(id)sender;
- (IBAction)checkEnableAirConditionerFastControl:(id)sender;
- (IBAction)checkEnableCapacitorFastControl:(id)sender;

- (IBAction)checkEnableSlowControl:(id)sender;
- (IBAction)selectSlowControlPeriodInMinutes:(id)sender;
- (IBAction)selectPredictionWindowInHours:(id)sender;
- (IBAction)checkEnablePhotoVoltaicSlowControl:(id)sender;
- (IBAction)checkEnableElectricVehicleSlowControl:(id)sender;
- (IBAction)checkEnablePoolPumpSlowControl:(id)sender;
- (IBAction)checkEnableAirConditinoerSlowControl:(id)sender;
- (IBAction)checkEnableCapacitorSlowControl:(id)sender;

- (IBAction)run:(id)sender;
- (IBAction)stop:(id)sender;

- (void)clear;

@end

@implementation AppDelegate

@synthesize networkName = _networkName;
@synthesize simulationHorizonInDays = _simulationHorizonInDays;
@synthesize controlObjective = _controlObjective;

@synthesize enableFastControl = _enableFastControl;
@synthesize fastControlPeriodInSeconds = _fastControlPeriodInSeconds;
@synthesize enablePhotoVoltaicFastControl = _enablePhotoVoltaicFastControl;
@synthesize enableElectricVehicleFastControl = _enableElectricVehicleFastControl;
@synthesize enablePoolPumpFastControl = _enablePoolPumpFastControl;
@synthesize enableAirConditionerFastControl = _enableAirConditionerFastControl;
@synthesize enableCapacitorFastControl = _enableCapacitorFastControl;

@synthesize enableSlowControl = _enableSlowControl;
@synthesize slowControlPeriodInMinutes = _slowControlPeriodInMinutes;
@synthesize predictionWindowInHours = _predictionWindowInHours;
@synthesize enablePhotoVoltaicSlowControl = _enablePhotoVoltaicSlowControl;
@synthesize enableElectricVehicleSlowControl = _enableElectricVehicleSlowControl;
@synthesize enablePoolPumpSlowControl = _enablePoolPumpSlowControl;
@synthesize enableAirConditionerSlowControl = _enableAirConditionerSlowControl;
@synthesize enableCapacitorSlowControl = _enableCapacitorSlowControl;

@synthesize stop = _stop;
@synthesize run = _run;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    [self selectSimulationHorizonInDays:self];
    [self selectControlObjective:self];
    
    [self checkEnableFastControl:self];
    [self selectFastControlPeriodInSeconds:self];
    [self checkEnablePhotoVoltaicFastControl:self];
    [self checkEnableElectricVehicleFastControl:self];
    [self checkEnablePoolPumpFastControl:self];
    [self checkEnableAirConditionerFastControl:self];
    [self checkEnableCapacitorFastControl:self];
    
    [self checkEnableSlowControl:self];
    [self selectSlowControlPeriodInMinutes:self];
    [self selectPredictionWindowInHours:self];
    [self checkEnablePhotoVoltaicSlowControl:self];
    [self checkEnableElectricVehicleSlowControl:self];
    [self checkEnablePoolPumpSlowControl:self];
    [self checkEnableAirConditinoerSlowControl:self];
    [self checkEnableCapacitorSlowControl:self];
    
    [_run setEnabled:NO];
    [_stop setEnabled:NO];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    [self clear];
}

- (IBAction)selectNetwork:(id)sender {
    // set up file path selection panel
    NSOpenPanel* directorySelection = [NSOpenPanel openPanel];
    [directorySelection setCanChooseFiles:NO];
    [directorySelection setCanChooseDirectories:YES];
    [directorySelection setAllowsMultipleSelection:NO];
    
    // set folder name when click on "OK"
    if ( [directorySelection runModal] == NSFileHandlingPanelOKButton ) {
        NSURL *directoryURL = [directorySelection URL];
        NSString *folderName = [directoryURL path];
        _simulator.setInputFolderName([folderName UTF8String]);
        [_networkName setTitle:[directoryURL lastPathComponent]];
        
        // init network topology and draw topology
        [self clear];
        _simulator.initialize();
        [_networkView setNetwork:&_simulator];
        [_networkView displayNetwork];
        [_run setEnabled:YES];
        [_stop setEnabled:YES];
    }
}

- (IBAction)selectSimulationHorizonInDays:(id)sender {
    NSString *simulationHorizon = [_simulationHorizonInDays titleOfSelectedItem];
    if ([simulationHorizon isEqualToString:@"1 day"])
        _simulator.setSimulationHorizonInDays(1);
    else if ([simulationHorizon isEqualToString:@"2 days"])
        _simulator.setSimulationHorizonInDays(2);
    else if ([simulationHorizon isEqualToString:@"3 days"])
        _simulator.setSimulationHorizonInDays(3);
    else {
        NSLog(@"Unexpected simulation horizon!");
        _simulator.setSimulationHorizonInDays(1);
    }
}

- (IBAction)selectControlObjective:(id)sender {
    NSString *controlObjective = [_controlObjective titleOfSelectedItem];
    if ([controlObjective isEqualToString:@"Minimize l2 norm"])
        _simulator.setControlObjective(MINIMIZE_L2_NORM);
    else {
        NSLog(@"Unexpected control objective!");
        _simulator.setControlObjective(MINIMIZE_L2_NORM);
    }
}

- (IBAction)checkEnableFastControl:(id)sender {
    NSInteger enableFastControl = [_enableFastControl state];
    if (enableFastControl == NSOffState)
        _simulator.setEnableFastControl(false);
    else
        _simulator.setEnableFastControl(true);
}

- (IBAction)selectFastControlPeriodInSeconds:(id)sender {
    NSString *fastControlPeriod = [_fastControlPeriodInSeconds titleOfSelectedItem];
    if ([fastControlPeriod isEqualToString:@"10 seconds"])
        _simulator.setFastControlPeriodInSeconds(10.0);
    else if ([fastControlPeriod isEqualToString:@"1 minute"])
        _simulator.setFastControlPeriodInSeconds(60.0);
    else if ([fastControlPeriod isEqualToString:@"5 minutes"])
        _simulator.setFastControlPeriodInSeconds(300.0);
    else {
        NSLog(@"Unexpected fast control period");
        _simulator.setFastControlPeriodInSeconds(10.0);
    }
}

- (IBAction)checkEnablePhotoVoltaicFastControl:(id)sender {
    NSInteger enablePVControl = [_enablePhotoVoltaicFastControl state];
    if (enablePVControl == NSOffState)
        _simulator.setEnablePhotoVoltaicFastControl(false);
    else
        _simulator.setEnablePhotoVoltaicFastControl(true);
}

- (IBAction)checkEnableElectricVehicleFastControl:(id)sender {
    NSInteger enableEVControl = [_enableElectricVehicleFastControl state];
    if (enableEVControl == NSOffState)
        _simulator.setEnableElectricVehicleFastControl(false);
    else
        _simulator.setEnableElectricVehicleFastControl(true);
}

- (IBAction)checkEnablePoolPumpFastControl:(id)sender {
    NSInteger enablePPControl = [_enablePoolPumpFastControl state];
    if (enablePPControl == NSOffState)
        _simulator.setEnablePoolPumpFastControl(false);
    else
        _simulator.setEnablePoolPumpFastControl(true);
}

- (IBAction)checkEnableAirConditionerFastControl:(id)sender {
    NSInteger enableACControl = [_enableAirConditionerFastControl state];
    if (enableACControl == NSOffState)
        _simulator.setEnableAirConditionerFastControl(false);
    else
        _simulator.setEnableAirConditionerFastControl(true);
}

- (IBAction)checkEnableCapacitorFastControl:(id)sender {
    NSInteger enableCapControl = [_enableCapacitorFastControl state];
    if (enableCapControl == NSOffState)
        _simulator.setEnableCapacitorFastControl(false);
    else
        _simulator.setEnableCapacitorFastControl(true);
}

- (IBAction)checkEnableSlowControl:(id)sender {
    NSInteger enableSlowControl = [_enableSlowControl state];
    if (enableSlowControl == NSOffState)
        _simulator.setEnableSlowControl(false);
    else
        _simulator.setEnableSlowControl(true);
}

- (IBAction)selectSlowControlPeriodInMinutes:(id)sender {
    NSString *slowControlPeriod = [_slowControlPeriodInMinutes titleOfSelectedItem];
    if ([slowControlPeriod isEqualToString:@"5 minutes"])
        _simulator.setSlowControlPeriodInMinutes(5);
    else if ([slowControlPeriod isEqualToString:@"15 minutes"])
        _simulator.setSlowControlPeriodInMinutes(15);
    else if ([slowControlPeriod isEqualToString:@"1 hour"])
        _simulator.setSlowControlPeriodInMinutes(60);
    else {
        NSLog(@"Unexpected slow control period!");
        _simulator.setSlowControlPeriodInMinutes(15);
    }
}

- (IBAction)selectPredictionWindowInHours:(id)sender {
    NSString *predictionWindow = [_predictionWindowInHours titleOfSelectedItem];
    if ([predictionWindow isEqualToString:@"4 hours"])
        _simulator.setPredictionWindowInHours(4);
    else if ([predictionWindow isEqualToString:@"12 hours"])
        _simulator.setPredictionWindowInHours(12);
    else if ([predictionWindow isEqualToString:@"24 hours"])
        _simulator.setPredictionWindowInHours(24);
    else {
        NSLog(@"Unexpected prediciton window!");
        _simulator.setPredictionWindowInHours(24);
    }
}

- (IBAction)checkEnablePhotoVoltaicSlowControl:(id)sender {
    NSInteger enablePVControl = [_enablePhotoVoltaicSlowControl state];
    if (enablePVControl == NSOffState)
        _simulator.setEnablePhotoVoltaicSlowConotrol(false);
    else
        _simulator.setEnablePhotoVoltaicSlowConotrol(true);
}

- (IBAction)checkEnableElectricVehicleSlowControl:(id)sender {
    NSInteger enableEVControl = [_enableElectricVehicleSlowControl state];
    if (enableEVControl == NSOffState)
        _simulator.setEnableElectricVehicleSlowControl(false);
    else
        _simulator.setEnableElectricVehicleSlowControl(true);
}

- (IBAction)checkEnablePoolPumpSlowControl:(id)sender {
    NSInteger enablePPControl = [_enablePoolPumpSlowControl state];
    if (enablePPControl == NSOffState)
        _simulator.setEnablePoolPumpSlowControl(false);
    else
        _simulator.setEnablePoolPumpSlowControl(true);
}

- (IBAction)checkEnableAirConditinoerSlowControl:(id)sender {
    NSInteger enableACControl = [_enableAirConditionerSlowControl state];
    if (enableACControl == NSOffState)
        _simulator.setEnableAirConditionerSlowControl(false);
    else
        _simulator.setEnableAirConditionerSlowControl(true);
}

- (IBAction)checkEnableCapacitorSlowControl:(id)sender {
    NSInteger enableCapControl = [_enableCapacitorSlowControl state];
    if (enableCapControl == NSOffState)
        _simulator.setEnableCapacitorSlowControl(false);
    else
        _simulator.setEnableCapacitorSlowControl(true);
}

- (IBAction)run:(id)sender {
    [_run setEnabled:NO];
    
    EventReturnType executionResult;
    
    do {
        executionResult = _simulator.executeEvent();
        if (executionResult == NEED_PLOTTING) {
            time_type time = _simulator.currentTimeInMinutes();
            float value = _simulator.computeObjectiveValue();
            [_resultView displayNumbers:PlotData(time, value)];
            [_networkView displayNetwork];
        }
    }
    while (executionResult != EVENT_QUEUE_EMPTY);
    
    [_stop setEnabled:YES];
}

- (IBAction)stop:(id)sender {
    [self clear];
    [_run setEnabled:NO];
    [_stop setEnabled:NO];
    [_networkName setTitle:@"Select a network"];
}

- (void)clear {
    _simulator.clear();
    [_networkView clear];
    [_resultView clear];
}

@end
