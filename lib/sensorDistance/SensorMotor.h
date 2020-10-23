/*
  SensorMotor.h - Library for move the sensor head


*/
#ifndef SensorMotor_h
#define SensorMotor_h

#include <Arduino.h>
#include <Servo.h>

#define  START_ANGLE    25.0
#define  END_ANGLE      160.0
#define ANGLE_RANGE     (END_ANGLE - START_ANGLE)
#define TIME_TO_TRAVEL  605
#define ANGULAR_SPEED   (ANGLE_RANGE / TIME_TO_TRAVEL)

class SensorMotor
{
  private:

  Servo servo;
  float servoIncrement = 0;
  // float servoPos = 0;
  float servoPos = START_ANGLE;
  float servoPosMin = 25;
  float servoPosMax = 160;
  int reverseIncrement = 1;
  float servoSpeed = 0;
  float angleAccuracy = 0;

  float servoAngle = servoPos;

  int valPot = 0;

  long previousServoMillis = 0;
  long previousLoopMainMillis = 0;
  int timeSinceLastMove = 0;
  float time_spent = 0;


  public:
  void sensorMotor(void);
  void sensorMotorSetup(float angle_accuracy, float servo_speed_ms){
    servoIncrement = angle_accuracy;
    angleAccuracy = angle_accuracy;
    servoSpeed = servo_speed_ms;
    servoPos = servoPosMin;
    servo.attach(0); // Attaching Servo to D3
    // servo.write(servoPos);
    servo.write(0);
    delay(2000);

    // for (int i = 0; i <= 180; i++) {
    //   servo.write(i);
    //   Serial.print(" angle: ");
    //   Serial.println(i);
    //   delay(500);
    // }
    //
    // for (int i = 180; i>= 0; i--) {
    //   servo.write(i);
    //   Serial.print(" angle: ");
    //   Serial.println(i);
    //   delay(500);
    // }
    servo.write(0);
    delay(1000);




    // for (int i = 180; i <= 180; i--) {
    //   valPot = analogRead(0);
    //   valPot = map(valPot, 0, 1023, 580, 750);
    //   servo.write(25);
    //   delay(valPot);
    //   servo.write(160);
    //   delay(valPot);
    //     Serial.print(" valPot: ");
    //     Serial.println(valPot);
    //
    // }


    // delay(10000);

  }
    void moveServo(void){
      unsigned long currentLoopMillis = millis();
      timeSinceLastMove = currentLoopMillis - previousServoMillis;

      float time_between_servo_writes = float(angleAccuracy)*(ANGULAR_SPEED);

      if (timeSinceLastMove >= TIME_TO_TRAVEL){
        previousServoMillis = currentLoopMillis;

        if (servoPos >= END_ANGLE){
          servoPos = START_ANGLE;
        }
        else if (servoPos <= START_ANGLE){
          servoPos = END_ANGLE;
        }
        servo.write(servoPos);
      }


      previousLoopMainMillis = currentLoopMillis;
    };

    float sensorMotorAngle(void){
      unsigned long currentLoopMillis = millis();

      moveServo();

      if (servoIncrement > 0){
        servoAngle = START_ANGLE + (timeSinceLastMove)*(ANGULAR_SPEED);
      } else {
        servoAngle = END_ANGLE - (timeSinceLastMove)*(ANGULAR_SPEED);
      }

      Serial.print("servoAngle: ");
      Serial.print(servoAngle);

      Serial.print(" ANGULAR_SPEED: ");
      Serial.print(ANGULAR_SPEED);
      Serial.print(" timeSinceLastMove: ");
      Serial.print(timeSinceLastMove);
      Serial.print(" servoPos: ");
      Serial.println(servoPos);


      return servoAngle;

    };


    float sensorMotorAngle2(void){
      unsigned long currentLoopMillis = millis();

      float timeSinceLastMove = currentLoopMillis-previousServoMillis;
      float loop_delay = currentLoopMillis-previousLoopMainMillis;
      time_spent = time_spent + loop_delay;

      valPot = analogRead(0);
      valPot = map(valPot, 0, 1023, 0, 1500);
      servoSpeed = valPot;

      // int time_between_servo_writes = servoSpeed * angleAccuracy/60;
      // float time_between_servo_writes = angleAccuracy / (servoSpeed/60);
      // float time_between_servo_writes = angleAccuracy / (servoSpeed/60);
      float time_between_servo_writes = float(angleAccuracy)*(ANGULAR_SPEED);

      // int angleIncrement = loop_delay*60/(servoSpeed);
      // servoAngle += (angleIncrement*reverseIncrement);
      if (reverseIncrement > 0)
        // servoAngle = servoPosMin + (time_spent_sum+time_spent)*60/(servoSpeed);
        // servoAngle = servoPosMin + (time_spent)*60/(servoSpeed);
        servoAngle = servoPosMin + (time_spent)*(ANGULAR_SPEED);

      else
        // servoAngle = servoPosMax - (time_spent_sum+time_spent)*60/(servoSpeed);
        // servoAngle = servoPosMax - (time_spent)*60/(servoSpeed);
        servoAngle = servoPosMax - (time_spent)*(ANGULAR_SPEED);



      if (timeSinceLastMove >= time_between_servo_writes){
        previousServoMillis = currentLoopMillis;
        servoPos += servoIncrement;
        servo.write(servoPos);
        // servoAngle = servoPos;

        if ((servoPos >= servoPosMax) || (servoPos <= servoPosMin)) // end of sweep
        {
          // reverse direction
          servoIncrement = -servoIncrement;
          reverseIncrement = -reverseIncrement;
          time_spent = 0;
        }
      }


      Serial.print("servoSpeed: ");
      Serial.print(servoSpeed);
      Serial.print(" time_between_servo_writes: ");
      Serial.print(time_between_servo_writes);
      Serial.print(" angle_accuracy: ");
      Serial.print(angleAccuracy);
      Serial.print(" loop_delay: ");
      Serial.print(loop_delay);
      Serial.print(" servoAngle: ");
      Serial.print(servoAngle);

      Serial.print(" time_spent: ");
      Serial.print(time_spent);
      Serial.print(" timeSinceLastMove: ");
      Serial.print(timeSinceLastMove);
      Serial.print(" servoPos: ");
      Serial.println(servoPos);

      previousLoopMainMillis = currentLoopMillis;
      return servoAngle;

    }
};

#endif
