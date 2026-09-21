#include <Adafruit_MLX90640.h>
#include <ESP32Servo.h>

//servo home values 
float homePan   = 95.0;   //slightly higher than 90 so it looks closer to face at the start
float homeTilt  = 70.0;   //middle is supposed to be 130 (270/2) but the actual middle when attempting to write to the servo was 70

//thermcal camera values
float humanTempMin = 26.0;  //min temp we want so it only tracks humans and now blank walls
float humanTempMax = 38.0;  //max temp we want so it only tracks humans and not fire/lights
int searchDelay    = 5000;  //if nobody in sight within 5 seconds, start searching

// tracking speeds
float trackingSpeed = 1.0;    
float smoothingFactor = 0.6;  
float scanSpeed = 2.0;      

//servo min and max tilt and pan
//servo pan was supposed to be 0-270, we found out that 0-125 was making it move from 0-270, really didnt change much besides starting position.
int panLimitMin = 5;    
int panLimitMax = 180;  
int tiltLimitMin = 0;   
int tiltLimitMax = 125;

//servo pins
const int panPin  = 21; //D9 on arduino nano esp32
const int tiltPin = 18; //D10 on arduino nano esp32

//pin A4 and A5 are used for SCL and SDA pins for thermal camera

Adafruit_MLX90640 mlx;
float frame[768]; 

Servo panServo;
Servo tiltServo;

float currentPan   = homePan;
float currentTilt  = homeTilt;
unsigned long lastSeenTime = 0;
unsigned long edgePauseTime = 0; 
bool isSearching = false;
bool scanClockwise = true;
bool isAtEdge = false;

void setup() {
  Serial.begin(921600); 
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  panServo.setPeriodHertz(50);
  tiltServo.setPeriodHertz(50);
  panServo.attach(panPin, 500, 2500);
  tiltServo.attach(tiltPin, 500, 2400);


// when power on, make servo go to starting position.
  panServo.write((int)currentPan);
  tiltServo.write((int)currentTilt);

  if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    while (1);
  }

  mlx.setMode(MLX90640_CHESS);
  mlx.setResolution(MLX90640_ADC_18BIT); 
  mlx.setRefreshRate(MLX90640_16_HZ); 
  Wire.setClock(1000000); 

  lastSeenTime = millis();
}

void loop() {
  if (mlx.getFrame(frame) != 0) return;

  float maxTemp = -100.0;
  int maxIndex = -1;

  // mlx90640 thermca camera trakcing
  for (int i = 0; i < 768; i++) {
    if (frame[i] >= humanTempMin && frame[i] <= humanTempMax) {
      if (frame[i] > maxTemp) {
        maxTemp = frame[i];
        maxIndex = i;
      }
    }
  }

  // tracking
  if (maxIndex != -1) {
    isSearching = false;
    isAtEdge = false;
    lastSeenTime = millis(); 
    
    int hotX = maxIndex % 32; 
    int hotY = maxIndex / 32;
    float errorX = hotX - 15.5; // center of pixel array (0-23)
    float errorY = hotY - 11.5; //center of pixel array (0-31)

    if (abs(errorX) > 1.2) {
      currentPan += (errorX * trackingSpeed * smoothingFactor);
    }
    if (abs(errorY) > 1.2) {
      currentTilt -= (errorY * trackingSpeed * smoothingFactor);
    }
  } 
  else if (millis() - lastSeenTime > searchDelay) {
    
    // searching
    isSearching = true;
    currentTilt = homeTilt; 

//set tilt to home tilt and move pan from 0-270 to look for human.
    if (isAtEdge) {
      if (millis() - edgePauseTime > 100) {
        isAtEdge = false;
        scanClockwise = !scanClockwise; 
      }
    } else {
      if (scanClockwise) {
        currentPan += scanSpeed;
        if (currentPan >= panLimitMax) {
          currentPan = panLimitMax;
          isAtEdge = true;
          edgePauseTime = millis();
        }
      } else {
        currentPan -= scanSpeed;
        if (currentPan <= panLimitMin) {
          currentPan = panLimitMin;
          isAtEdge = true;
          edgePauseTime = millis();
        }
      }
    }
  }

  // constraints
  currentPan  = constrain(currentPan, panLimitMin, panLimitMax);
  currentTilt = constrain(currentTilt, tiltLimitMin, tiltLimitMax);
  
  panServo.write((int)currentPan);
  tiltServo.write((int)currentTilt);

  // data to computer/python
  for (int i = 0; i < 768; i++) {
    Serial.print(frame[i], 1);
    if (i < 767) Serial.print(",");
  }
  Serial.println(); 
}