# Function Generator
The signal was generated with an AD9833 module. Digital potentiometer X0c102 (1k ohms) was use to attenuate signal, followed by an op amp TL072 as inverting amplifier, with gain=2. 
A rotary encoder was used to control the wave form, frequency and amplitude.

## Limitations
The sine wave looks ok at 500k htz, amplitude range 50 mv to 1.2 v. The square wave on my AD9833 module does not look good. 

## Compatible Hardware
Diagram of the circuit:
https://www.circuitlab.com/circuit/sdt7s7j26694/function-generator/

* Arduino nano
* EC11 rotary encoder
* LCD display (16x2)
* LM7805 for 5v voltage regulator 
* X9c102 1k ohms digital pot as voltage divider to attenuate signal
* TL072 op amp for fixed gain 
* AD9833 programmable signal generator

The enclosure was printed with a 3D printer. 

## License

This code is released under the MIT License.
