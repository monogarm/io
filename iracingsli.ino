#include <TM1638.h>

TM1638 module(8, 7, 9);

word leds [17] = {0, 256, 768, 1792, 3840, 7936, 16128, 32512, 65280, 1, 3, 7, 15, 31, 63, 127, 255};

byte buttons, oldbuttons, page;
byte gear, spd, shift, rpm_h, rpm_l, engine;
int fuel;
word rpm;
boolean changed, blinkrpm;
unsigned long milstart, milstart2 = 0;

void setup() {
	Serial.begin(9600);

	oldbuttons = 0;
	page = 0;
        changed = false;
        blinkrpm = false;
}

void loop() {
	if (Serial.available() > 0) {
		if (Serial.available() > 8) {
			if (Serial.read() == 255) {
				gear = Serial.read();
				spd = Serial.read();
				rpm_h = Serial.read();
				rpm_l = Serial.read();
				fuel = Serial.read();
				shift = Serial.read();
				engine = Serial.read();

				rpm = (rpm_h << 8) | rpm_l;
			}
		}
	}

	buttons = module.getButtons();
	if (buttons != 0) {
		if (buttons != oldbuttons) {
			oldbuttons = buttons;
			page = buttons;
                        module.clearDisplay();

                        switch (page) {
                            case 8:
                                module.setDisplayToString("Gear spd", 0, 0);
                                break;
                            case 16:
                                module.setDisplayToString("Engine", 0, 0);
                                break;
                            case 32:
                                module.setDisplayToString("Fuel pct", 0, 0);
                                break;
                        }

                        changed = true;
                        milstart = millis();
		}
	}
	
        if (changed == false) {
        	switch (page) {
        		case 8:
                                if (gear == 0) 
        			    module.setDisplayToString("r", 0, 0);
                                else if (gear == 1)
                                    module.setDisplayToString("n", 0, 0);
                                else
                                    module.setDisplayToString(String(gear - 1, DEC), 0, 0);
                                
                                if (spd < 100)    
                                    module.clearDisplayDigit(7, false);   
        			module.setDisplayToString(String(spd, DEC), 0, 5);
        			break;
        		case 16:
        			module.setDisplayToDecNumber(rpm, 0, false);
        			break;
        		case 32:
                                String fuelstr = String(fuel, DEC);
        			module.setDisplayToString(String(fuelstr + " pct"), 0, 0);
        			break;
        	}
        } else {
            if ((millis() - milstart) > 2000) {
                changed = false;
                module.clearDisplay();
            }
        }
        
        if ((engine & 0x10) == 0) {
            if (shift == 16) {
                if ((millis() - milstart2) > 50) {
                    if (blinkrpm == false) {
                        module.setLEDs(0x0000);
                        blinkrpm = true;
                    } else {
                        module.setLEDs(leds[shift]);
                        blinkrpm = false;
                    }
                    milstart2 = millis();
                }
            } else {
                module.setLEDs(leds[shift]);
            }
        } else {
            if ((millis() - milstart2) > 200) {
                if (blinkrpm == false) {
                    module.setLEDs(0x0000);
                    blinkrpm = true;
                } else {
                    module.setLEDs(0xFF00);
                    blinkrpm = false;
                }
                milstart2 = millis();
            }
        }
}
