
/* reflowOven_V3.ino
 * 
 * June 19th, 2017
 * by: Chris Ernst
 * 
 * This sketch is built with the intent of controlling a two-element toaster for reflowing printed circuit boards.
 * 
 * This program is broken into 5 separate steps
 * Some steps will be included in the setup portion of the sketch,
 * while the rest will remain in the loop.
 * 
 * The 5 separate steps are:
 *    1. Startup/Welcome 
 *    2. Preheating Sequence
 *    3. Soaking Process
 *    4. Reflow Process
 *    5. Cooldown/Completion
 *    
 *    
 *    
 * Note: Any numerical values given are in reference to Kester EP256 leaded solder paste.
 * 
 * 
 *
 * In further detail, these steps will complete the following tasks:
 * 
 *    1. Startup/Welcome:  This will give initialize the LCD, I/O pins, and give the user a welcome message.
 *        This will then be followed by a prompt that asks for the 'pushButton' to be pressed to begin preheating.
 *        It will contain a function that sits and waits for the pushButton==1, which will trigger the Preheating sequence.
 *        
 *    2. Preheating Sequence:  This step will heat the oven up to the 'soakStartTemp'. Once the 'soakStartTemp' is acheived,
 *        the oven will hold this temperature and display, via the LCD, "Please load board" in row1, and "Press to Begin" in row 2
 *        and the pushbutton should be pressed to begin the soaking process. (Note: since this is a process with many variables, 
 *        there is no duration specified.)
 *        
 *    3. Soaking Process: Once the pushbutton has been pressed, the oven will begin a 'timer' that will last 'soakDuration' in length
 *        (typically 30-60s, 120s max) and it will attempt to reach the 'reflowStartTemp' in that time period.
 *        
 *    4. Reflow Process:  Once the 'thermocouple.readCelsius()' has exceeded 'reflowStartTemp', the Reflow Process will begin automatically. This 
 *        process will last 'reflowDuration' in length (typically 45-75s; 90s max). This process will complete once 
 *        'thermocouple.readCelsius()' has exceeded 'peakTemperature'. The following condition must be true:'soakingDuration' + 'reflowDuration' = timeToPeak <=330 seconds
 *        
 *    5. Cooldown/Completion:  Once the 'peakTemperature' has been achieved, the cooldown cycle will commence.
 *        If the 'timeToPeak' is greater than 240s, the heaters will turn off completely and the blue lights will flash slowly. 
 *        Once 'timer' is greater than 300s, the blue leds will flash rapidly, and the message will say 'Please Open Door'
 *        
 *        
 *        
 *        
 *                    Kester EP256 Solder Profile     
 * 
 * 
 * 
 * Temperature (Degree Celcius)                 Magic Happens Here!
 * 220-|                                               x  x  
 *     |                                            x        x
 *     |                                         x              x
 *     |                                      x                    x
 * 185-|                                   x                          x
 *     |                              x    |                          |   x   
 *     |                         x         |                          |       x
 *     |                    x              |                          |
 * 150-|               x                   |                          |
 *     |             x |                   |                          |
 *     |           x   |                   |                          | 
 *     |         x     |                   |                          | 
 *     |       x       |                   |                          | 
 *     |     x         |                   |                          |
 *     |   x           |                   |                          |
 * 30 -| x             |                   |                          |
 *     |<     --      >|<    30 - 60 s    >|<       45 - 75 s        >|
 *     | Preheat Stage |   Soaking Stage   |       Reflow Stage       | Cool
 *  0  |_ _ _ _ _ _ _ _|_ _ _ _ _ _ _ _ _ _|_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ 
 *                                                                Time (Seconds)       
 *                                                                
 *         (the above graph was adapted from Whizoo's reflow profile)
 *  
 * 
 * 
 * 
 * 
 *  Variable List
 *  
 *  - buttonPin: The pin the pushbutton is connected to.
 *  - buttonState: the boolean state of the pushbutton.
 *  - counter: A running timer beginning from when the push button is pressed, after the oven has preheated. Timing the total length the board has been in the oven.
 *  - thermocouple.readCelsius(): The current temperature reading of the thermocouple.
 *  - soakStartTemp: This is the temperature the board is placed in at.
 *  - soakDuration: the length of the soak portion of the process.
 *  - reflowStartTemp: This is the temperature the reflow sequence begins at.
 *  - reflowDuration: the length of the reflow portion of the process. 
 *  - peakTemperature: This is the peak temperature the oven will reach; after this, the cooldown will occur.
 *  - timeToPeak = soakingDuration + reflowDuration:  This is the length of time from when the board is placed in the oven until peakTemperature is reached.
 *  - topHeater: the upper heating element of the oven and red led.
 *  - bottomHeater: the lower heating element of the oven and red led.
 *  - blueLeds: the blue anodes of the LEDs in the toaster 
 *  - lcdContrast: this is the pin the contrast is connected to PWM variable on the lcd, which sets the contrast.
 *      Some communication pins:
 *  - thermoCLK: Clock line
 *  - thermoCS: Slave Select
 *  - thermoDO: Master Input Slave Output
 *  
 *  
 *  
 */




  //  Include the relevant libraries
    #include <max6675.h>
    #include <LiquidCrystal.h>
    #include <Wire.h>
    #include <Event.h>
    #include <Timer.h>
    
    Timer t;


  // Pin declarations and initializations:
    int buttonPin = 7;
    int topHeater = 8;
    int bottomHeater = 9;
    int blueLeds = 10;
    int lcdContrast = 6;
    
    // the pins required for the SPI interface from the thermocouple>>MAX6675>>arduino
    int thermoDO = A3;
    int thermoCS = A4;
    int thermoCLK = A5;
    
    MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
    int vccPin = 3;
    int gndPin = 2;
    
    // the required pins for the 16x2 LCD
    LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


  // Constant declarations and initializations:
    const int soakStartTemp = 150;
    int soakDuration = 0;
    const int reflowStartTemp = 185;
    int reflowDuration = 0;
    const int peakTemperature = 205;


  // Variable declarations:
    int timeToPeak = soakDuration + reflowDuration;
    int buttonState = 0;
    int currentTemp;
    int printTemp;



