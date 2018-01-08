
// #define MY_CORE_ONLY
#define MY_DEBUG
#define MY_RADIO_NRF24
#define ENABLED_SERIAL
// #define MY_RF24_CHANNEL 125
// #define MY_REPEATER_FEATURE
#define MY_BAUD_RATE 115200
// #define RF24_PA_LEVEL_GW 83
// #define MY_RF24_PA_LEVEL 83 // RF24_PA_HIGH
#define MY_RF24_PA_LEVEL RF24_PA_LOW
#define MY_NODE_ID 111
#define USEACK false

#include <MySensors.h>

#define PWR_MOISTURE 7
#define PIN_MOISTURE A2
#define CHILD_ID_MOISTURE 1
#define BASEVOLTAGE 5000
#define INITCOUNT 10
#define SLEEPTIME 900000

MyMessage msg_moisture(CHILD_ID_MOISTURE, V_LEVEL);
uint8_t counter = 0;


void presentation()  {
  sendSketchInfo("Moisture Sensor", "1.0", USEACK);
  present(CHILD_ID_MOISTURE, S_MOISTURE, "Moisture", USEACK); 
  Serial.println("Presentation done");
}

void setup() {
  pinMode(PWR_MOISTURE, OUTPUT);
  digitalWrite(PWR_MOISTURE, LOW);
  pinMode(PIN_MOISTURE, INPUT);

  // ADMUX = (0<<REFS1) | (0<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
  Serial.println("Setup done");
}


void loop() {
  digitalWrite(PWR_MOISTURE, HIGH);
  delay(5000);
  long v_moist = analogRead(PIN_MOISTURE);
  digitalWrite(PWR_MOISTURE, LOW);
  Serial.print("Moisture: ");
  Serial.println(v_moist);
  send(msg_moisture.set(v_moist), USEACK);
  getBattery();
  if(counter <= INITCOUNT) {
    smartSleep(20);
    counter++;
  } else {
    smartSleep(SLEEPTIME);
  }
}



void getBattery() {
  long pct;
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  Serial.print("Voltage: ");
  Serial.println(result);
  pct = result * 100.0 / BASEVOLTAGE;
  Serial.print("Battery percent: ");
  Serial.println(pct);
  if(pct > 100) {
    pct = 100;
  }
  sendBatteryLevel(pct, USEACK);
}

void getBattery2() {
  uint8_t level;
  const long InternalReferenceVoltage = 1083L;

  ADMUX = (0<<REFS1) | (0<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
  ADCSRA |= _BV( ADSC );
 
  while( ( (ADCSRA & (1<<ADSC)) != 0 ) );
  level = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L;

  
  Serial.print("Battery: ");
  Serial.println(level);
  // sendBatteryLevel(level, USEACK);
}




