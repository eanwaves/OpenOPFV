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
 * Module OPFResultView.m
 *
 ***********************************************************************/

#import "OPFResultView.h"

@implementation OPFResultView

- (void)awakeFromNib {
    _dataArray = vector<PlotData>();
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // set background color to white
    [[NSColor whiteColor] set];
    [NSBezierPath fillRect:[self bounds]];
    
    // draw data array
    if (!_dataArray.empty()) {
        
        // get range of the array
        int numberOfPoints = int( _dataArray.size() );
        float xMin = _dataArray.front()._time;
        float xMax = _dataArray.back()._time;
        float yMin = _dataArray[0]._value;
        float yMax = _dataArray[0]._value;
        for (int i = 1; i < numberOfPoints; i ++) {
            float tmp = _dataArray[i]._value;
            if (tmp < yMin) yMin = tmp;
            if (tmp > yMax) yMax = tmp;
        }
        if (yMin > 0.0) yMin = 0.0;
        
        // create the path
        NSBezierPath *path = [NSBezierPath bezierPath];
        float x = (_dataArray[0]._time - xMin) / (xMax - xMin + 0.1);
        float y = (_dataArray[0]._value - yMin) / (yMax - yMin + 0.1);
        [path moveToPoint:NSMakePoint(x, y)];
        
        for (int i = 1; i < numberOfPoints; i ++) {
            x = (_dataArray[i]._time - xMin) / (xMax - xMin + 0.1);
            y = (_dataArray[i]._value - yMin) / (yMax - yMin + 0.1);
            [path lineToPoint:NSMakePoint(x, y)];
        }
        
        // scale to frame
        NSAffineTransform *transform = [NSAffineTransform transform];
        NSRect selfRect = [self bounds];
        [transform scaleXBy:selfRect.size.width yBy:selfRect.size.height];
        [transform translateXBy:selfRect.origin.x yBy:selfRect.origin.y];
        [path transformUsingAffineTransform:transform];
        
        // draw number array
        [[NSColor blackColor] set];
        [path stroke];
    }
}

- (void)displayNumbers:(PlotData)data {
    NSLog(@"time = %7.2f, value = %6.2f", data._time, data._value);
    if (_dataArray.empty() || data._time > _dataArray.back()._time)
        _dataArray.push_back(data);
    else
        _dataArray.back() = data;
    [self setNeedsDisplay:YES];
    // if (_dataArray.size() % 100 == 0)
        [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate: [NSDate date]];
}

- (void)clear {
    _dataArray.clear();
    [self setNeedsDisplay:YES];
    [self drawRect:[self bounds]];
}

@end
