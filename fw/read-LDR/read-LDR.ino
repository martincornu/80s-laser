/**************************************************************************/
/*! 
    @file     read-LDR
    @author   Martin CORNU
    @date     16/01/22
    @version  1.0

This program read an analog value from an LDR which can be in contact with a laser.
If the analog value is below the defined threshold it means that the laser is in contact.
If laser is in contact, then deactivate an EM by setting low the corresponding digital output.
An emergency button also allows the EM to be deactivated manually.
*/
/**************************************************************************/

//#define DEBUG

#define LED_INT_PIN         13        // internal led pin
#define LDR_PIN             A0        // pin where we connected the LDR and the resistor
#define LDR_LASER_THR       450       // below this threshold means laser is in contact
#define LDR_LASER_THR_MS    2000      // time in ms to stay below the threshold to consider laser is in contact
#define EM_PIN              2         // electromagnet pin
#define BTN_EMERGENCY_PIN   3         // emergency button pin
#define DELAY_LOOP_MS       250

uint8_t g_u8EMDefaultState = HIGH;    //EM ON at boot (locked)

int g_s32ButtonState;             // the current reading from the input pin
int g_s32LastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long g_u64LastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long g_u64DebounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);                   // sets serial port for communication
    Serial.println("Program start..");      // print the value to the serial port 
  #endif
  
  pinMode(LED_INT_PIN, OUTPUT);
  digitalWrite(LED_INT_PIN, LOW);

  pinMode(EM_PIN, OUTPUT);
  digitalWrite(EM_PIN, g_u8EMDefaultState);

  pinMode(LDR_PIN, INPUT);
  pinMode(BTN_EMERGENCY_PIN, INPUT);
}

void loop() {
  uint16_t l_u16LDRValue = 0;           // result of reading the analog pin
  static uint32_t l_u32Timer = 0;              // time below laser thr in ms
  
  BtnEmergencyManagement();
  
  l_u16LDRValue = analogRead(LDR_PIN);  // read the value from the LDR
  #ifdef DEBUG
  Serial.println(l_u16LDRValue);        // print the value to the serial port
  #endif

  // laser detected
  if (l_u16LDRValue < LDR_LASER_THR)
  {
    #ifdef DEBUG
    Serial.println(l_u16LDRValue);        // print the value to the serial port
    #endif
    
    // we stayed more than THR ms with laser detected
    if (l_u32Timer >= LDR_LASER_THR_MS)
    {
      digitalWrite(EM_PIN, !g_u8EMDefaultState);
      digitalWrite(LED_INT_PIN, HIGH);
      #ifdef DEBUG
      Serial.println("Success !");
      #endif
      
      while(1);
    }else{
     l_u32Timer += DELAY_LOOP_MS; 
    }
  }else{
    // laser not detected then restart timer
    l_u32Timer = 0;
  }
  
  delay(DELAY_LOOP_MS);
}

void BtnEmergencyManagement(void) {
   int l_s32Reading = 0;                 // btn emergency reading
  
  // read the state of the switch into a local variable:
  l_s32Reading = digitalRead(BTN_EMERGENCY_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (l_s32Reading != g_s32LastButtonState) {
    // reset the debouncing timer
    g_u64LastDebounceTime = millis();
  }

  if ((millis() - g_u64LastDebounceTime) > g_u64DebounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (l_s32Reading != g_s32ButtonState) {
      g_s32ButtonState = l_s32Reading;

      // button state is high
      if (g_s32ButtonState == HIGH) {
        digitalWrite(EM_PIN, !g_u8EMDefaultState);
        digitalWrite(LED_INT_PIN, HIGH);
        #ifdef DEBUG
        Serial.println("Success !");
        #endif
        while(1);
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the g_s32LastButtonState:
  g_s32LastButtonState = l_s32Reading;
}
