//
//  ViewController.swift
//  Calculator
//
//  Created by ongchi on 3/13/16.
//  Copyright © 2016 hii. All rights reserved.
//

import UIKit

class ViewController: UIViewController {
    @IBOutlet weak var display: UILabel!
    @IBOutlet weak var history: UILabel!

    var userIsInTheMiddleOfTypingANumber = false
    var operateButtonPressed = false
    
    @IBAction func appendDigit(sender: UIButton) {
        let digit = sender.currentTitle!

        if userIsInTheMiddleOfTypingANumber {
            switch digit{
            case ".":
                if !display.text!.containsString(".") {
                    display.text = display.text! + digit
                }
            case "π":
                display.text = "π"
            default:
                display.text = display.text! + digit
            }
        } else {
            display.text = digit
            userIsInTheMiddleOfTypingANumber = true
        }
    }

    @IBAction func operate(sender: UIButton) {
        operateButtonPressed = true
        let operation = sender.currentTitle!
        history.text = history.text! + operation + " "
        switch operation{
        case "×": performOperation({ $0 * $1 })
        case "÷": performOperation({ $1 / $0 })
        case "+": performOperation({ $0 + $1 })
        case "−": performOperation({ $1 - $0 })
        case "sin": performOperation({ sin($0) })
        case "cos": performOperation({ cos($0) })
        case "√": performOperation({ sqrt($0) })
        case "C":
            operateButtonPressed = false
            operandStack.removeAll()
            display.text = "0"
            history.text = ""
        default: break
        }
    }
    
    @nonobjc func performOperation(operation: (Double, Double) -> Double) {
        if operandStack.count >= 2 {
            displayValue = operation(operandStack.removeLast(), operandStack.removeLast())
            enter()
        }
    }
    
    @nonobjc func performOperation(operation: Double -> Double) {
        if operandStack.count >= 1 {
            displayValue = operation(operandStack.removeLast())
            enter()
        }
    }
    
    var operandStack = Array<Double>()
    
    @IBAction func enter() {
        userIsInTheMiddleOfTypingANumber = false
        operandStack.append(displayValue)
        print("\(operandStack)")
        if operateButtonPressed == false {
            history.text = history.text! + display.text! + " "
        }
        operateButtonPressed = false
    }
    
    var displayValue: Double {
        get {
            if display.text == "π" {
                return M_PI
            } else {
                return NSNumberFormatter().numberFromString(display.text!)!.doubleValue
            }
        }
        set {
            display.text = "\(newValue)"
        }
    }
}

