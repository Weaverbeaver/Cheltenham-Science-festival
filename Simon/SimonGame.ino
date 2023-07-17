/*This sketch is a simple version of the famous Simon Says game. You can use it and improved it adding
levels and everything you want to increase the diffuculty!
There are five buttons connected to 3, D1, D2, D3 and D4.
The buttons from 3 to D3 are used to insert the right sequence while D4 to start the game.
When a wrong sequence is inserted all the leds will blink for three time very fast otherwhise the
inserted sequence is correct.
Hardware needed:
5x pushbuttons
1x Blue led
1x Yellow led
1x Red led
1x Green Led
4x 1k resistors
4x 10k resisors
10x jumpers
*/

#include <ESP8266WiFi.h>
#include <Adafruit_AW9523.h>

Adafruit_AW9523 aw;

/*Put your SSID & Password*/
const char * ssid = ""; // Enter SSID here
const char * password = ""; //Enter Password here

const char * server = "";

WiFiClient client;

const int MAX_LEVEL = 8;
int sequence[MAX_LEVEL];
int your_sequence[MAX_LEVEL];
int level = 1;

int velocity = 1000;

bool game = true;

uint8_t LedPin1 = 1;
uint8_t LedPin2 = 2;
uint8_t LedPin3 = 3;
uint8_t LedPin4 = 4;
uint8_t MaxBrightness = 255;
uint8_t MinBrightness = 0;

void setup() {

  if (! aw.begin(0x58)) {
    Serial.println("AW9523 not found? Check wiring!");
    while (1) delay(10);  // halt forever
  }

  Serial.println("AW9523 found!");
  aw.pinMode(LedPin1, AW9523_LED_MODE); // set to constant current drive!
  aw.pinMode(LedPin2, AW9523_LED_MODE);
  aw.pinMode(LedPin3, AW9523_LED_MODE);
  aw.pinMode(LedPin4, AW9523_LED_MODE);

  Serial.begin(9600);
  Serial.println("beginning setup");

  Serial.println("Connecting to ");
  Serial.println(ssid);
  
    //connect to your local wi-fi network
    WiFi.begin(ssid, password);
  
    //check wi-fi is connected to wi-fi network
    int count = 0;
    while (WiFi.status() != WL_CONNECTED && count < 15) {
    Serial.print(".");
    count = count + 1;
    aw.analogWrite(LedPin1, MaxBrightness);
    delay(500);
    aw.analogWrite(LedPin1, MinBrightness);
    aw.analogWrite(LedPin2, MaxBrightness);
    delay(500);
    aw.analogWrite(LedPin2, MinBrightness);
    aw.analogWrite(LedPin3, MaxBrightness);
    delay(500);
    aw.analogWrite(LedPin3, MinBrightness);
    aw.analogWrite(LedPin4, MaxBrightness);
    delay(500);
    aw.analogWrite(LedPin4, MinBrightness);
    }
    aw.analogWrite(LedPin1, MaxBrightness);
    aw.analogWrite(LedPin2, MaxBrightness);
    aw.analogWrite(LedPin3, MaxBrightness);
    aw.analogWrite(LedPin4, MaxBrightness);
    delay(1000);
    aw.analogWrite(LedPin1, MinBrightness);
    aw.analogWrite(LedPin2, MinBrightness);
    aw.analogWrite(LedPin3, MinBrightness);
    aw.analogWrite(LedPin4, MinBrightness);

    if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    }
    else {
    Serial.println("");
    Serial.println("WiFi not connected");
    }

  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  
  wrong_sequence();

  aw.analogWrite(LedPin1, MinBrightness);
  aw.analogWrite(LedPin2, MinBrightness);
  aw.analogWrite(LedPin3, MinBrightness);
  aw.analogWrite(LedPin4, MinBrightness);

  Serial.print("Level ");
  Serial.println(level);
}

