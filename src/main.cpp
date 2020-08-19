#include <Arduino.h>
#include <WiFi.h>
#include <NewPing.h>
#include <FirebaseESP32.h>
#include <TaskScheduler.h>

//firebase Auth included for connecting with firebase
#define FIREBASE_HOST "esp32-flutter-project.firebaseio.com"
#define FIREBASE_AUTH "sMuaV86cJhMXpk3htAaXfk3xkveaZ6CcqrL9tuW3" 
//Ultrasonic Sensor HC-SR04
#define TRIGGER_PIN 4
#define ECHO_PIN 5
#define MAX_DISTANCE 400
#define Buzzer 33

//Define firebase Data object
FirebaseData firebaseData;
String path="/ESP32_Device";

// setting PWM properties
int freq=2000;
int channel=0;
uint resolution1=8;


// NewPing setup of pins and maximum distance
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 

const char* ssid = "SHAW-BDF72C";
const char* password = "Saavi2019";

//send distance data to firebase   
void senddistance();
Task task1_distance(1000, TASK_FOREVER, &senddistance); //scheduled 'send distance' task with 1s delay


// Scheduler to manage the tasks    
Scheduler runner; 
void initscheduler()
{
 runner.init();
 
runner.addTask(task1_distance);//add the task to the task scheduler
task1_distance.enable();     
}

void iniFirebase(); //function declaration

void setup() {
  
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println("Hello...");
  
  ledcSetup(channel,freq,resolution1); // configure buzzer PWM functionalitites
  ledcAttachPin(33,0);  // attach the channel to the GPIO to be controlled
  

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  iniFirebase();  //firebase function calling
  initscheduler(); 
}


void iniFirebase()
{
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //set database read timeout to 1 minute
  Firebase.setReadTimeout(firebaseData,1000*60);
  Firebase.setwriteSizeLimit(firebaseData,"tiny");
}

void loop() {
  //put your main code here, to run repeatedly:
  runner.execute();
   
}

void senddistance()
{
  double distance = sonar.ping_cm();
   if(distance > 0){
   Serial.print(distance);
   Serial.println("cm");
   }

   Firebase.setDouble(firebaseData,path+"/Distance/Data",distance);

  //control the buzzer using PWM
 ledcWrite(channel, 125);
    if (distance > 0 && distance <400){
    ledcWriteTone(0,7000);
  }
  else {
     ledcWriteTone(0,0);
  }
}
