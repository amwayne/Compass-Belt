// I2C device class (I2Cdev) demonstration Arduino sketch for MPU9150
// 1/4/2013 original by Jeff Rowberg <jeff@rowberg.net> at https://github.com/jrowberg/i2cdevlib
//          modified by Aaron Weiss <aaron@sparkfun.com>
//
// Changelog:
//     2011-10-07 - initial release
//     2013-1-4 - added raw magnetometer output

/* ============================================
I2Cdev device library code is placed under the MIT license
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;

#define DEVICE_OFFSET_FROM_FRONT_COUNTERCLOCKWISE 180
#define NUM_BUZZERS 6

int buzzer_ports[NUM_BUZZERS];
int buzzer_angle;


#define LED_PIN 13
bool blinkState = false;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // configure Arduino LED for
    pinMode(LED_PIN, OUTPUT);

    // define up pins for the 
    buzzer_ports[0] = 5;
    buzzer_ports[1] = 6;
    buzzer_ports[2] = 7;
    buzzer_ports[3] = 8;
    buzzer_ports[4] = 9;
    buzzer_ports[5] = 11;

    buzzer_angle = 360 / NUM_BUZZERS;

    // set up pins for buzzers
    for(int i = 0; i < NUM_BUZZERS; i++){
      pinMode(buzzer_ports[i], OUTPUT);
      digitalWrite(buzzer_ports[i], LOW);
    }
    
}

void loop() {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);
    
    // display tab-separated accel/gyro x/y/z values

    double mx_corr = (mx+21.5)*(500/223);
    double my_corr = (my-38.5)*(500/253);
    double mz_corr = (mz+160)*(500/246);
    Serial.print(mx_corr); Serial.print("\t");
    Serial.print(my_corr); Serial.print("\t");
    Serial.print(mz_corr); Serial.print("\t");
    
    int angle = (int) (atan2(mx_corr,mz_corr)*180/3.14);
    angle = usable_angle(angle);
    Serial.print(angle); Serial.print("\t");
    choose_buzzers_by_angle(angle);
    
    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}

int usable_angle(int in_angle){
    return 360 - (in_angle + 180 + DEVICE_OFFSET_FROM_FRONT_COUNTERCLOCKWISE) % 360;
}

void choose_buzzers_by_angle(int angle){
    int split_into = 4;
    int buzzer_num = angle / buzzer_angle;
    int relative_angle = angle % buzzer_angle;
    if(relative_angle < (buzzer_angle / split_into)){
      turn_On(buzzer_num, (buzzer_num + NUM_BUZZERS - 1) % NUM_BUZZERS);
    }
    else{
      if(relative_angle > (buzzer_angle * (split_into - 1) / split_into)){
        turn_On(buzzer_num, (buzzer_num + 1) % NUM_BUZZERS);
      }
      else{
        turn_On(buzzer_num, -1);
      }
    }
    Serial.println(buzzer_num); Serial.print("\t");
    
}

void turn_On(int buzzer_one, int buzzer_two){
    digitalWrite(buzzer_ports[buzzer_one], HIGH);
    if(buzzer_two >= 0){
        digitalWrite(buzzer_ports[buzzer_two], HIGH);
    }
    for(int i = 0; i < NUM_BUZZERS; i++){
        if(i != buzzer_one && i != buzzer_two) {
            digitalWrite(buzzer_ports[i], LOW);
        }
    }
}

