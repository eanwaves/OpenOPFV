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
 * Module OPFNetworkView.m
 *
 ***********************************************************************/

#import "OPFNetworkView.h"

@implementation OPFNetworkView

- (void)awakeFromNib {
    [self clear];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // set background color to white
    [[NSColor whiteColor] set];
    [NSBezierPath fillRect:[self bounds]];
    
    // draw network topology
    if (_simulator) {
        // compute transform
        NSAffineTransform *transform = [NSAffineTransform transform];
        NSRect selfRect = [self bounds];
        [transform scaleXBy:selfRect.size.width yBy:selfRect.size.height];
        [transform translateXBy:selfRect.origin.x yBy:selfRect.origin.y];
        
        // draw lines
        NSBezierPath *path = [NSBezierPath bezierPath];
        for (int lineId = 0; lineId < _simulator->networkModel()->numberOfLines(); lineId ++) {
            Line *line = _simulator->networkModel()->getLineByIndex(lineId);
            Coordinate fromBusCoordinate = line->fromBus()->coordinate();
            Coordinate toBusCoordinate = line->toBus()->coordinate();
            [path moveToPoint:NSMakePoint(fromBusCoordinate._x, fromBusCoordinate._y)];
            [path lineToPoint:NSMakePoint(toBusCoordinate._x, toBusCoordinate._y)];
        }
        [[NSColor blackColor] set];
        [path transformUsingAffineTransform:transform];
        [path stroke];
        
        // draw buses
        for (int layerId = 0; layerId < _busInLayers.size(); layerId ++) {
            float width = 1.0 / (_busInLayers.size() + 1);
            for (int busId = 0; busId < _busInLayers[layerId].size(); busId ++) {
                float height = 1.0 / (_busInLayers[layerId].size() + 1);
                float edgeLength = width < height ? width : height;
                NSBezierPath *busPath = [NSBezierPath bezierPath];
                Bus *bus = _busInLayers[layerId][busId];
                // set location according to coordinate
                Coordinate center = bus->coordinate();
                // double scale = std::sqrt(_simulator->networkModel()->numberOfBuses() / 40.0);
                [busPath appendBezierPathWithArcWithCenter:NSMakePoint(center._x, center._y) radius:(edgeLength/2.0) startAngle:0.0 endAngle:360.0];
                // set color according to voltage
                ColumnVector<complex_type> voltage = bus->voltage();
                double meanVoltage = 0.0;
                for (int phaseId = 0; phaseId < voltage.size(); phaseId ++) {
                    meanVoltage += std::sqrt( std::norm( voltage[phaseId] ) );
                }
                meanVoltage /= voltage.size();
                double middle = 1.00;
                CGFloat red = 20 * (meanVoltage - middle);
                CGFloat green = 20 * (middle - meanVoltage);
                CGFloat blue = 1.0;
                if (meanVoltage > middle) {
                    blue = 1.0 - red;
                }
                else if (meanVoltage < middle) {
                    blue = 1.0 - green;
                }
                CGFloat alpha = 0.5;
                [[NSColor colorWithCalibratedRed:red green:green blue:blue alpha:alpha] set];
                [busPath transformUsingAffineTransform:transform];
                [busPath fill];
            }
        }
    }
}

- (void)setNetwork:(Simulator *)simulator {
    _simulator = simulator;
    
    // compute _busInLayers
    _busInLayers.clear();
    vector<Line *> lines = _simulator->networkModel()->lines();
    vector<Bus *> layer;
    layer.push_back(lines[0]->fromBus());
    _busInLayers.push_back(layer);
    vector<Bus *> nextLayer;
    
    int lineId = 0;
    while (lineId < lines.size()) {
        Line *line = lines[lineId];
        Bus *fromBus = line->fromBus();
        
        bool inNextLayer = false;
        for (int i = 0; i < layer.size(); i ++) {
            if (layer[i] == fromBus) {
                inNextLayer = true;
                break;
            }
        }
        
        if (inNextLayer) {
            nextLayer.push_back(line->toBus());
            lineId ++;
        }
        else {
            layer = nextLayer;
            nextLayer.clear();
            _busInLayers.push_back(layer);
        }
    }
    _busInLayers.push_back(nextLayer);
    
    // get coordinates
    for (int layerId = 0; layerId < _busInLayers.size(); layerId ++) {
        float x = 1.0 / (_busInLayers.size() + 1) * (layerId+1);
        for (int busId = 0; busId < _busInLayers[layerId].size(); busId ++) {
            float y = 1.0 / (_busInLayers[layerId].size() + 1) * (busId + 1);
            _busInLayers[layerId][busId]->setCoordinate(Coordinate(x, y));
        }
    }
}

- (void)displayNetwork {
    [self setNeedsDisplay:YES];
    // [self drawRect:[self bounds]];
    // if (int(_simulator->currentTimeInMinutes()) % 20 == 0) {
        [[NSRunLoop currentRunLoop] runMode: NSDefaultRunLoopMode beforeDate: [NSDate date]];
    // }
}

- (void)clear {
    _simulator = NULL;
    _busInLayers.clear();
    [self setNeedsDisplay:YES];
    [self drawRect:[self bounds]];
}

@end