void setup() {




  Serial.begin(9600);

  pinMode(vccPin, OUTPUT); 
  digitalWrite(vccPin, HIGH);
  
  pinMode(gndPin, OUTPUT); 
  digitalWrite(gndPin, LOW);

  pinMode(buttonPin, INPUT);

  // Set top heater as output and turn off
  pinMode(topHeater, OUTPUT); 
  digitalWrite(topHeater, LOW);

  // Set bottom heater as output and turn off
  pinMode(bottomHeater, OUTPUT); 
  digitalWrite(bottomHeater, LOW);

  // Turn on the blue leds located on the front of the oven
  pinMode(blueLeds, OUTPUT); 
  analogWrite(blueLeds, 50);

  // Set LCD contrast pin as output, and send 60/255 PWM 24% duty cycle to make text visible
  // It should be noted this value is inconsistent from circuit to circuit and you may have to play with for the text to display properly
  pinMode(lcdContrast, OUTPUT); 
  analogWrite(lcdContrast, 60);

  // Let the LCD warm up
  delay(1000);

  // Initialize the LCD
  lcd.begin(16, 2);

  // Clear the screen
  lcd.clear();
  delay(100);
  
  // This is Step 1. Welcome/Startup

  // Set the cursor and display the Welcome Message
  lcd.setCursor(4, 0);
  lcd.print("Welcome!");
  lcd.noCursor();

  // wait for MAX chip to stabilize
  delay(4000);

  // Clear LCD and display Start
  lcd.clear();


}


void loop() {
  
  // Read the state of the pushbutton
  buttonState = digitalRead(buttonPin);

   
  // Check if the pushbutton is pressed

  if(buttonState == 1) {

    // This is Step 2. Preheating Sequence:

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Starting Preheat");
    delay(1000);
  
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Get Ready");
    delay(1000);
  
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Three");
    delay(500);
  
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("Two");
    delay(500);
  
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("One");
    delay(500);
    
    analogWrite(blueLeds, 0);

    while(thermocouple.readCelsius() < soakStartTemp){
    
      digitalWrite(topHeater, HIGH);
      digitalWrite(bottomHeater, HIGH);
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Pre Heating");
      lcd.setCursor(3, 1);
      lcd.print(thermocouple.readCelsius());
      Serial.println(thermocouple.readCelsius());
      lcd.print(" degC");
      lcd.noCursor();
      delay(1000);
    }

    delay(50);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hold to Reflow");
    lcd.setCursor(0, 1);
    lcd.print(thermocouple.readCelsius());
    lcd.print(" degC");
    lcd.noCursor();

        
        
    buttonState = digitalRead(buttonPin);
    t.every(10, buttonState);
    
  if(buttonState == 1 && thermocouple.readCelsius() > soakStartTemp){

    // This is Step 3. Soaking Process:

    while(thermocouple.readCelsius() < reflowStartTemp){
      digitalWrite(topHeater, HIGH);
      digitalWrite(bottomHeater, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Soaking Process");
      lcd.setCursor(0, 1);
      lcd.print(thermocouple.readCelsius());
      Serial.println(thermocouple.readCelsius());
      lcd.print(" degC");
      //lcd.setCursor(13, 1);
      //lcd.print(soakDuration);
      lcd.noCursor();
      delay(1000);
      //soakDuration=soakDuration++; 
    }
    
       // This is Step 4. Reflow Process:
  
    while(thermocouple.readCelsius() < peakTemperature){
      digitalWrite(topHeater, HIGH);
      digitalWrite(bottomHeater, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reflowing");
      lcd.setCursor(0, 1);
      lcd.print(thermocouple.readCelsius());
      Serial.println(thermocouple.readCelsius());
      lcd.print(" degC");
      //lcd.setCursor(8, 1);
      //lcd.print(reflowDuration);
      lcd.noCursor();
      delay(1000);
      //reflowDuration=reflowDuration++;
    }
    

    // This is Step 5. Cooldown Process:

    digitalWrite(topHeater, LOW);
    digitalWrite(bottomHeater, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Complete");
    delay(50);
    //lcd.setCursor(0, 1);
    //lcd.print(timeToPeak);
    for(int i=0;i<300;i++){
      analogWrite(blueLeds, 50);
      delay(100);
      analogWrite(blueLeds, 0);
      delay(100);
      Serial.println(thermocouple.readCelsius());

    }
    lcd.noDisplay();      
  }
    
    // hold temp at soakStartTemp
    
    if(thermocouple.readCelsius()<soakStartTemp){
      digitalWrite(topHeater, HIGH);
      digitalWrite(bottomHeater, HIGH);
      }
    else{
      digitalWrite(topHeater, LOW); 
      digitalWrite(bottomHeater, LOW);
      }


  } // initial if button pressed bracket
  else{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press to Preheat");
    lcd.setCursor(3, 1);
    printTemp = lcd.print(thermocouple.readCelsius());
    Serial.println(thermocouple.readCelsius());
    lcd.print(" degC");
    lcd.noCursor();
    delay(500);
  }



} // void loop bracket
