# Iambic-CW-Keyer
Iambic CW Keyer - Arduino
  
 Read the Bencher Iambic Key, 
   plays tone on speaker, 
   lights LED, 
   writes to console, 
   keys the radio all without using delay()
 
 Turns on and off a light emitting diode(LED) connected to digital  
 pin 13, when paddle 1 or 2 or both pressed.
 
 The circuit:
 * Yellow wire to ground with 22K resistor to Red and 22K resisstor to Black
 * Red wire pin 12 - plug center contact
 * Green wire pin 11 - plug ground contact
 * Black wire pin 10 - plug tip contact
 * Tone appears on pin 9 - attach 8 ohm skeaker in series with 100 ohm resistor 
   to pin 9 and ground pin
 * LED attached from pin 13 to ground (on board)
 * pushbutton attached to pin 2 from +5V
 * Note: on most Arduinos there is already an LED on the board
 attached to pin 13.
 
 The dot duration is the basic unit of time measurement in code transmission.
 
 The duration of a dash is three times the duration of a dot. 
 Each dot or dash is followed by a short silence, equal to the dot duration. 
 The letters of a word are separated by a space equal to three dots (one dash), 
 and the words are separated by a space equal to seven dots. 