void loop() {
  if (level == 1)
    generate_sequence(); //generate a sequence;

  if (digitalRead(D4) == LOW || level != 1) //If start button is pressed or you're winning
  {
    show_sequence(); //show the sequence
    get_sequence(); //wait for your sequence
  }

  if (level == 8) {
    game_complete();
  aw.analogWrite(LedPin1, MaxBrightness);
  aw.analogWrite(LedPin2, MinBrightness);
  aw.analogWrite(LedPin3, MaxBrightness);
  aw.analogWrite(LedPin4, MinBrightness);
    delay(200);
  aw.analogWrite(LedPin1, MinBrightness);
  aw.analogWrite(LedPin2, MaxBrightness);
  aw.analogWrite(LedPin3, MinBrightness);
  aw.analogWrite(LedPin4, MaxBrightness);
    delay(200);
  aw.analogWrite(LedPin1, MaxBrightness);
  aw.analogWrite(LedPin2, MinBrightness);
  aw.analogWrite(LedPin3, MaxBrightness);
  aw.analogWrite(LedPin4, MinBrightness);
    delay(200);
  aw.analogWrite(LedPin1, MinBrightness);
  aw.analogWrite(LedPin2, MaxBrightness);
  aw.analogWrite(LedPin3, MinBrightness);
  aw.analogWrite(LedPin4, MaxBrightness);
    delay(200);
  aw.analogWrite(LedPin1, MinBrightness);
  aw.analogWrite(LedPin2, MinBrightness);
  aw.analogWrite(LedPin3, MinBrightness);
  aw.analogWrite(LedPin4, MinBrightness);
    if (client.connect(server, 80)) {
      String _getLink = "http://192.168.0.2/updatescore?game=simon&score=8";
      String getStr = _getLink;
      Serial.println("Updating score:" + _getLink);

      client.print("GET " + getStr + "\n");
      client.print("HTTP/1.1\n");
      client.print("Host: marblerun\n");
      client.print("Connection: close\n\n\n");

      while (client.available()) {
        char c = client.read();
        Serial.print(c);

      }
    }
  }
}

void show_sequence() {
  Serial.println("show sequence");
  aw.analogWrite(LedPin1, MinBrightness);
  aw.analogWrite(LedPin2, MinBrightness);
  aw.analogWrite(LedPin3, MinBrightness);
  aw.analogWrite(LedPin4, MinBrightness);

  for (int i = 0; i < level; i++) {
    Serial.print(sequence[i]);
    if (sequence[i] == 1) {
      aw.analogWrite(LedPin1, MaxBrightness);
      delay(velocity);
      aw.analogWrite(LedPin1, MinBrightness);
      delay(150);
    }

    if (sequence[i] == 2) {
      aw.analogWrite(LedPin2, MaxBrightness);
      delay(velocity);
      aw.analogWrite(LedPin2, MinBrightness);
      delay(150);
    }

    if (sequence[i] == 3) {
      aw.analogWrite(LedPin3, MaxBrightness);
      delay(velocity);
      aw.analogWrite(LedPin3, MinBrightness);
      delay(150);
    }

    if (sequence[i] == 4) {
      aw.analogWrite(LedPin4, MaxBrightness);
      delay(velocity);
      aw.analogWrite(LedPin4, MinBrightness);
      delay(150);
    }
  }
}

void get_sequence() {
  Serial.println("get sequence");
  Serial.println(digitalRead(D5));
  Serial.println(digitalRead(D6));
  Serial.println(digitalRead(D7));
  Serial.println(digitalRead(3));

  int flag = 0; //this flag indicates if the sequence is correct

  for (int i = 0; i < level; i++) {
    flag = 0;
    while (flag == 0) {
      delay(10);
      if (digitalRead(D4) == LOW) {
        Serial.println("reset game detected");
        level = 1;
        velocity = 1000;
        return;
      }
      if (digitalRead(D5) == LOW) {
        Serial.println("button 1");
        aw.analogWrite(LedPin1, MaxBrightness);
        your_sequence[i] = 1;
        flag = 1;
        while (digitalRead(D5) == LOW) {
          delay(100);
        }
        if (your_sequence[i] != sequence[i]) {
          wrong_sequence();
          return;
        }
        aw.analogWrite(LedPin1, MinBrightness);
      }

      if (digitalRead(D6) == LOW) {
        Serial.println("button 2");
        aw.analogWrite(LedPin2, MaxBrightness);
        your_sequence[i] = 2;
        flag = 1;
        while (digitalRead(D6) == LOW) {
          delay(100);
        }
        if (your_sequence[i] != sequence[i]) {
          wrong_sequence();
          return;
        }
        aw.analogWrite(LedPin2, MinBrightness);
      }

      if (digitalRead(D7) == LOW) {
        Serial.println("button 3");
        aw.analogWrite(LedPin3, MaxBrightness);
        your_sequence[i] = 3;
        flag = 1;
        while (digitalRead(D7) == LOW) {
          delay(100);
        }
        if (your_sequence[i] != sequence[i]) {
          wrong_sequence();
          return;
        }
        aw.analogWrite(LedPin3, MinBrightness);
      }

      if (digitalRead(3) == LOW) {
        Serial.println("button 4");
        aw.analogWrite(LedPin4, MaxBrightness);
        your_sequence[i] = 4;
        flag = 1;
        while (digitalRead(3) == LOW) {
          delay(100);
        }
        if (your_sequence[i] != sequence[i]) {
          wrong_sequence();
          return;
        }
        aw.analogWrite(LedPin4, MinBrightness);
      }

    }
  }
  right_sequence();
}

