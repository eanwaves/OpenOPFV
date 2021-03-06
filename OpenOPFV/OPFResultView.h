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
 *
 * Project OpenOPFV - Optimal Power Flow Visualizer – 7/20/2019
 * Module OPFResultView.h
 *
 ***********************************************************************/

#import <Cocoa/Cocoa.h>
#import "Simulator.h"

struct PlotData {
    time_type _time;
    float _value;
    PlotData(time_type time, float value) : _time(time), _value(value) {}
};

@interface OPFResultView : NSView {
    vector<PlotData> _dataArray;
}

- (void)displayNumbers:(PlotData)data;

- (void)clear;

@end
