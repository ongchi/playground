//
//  ShapeType.swift
//  GeometryFighter
//
//  Created by ongchi on 9/18/16.
//  Copyright © 2016 hii. All rights reserved.
//

import Foundation

public enum ShapeType: Int {
    case Box = 0
    case Sphere
    case Pyramid
    case Torus
    case Capsule
    case Cylinder
    case Cone
    case Tube
    
    static func random() -> ShapeType {
        let maxvalue = Tube.rawValue
        let rand = arc4random_uniform(UInt32(maxvalue+1))
        return ShapeType(rawValue: Int(rand))!
    }
}