void generate_sequence() {
  //Serial.println("Generating sequence");
  randomSeed(millis()); //in this way is really random!!!

  for (int i = 0; i < MAX_LEVEL; i++) {
    sequence[i] = random(1, 5);
  }
}
void wrong_sequence() {
  Serial.println("wrong sequence detected");
  for (int i = 0; i < 3; i++) {
    aw.analogWrite(LedPin1, MaxBrightness);
    aw.analogWrite(LedPin2, MaxBrightness);
    aw.analogWrite(LedPin3, MaxBrightness);
    aw.analogWrite(LedPin4, MaxBrightness);
    delay(200);
    aw.analogWrite(LedPin1, MinBrightness);
    aw.analogWrite(LedPin2, MinBrightness);
    aw.analogWrite(LedPin3, MinBrightness);
    aw.analogWrite(LedPin4, MinBrightness);
    delay(200);
  }
  level = 1;
  velocity = 1000;
}

void right_sequence() {
  Serial.println("right sequence detected");
  aw.analogWrite(LedPin1, MinBrightness);
  aw.analogWrite(LedPin2, MinBrightness);
  aw.analogWrite(LedPin3, MinBrightness);
  aw.analogWrite(LedPin4, MinBrightness);
  delay(250);

  aw.analogWrite(LedPin1, MaxBrightness);
  aw.analogWrite(LedPin2, MaxBrightness);
  aw.analogWrite(LedPin3, MaxBrightness);
  aw.analogWrite(LedPin4, MaxBrightness);
  delay(500);
  aw.analogWrite(LedPin1, MinBrightness);
  aw.analogWrite(LedPin2, MinBrightness);
  aw.analogWrite(LedPin3, MinBrightness);
  aw.analogWrite(LedPin4, MinBrightness);
  delay(500);

  if (level < MAX_LEVEL);
  level++;

  velocity -= 50; //increase difficulty

  // if (client.connect(server,80))     // "184.106.153.149" or api.thingspeak.com
  //  { 
  //    String _getLink = "http://192.168.0.2/updatescore?game=simon&score=8";
  //    _getLink = _getLink + level;
  //    String getStr = _getLink;
  //    Serial.println("Updating score:"+_getLink);
  //
  //    client.print("GET "+getStr+"\n");
  //    client.print("HTTP/1.1\n");
  //    client.print("Host: marblerun\n");
  //    client.print("Connection: close\n\n\n");
  //
  //    while(client.available()){
  //  char c = client.read();
  //  Serial.print(c);
  //
  //    }
  //}
}

void game_complete() {
  Serial.println("Game Completed");

  for (int i = 0; i < 10; i++) {
  aw.analogWrite(LedPin1, MaxBrightness);
  aw.analogWrite(LedPin2, MaxBrightness);
  aw.analogWrite(LedPin3, MaxBrightness);
  aw.analogWrite(LedPin4, MaxBrightness);
    delay(200);
  aw.analogWrite(LedPin1, MinBrightness);
  aw.analogWrite(LedPin2, MinBrightness);
  aw.analogWrite(LedPin3, MinBrightness);
  aw.analogWrite(LedPin4, MinBrightness);
    delay(200);
  }
  level = 1;
  velocity = 1000;
}
