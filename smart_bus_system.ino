#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

Servo doorServo;

// PING))) Sensors
const int entrySensor = 8;
const int exitSensor = 6;

// Output Devices
const int buzzer = 10;
const int ledPin = A0;

// Passenger Details
int passengerCount = 0;
int maxCapacity = 10;

// Sensor Flags
bool entryDetected = false;
bool exitDetected = false;

// Function to Measure Distance
long getDistance(int signalPin)
{
  pinMode(signalPin, OUTPUT);

  digitalWrite(signalPin, LOW);
  delayMicroseconds(2);

  digitalWrite(signalPin, HIGH);
  delayMicroseconds(5);

  digitalWrite(signalPin, LOW);

  pinMode(signalPin, INPUT);

  long duration = pulseIn(signalPin, HIGH);

  long distance = duration * 0.034 / 2;

  return distance;
}

void setup()
{
  // LCD Initialization
  lcd.begin(16,2);

  // Output Pins
  pinMode(buzzer, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Servo Initialization
  doorServo.attach(13);

  // Welcome Message
  lcd.setCursor(0,0);
  lcd.print("SMART BUS");
  lcd.setCursor(0,1);
  lcd.print("SYSTEM");

  delay(3000);

  lcd.clear();

  // Door Initially Open
  doorServo.write(0);
}

void loop()
{
  int entryDistance = getDistance(entrySensor);
  int exitDistance = getDistance(exitSensor);

  // =========================
  // ENTRY DETECTION
  // =========================
  if(entryDistance < 15 && !entryDetected)
  {
    // Allow Entry Only If Bus Not Full
    if(passengerCount < maxCapacity)
    {
      passengerCount++;

      lcd.clear();
      lcd.print("Passenger IN");

      delay(1000);
    }
    else
    {
      lcd.clear();
      lcd.print("ENTRY BLOCKED");
      lcd.setCursor(0,1);
      lcd.print("BUS FULL!");

      delay(1000);
    }

    entryDetected = true;
  }

  // Reset Entry Trigger
  if(entryDistance > 15)
  {
    entryDetected = false;
  }

  // =========================
  // EXIT DETECTION
  // =========================
  if(exitDistance < 15 && !exitDetected)
  {
    if(passengerCount > 0)
    {
      passengerCount--;

      lcd.clear();
      lcd.print("Passenger OUT");

      delay(1000);
    }

    exitDetected = true;
  }

  // Reset Exit Trigger
  if(exitDistance > 15)
  {
    exitDetected = false;
  }

  // =========================
  // OCCUPANCY CALCULATION
  // =========================
  int occupancy = (passengerCount * 100) / maxCapacity;

  // =========================
  // LCD MAIN DISPLAY
  // =========================
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Count:");
  lcd.print(passengerCount);

  lcd.setCursor(10,0);
  lcd.print(occupancy);
  lcd.print("%");

  lcd.setCursor(0,1);

  // =========================
  // OVERLOAD CONDITION
  // =========================
  if(passengerCount >= maxCapacity)
  {
    lcd.print("BUS FULL!");

    digitalWrite(buzzer, HIGH);

    digitalWrite(ledPin, HIGH);

    // Lock Door
    doorServo.write(90);
  }
  else
  {
    lcd.print("Door Open");

    digitalWrite(buzzer, LOW);

    digitalWrite(ledPin, LOW);

    // Open Door
    doorServo.write(0);
  }

  delay(500);
}