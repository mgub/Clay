#ifndef APPLICATION_H
#define APPLICATION_H

/* User includes (#include below this line is not maintained by Processor Expert) */

#include "Message.h"

#include <stdio.h>
#include <stdlib.h>

#include "Utilities/UUID.h"

//#include "Drivers/LEDs.h"
//#include "Drivers/PCA9552.h"
//#include "Drivers/MPU9250.h"
#include "Drivers/WiFi/WiFi.h"

#include "Clock.h"

#include "Action.h"
#include "Event.h"
#include "Timeline.h"

//#include "meshTest.h"
#include "Bootloader.h"
#include "Mesh.h"
#include "RGB_LED.h"
#include "GPIO.h"
#include "Clock.h"
#include "MPU9250.h"
#include "LEDs.h"
#include "Events.h"
//#include "RGBDemo.h"
#include "ADC0.h"

#include "Buzzer.h"
#include "PowerOn.h"
#include "Button.h"

#include "Interactive_Assembly.h"

/**
 * Initializes the device.
 */
void Initialize ();

/**
 * Runs the application. This should be called only after calling Initialize().
 */
void Application ();

#endif
