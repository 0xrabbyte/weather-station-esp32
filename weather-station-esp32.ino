/**The MIT License (MIT)

  Copyright (c) 2017 by Zhanghao

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  If you have some troubles or questions ,email me at 2582008641@qq.com
  Credits for some parts of this code go to Squix(http://blog.squix.ch). Thank you so much for sharing!
  Also thank to MartinCoolen for his code on email.
*/
/*
 * Go and download TimeLib and TimeAlarms here:https://github.com/PaulStoffregen/
 */
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <Wire.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiClientSecure.h>
#include "ssd1306_i2c.h"
#include "icons.h"
#include "yourdata.h"
#include "/home/zhanghao/esp32weatherstring.h"  //if put string.h in arduino's folder,correct Chinese word in UART can't display

int screenW = 128;
int screenH = 64;
int clockCenterX = screenW/2-32;
int clockCenterY = ((screenH-16)/2);   // top yellow part is 16 px height
int clockRadius = 21;
String twoDigits(int digits){
  if(digits < 10) {
    String i = '0'+String(digits);
    return i;
  }
  else {
    return String(digits);
  }
}
const String weekdays[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
const char* test_root_ca_weather = \
                                   "-----BEGIN CERTIFICATE-----\n" \
                                   "MIIF2DCCA8CgAwIBAgIQTKr5yttjb+Af907YWwOGnTANBgkqhkiG9w0BAQwFADCB\n" \
                                   "hTELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4G\n" \
                                   "A1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxKzApBgNV\n" \
                                   "BAMTIkNPTU9ETyBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAwMTE5\n" \
                                   "MDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBhTELMAkGA1UEBhMCR0IxGzAZBgNVBAgT\n" \
                                   "EkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEaMBgGA1UEChMR\n" \
                                   "Q09NT0RPIENBIExpbWl0ZWQxKzApBgNVBAMTIkNPTU9ETyBSU0EgQ2VydGlmaWNh\n" \
                                   "dGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQCR\n" \
                                   "6FSS0gpWsawNJN3Fz0RndJkrN6N9I3AAcbxT38T6KhKPS38QVr2fcHK3YX/JSw8X\n" \
                                   "pz3jsARh7v8Rl8f0hj4K+j5c+ZPmNHrZFGvnnLOFoIJ6dq9xkNfs/Q36nGz637CC\n" \
                                   "9BR++b7Epi9Pf5l/tfxnQ3K9DADWietrLNPtj5gcFKt+5eNu/Nio5JIk2kNrYrhV\n" \
                                   "/erBvGy2i/MOjZrkm2xpmfh4SDBF1a3hDTxFYPwyllEnvGfDyi62a+pGx8cgoLEf\n" \
                                   "Zd5ICLqkTqnyg0Y3hOvozIFIQ2dOciqbXL1MGyiKXCJ7tKuY2e7gUYPDCUZObT6Z\n" \
                                   "+pUX2nwzV0E8jVHtC7ZcryxjGt9XyD+86V3Em69FmeKjWiS0uqlWPc9vqv9JWL7w\n" \
                                   "qP/0uK3pN/u6uPQLOvnoQ0IeidiEyxPx2bvhiWC4jChWrBQdnArncevPDt09qZah\n" \
                                   "SL0896+1DSJMwBGB7FY79tOi4lu3sgQiUpWAk2nojkxl8ZEDLXB0AuqLZxUpaVIC\n" \
                                   "u9ffUGpVRr+goyhhf3DQw6KqLCGqR84onAZFdr+CGCe01a60y1Dma/RMhnEw6abf\n" \
                                   "Fobg2P9A3fvQQoh/ozM6LlweQRGBY84YcWsr7KaKtzFcOmpH4MN5WdYgGq/yapiq\n" \
                                   "crxXStJLnbsQ/LBMQeXtHT1eKJ2czL+zUdqnR+WEUwIDAQABo0IwQDAdBgNVHQ4E\n" \
                                   "FgQUu69+Aj36pvE8hI6t7jiY7NkyMtQwDgYDVR0PAQH/BAQDAgEGMA8GA1UdEwEB\n" \
                                   "/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAArx1UaEt65Ru2yyTUEUAJNMnMvl\n" \
                                   "wFTPoCWOAvn9sKIN9SCYPBMtrFaisNZ+EZLpLrqeLppysb0ZRGxhNaKatBYSaVqM\n" \
                                   "4dc+pBroLwP0rmEdEBsqpIt6xf4FpuHA1sj+nq6PK7o9mfjYcwlYRm6mnPTXJ9OV\n" \
                                   "2jeDchzTc+CiR5kDOF3VSXkAKRzH7JsgHAckaVd4sjn8OoSgtZx8jb8uk2Intzna\n" \
                                   "FxiuvTwJaP+EmzzV1gsD41eeFPfR60/IvYcjt7ZJQ3mFXLrrkguhxuhoqEwWsRqZ\n" \
                                   "CuhTLJK7oQkYdQxlqHvLI7cawiiFwxv/0Cti76R7CZGYZ4wUAc1oBmpjIXUDgIiK\n" \
                                   "boHGhfKppC3n9KUkEEeDys30jXlYsQab5xoq2Z0B15R97QNKyvDb6KkBPvVWmcke\n" \
                                   "jkk9u+UJueBPSZI9FoJAzMxZxuY67RIuaTxslbH9qh17f4a+Hg4yRvv7E491f0yL\n" \
                                   "S0Zj/gA0QHDBw7mh3aZw4gSzQbzpgJHqZJx64SIDqZxubw5lT2yHh17zbqD5daWb\n" \
                                   "QOhTsiedSrnAdyGN/4fy3ryM7xfft0kL0fJuMAsaDk527RH89elWsn2/x20Kk4yl\n" \
                                   "0MC2Hb46TpSi125sC8KKfPog88Tk5c0NqMuRkrF8hey1FGlmDoLnzc7ILaZRfyHB\n" \
                                   "NVOFBkpdn627G190\n" \
                                   "-----END CERTIFICATE-----\n" ;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 28800, 60000);
String timeString = "hh:mm:ss";

char JSON_Array[6000];
const unsigned int WeatherCodeSet[] PROGMEM = { 100 , 101 , 102 , 103 , 104 , 200 , 201 , 202 , 203 , 204 , 205 , 206 , 207 , 208 , 209 , 210 , 211 , 212 , 213 , 300 , 301 , 302 , 303 , 304 , 305 , 306 , 307 , 308 , 309 , 310 , 311 , 312 , 313 , 400 , 401 , 402 , 403 , 404 , 405 , 406 , 407 , 500 , 501 , 502 , 503 , 504 , 506 , 507 , 508 , 900 , 901 , 999 };
String num_emails;
String tmp_emails;
unsigned char codeNo_d_0;
unsigned char codeNo_n_0;
int tmp_max_0;
int tmp_min_0;
unsigned char codeNo_d_1;
unsigned char codeNo_n_1;
int tmp_max_1;
int tmp_min_1;
unsigned char codeNo_d_2;
unsigned char codeNo_n_2;
int tmp_max_2;
int tmp_min_2;
unsigned char codeNo_now;
int tmp_now;
int hum_now;
void smartConfig()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("SmartConfig received.");
}
unsigned char ParseCode(unsigned int Code)
{
  unsigned char i, No;
  for (i = 0 ; i < 52 ; i ++) {
    if (Code == WeatherCodeSet[i]) {
      break;
    }
  }
  return i;
}

int ParseStringToInt(char* inString , unsigned int StartPos , unsigned char offset)
{
  int OutInt;

  if (inString[StartPos + offset] != '-')                  // >= 0
  {
    if (inString[StartPos + offset + 1] == '"') {         // [0,9]
      OutInt = inString[StartPos + offset] - '0';
    }
    else if (inString[StartPos + offset + 2] == '"') {    // [10,99]
      OutInt = (inString[StartPos + offset] - '0') * 10 + (inString[StartPos + offset + 1] - '0');
    }
    else if (inString[StartPos + offset + 3] == '"') {    // [100,999]
      OutInt = (inString[StartPos + offset] - '0') * 100 + (inString[StartPos + offset + 1] - '0') * 10 + (inString[StartPos + offset + 2] - '0');
    }
  }
  else
  {
    if (inString[StartPos + offset + 2] == '"') {         // [-9,0)
      OutInt = (inString[StartPos + offset + 1] - '0') * -1;
    }
    else if (inString[StartPos + offset + 3] == '"') {    // [-99,-10]
      OutInt = (inString[StartPos + offset + 1] - '0') * -10 + (inString[StartPos + offset + 2] - '0') * -1;
    }
  }
  return OutInt;
}
void new_email() {
  for (int i=0;i<5;i++){
    digitalWrite(15, HIGH);   // turn the LED on (HIGH is the voltage level)
    Alarm.delay(100);                       
    digitalWrite(15, LOW);    // turn the LED off by making the voltage LOW
    Alarm.delay(100);
    digitalWrite(15, HIGH);   // turn the LED on (HIGH is the voltage level)
    Alarm.delay(100);                       
    digitalWrite(15, LOW);    // turn the LED off by making the voltage LOW
    Alarm.delay(1000);                       // wait for a second
  }
}
void alarm() {
  const int timetone[]={500,300,200,100};
  // put your main code here, to run repeatedly:
  for (int i=0;i<4;i++){
    for (int j=0;j<15000/timetone[i];j++){
      digitalWrite(15, HIGH);
      delay(timetone[i]);
      digitalWrite(15, LOW);
      delay(timetone[i]);
    }
  }
}
void UpdateWeatherData(void) {
  String JSON_String;
  WiFiClientSecure client;
  client.setCACert(test_root_ca_weather);
  const int httpsPort = 443;
  if (!client.connect("free-api.heweather.com", httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host:api.heweather.com\r\n" +
               "Connection: close\r\n\r\n");
  while (!client.available()) {

    Alarm.delay(200);
  }

  Serial.println("Get forecast ");

  unsigned char rx_counter = 0;

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(rx_counter);
    Serial.println(line);
    //Serial.println(line.length());

    //if(rx_counter == 7)   //9th is JSON String
    //in March,2016,rx_counter should be 9,
    //but in June,2016,rx_counter should be 7,
    //so I using method below
    if ( sizeof( line ) > 2000);
    {
      JSON_String = line;
      JSON_String.toCharArray(JSON_Array , 6000);
    }
    rx_counter++;
  }
  Serial.println("closing connection");

  //------------------------------------------------------------------ 0 TODAY ---------------------------------------------------------------
  int code_d_0_pos = JSON_String.indexOf("code_d");
  int code_d_0 = (JSON_Array[code_d_0_pos + 9] - '0') * 100 + (JSON_Array[code_d_0_pos + 10] - '0') * 10 + (JSON_Array[code_d_0_pos + 11] - '0');
  codeNo_d_0 = ParseCode(code_d_0);

  int code_n_0_pos = JSON_String.indexOf("code_n");
  int code_n_0 = (JSON_Array[code_n_0_pos + 9] - '0') * 100 + (JSON_Array[code_n_0_pos + 10] - '0') * 10 + (JSON_Array[code_n_0_pos + 11] - '0');
  codeNo_n_0 = ParseCode(code_n_0);

  int tmp_max_0_pos = JSON_String.indexOf("max");
  tmp_max_0 = ParseStringToInt(JSON_Array , tmp_max_0_pos , 6);

  int tmp_min_0_pos = JSON_String.indexOf("min");
  tmp_min_0 = ParseStringToInt(JSON_Array , tmp_min_0_pos , 6);

  Serial.print(F("code_d_0 is "));
  Serial.println(code_d_0);
  Serial.println(WeatherString[codeNo_d_0]);
  Serial.print(F("code_n_0 is "));
  Serial.println(code_n_0);
  Serial.println(WeatherString[codeNo_n_0]);
  Serial.print(F("tmp_max_0 is "));
  Serial.println(tmp_max_0);
  Serial.print(F("tmp_min_0 is "));
  Serial.println(tmp_min_0);
  Serial.println(" ");

  //------------------------------------------------------------------ 1 TOMORROW ---------------------------------------------------------------
  int code_d_1_pos = JSON_String.indexOf("code_d", code_d_0_pos + 10);
  int code_d_1 = (JSON_Array[code_d_1_pos + 9] - '0') * 100 + (JSON_Array[code_d_1_pos + 10] - '0') * 10 + (JSON_Array[code_d_1_pos + 11] - '0');
  codeNo_d_1 = ParseCode(code_d_1);

  int code_n_1_pos = JSON_String.indexOf("code_n", code_n_0_pos + 10);
  int code_n_1 = (JSON_Array[code_n_1_pos + 9] - '0') * 100 + (JSON_Array[code_n_1_pos + 10] - '0') * 10 + (JSON_Array[code_n_1_pos + 11] - '0');
  codeNo_n_1 = ParseCode(code_n_1);

  int tmp_max_1_pos = JSON_String.indexOf("max", tmp_max_0_pos + 10);
  tmp_max_1 = ParseStringToInt(JSON_Array , tmp_max_1_pos , 6);

  int tmp_min_1_pos = JSON_String.indexOf("min", tmp_min_0_pos + 10);
  tmp_min_1 = ParseStringToInt(JSON_Array , tmp_min_1_pos , 6);

  Serial.print(F("code_d_1 is "));
  Serial.println(code_d_1);
  Serial.println(WeatherString[codeNo_d_1]);
  Serial.print(F("code_n_1 is "));
  Serial.println(code_n_1);
  Serial.println(WeatherString[codeNo_n_1]);
  Serial.print(F("tmp_max_1 is "));
  Serial.println(tmp_max_1);
  Serial.print(F("tmp_min_1 is "));
  Serial.println(tmp_min_1);
  Serial.println(" ");

  //------------------------------------------------------------------ 2 THE DAY AFTER TOMORROW ---------------------------------------------------------------
  int code_d_2_pos = JSON_String.indexOf("code_d", code_d_1_pos + 10);
  int code_d_2 = (JSON_Array[code_d_2_pos + 9] - '0') * 100 + (JSON_Array[code_d_2_pos + 10] - '0') * 10 + (JSON_Array[code_d_2_pos + 11] - '0');
  codeNo_d_2 = ParseCode(code_d_2);

  int code_n_2_pos = JSON_String.indexOf("code_n", code_n_1_pos + 10);
  int code_n_2 = (JSON_Array[code_n_2_pos + 9] - '0') * 100 + (JSON_Array[code_n_2_pos + 10] - '0') * 10 + (JSON_Array[code_n_2_pos + 11] - '0');
  codeNo_n_2 = ParseCode(code_n_2);

  int tmp_max_2_pos = JSON_String.indexOf("max", tmp_max_1_pos + 10);
  tmp_max_2 = ParseStringToInt(JSON_Array , tmp_max_2_pos , 6);

  int tmp_min_2_pos = JSON_String.indexOf("min", tmp_min_1_pos + 10);
  tmp_min_2 = ParseStringToInt(JSON_Array , tmp_min_2_pos , 6);

  Serial.print(F("code_d_2 is "));
  Serial.println(code_d_2);
  Serial.println(WeatherString[codeNo_d_2]);
  Serial.print(F("code_n_2 is "));
  Serial.println(code_n_2);
  Serial.println(WeatherString[codeNo_n_2]);
  Serial.print(F("tmp_max_2 is "));
  Serial.println(tmp_max_2);
  Serial.print(F("tmp_min_2 is "));
  Serial.println(tmp_min_2);
  Serial.println(" ");

  //------------------------------------------------------------------ NOW ---------------------------------------------------------------
  int code_now_pos = JSON_String.indexOf("code", JSON_String.indexOf("now"));      //search "code" after "now"
  int code_now = (JSON_Array[code_now_pos + 7] - '0') * 100 + (JSON_Array[code_now_pos + 8] - '0') * 10 + (JSON_Array[code_now_pos + 9] - '0');
  codeNo_now = ParseCode(code_now);

  int tmp_now_pos = JSON_String.indexOf("tmp", JSON_String.indexOf("now"));       //search "tmp" after "now"
  tmp_now = ParseStringToInt(JSON_Array , tmp_now_pos , 6);

  int hum_now_pos = JSON_String.indexOf("hum", JSON_String.indexOf("now"));       //search "hump" after "now"
  hum_now = ParseStringToInt(JSON_Array , hum_now_pos , 6);

  //Serial.print(F("code No now is "));
  //Serial.println(codeNo_now);
  Serial.print(F("code_now is "));
  Serial.println(code_now);
  Serial.println(WeatherString[codeNo_now]);
  Serial.print(F("tmp_now is "));
  Serial.println(tmp_now);
  Serial.print(F("hum_now is "));
  Serial.println(hum_now);
  Serial.println(" ");
  //------------------------------------------------------------------ END of Parse ---------------------------------------------------------------
  client.setCACert(test_root_ca_email);
  if (!client.connect(serverName, popport)) {
    Serial.println("connection failed");
    return;
  }
  Serial.println("Connected");
  char c;
  while (client.available()) {
    c = client.read();
    Serial.print(c);
  };
  client.print("USER ");
  client.println(user);
  Alarm.delay(200);
  while (client.available()) {
    c = client.read();
    Serial.print(c);
  };
  client.print("PASS ");
  client.println(password);
  Alarm.delay(500);
  while (client.available()) {
    c = client.read();
    Serial.print(c);
  };
  num_emails="";
  bool a,b;
  a=false;b=false;
  client.println("STAT");
  Alarm.delay(500);
  while (client.available()) {
    c = client.read();
    Serial.print(c);
    if (c==' ' && a==false) { a=true;}
    else if (c==' ' && a==true) b=true;
    if (a==true && b==false && c!=' ') num_emails=num_emails+c;
  };
  Serial.println(num_emails);
  if(num_emails=="") num_emails="Failed";
  if(tmp_emails!=num_emails) new_email();
  tmp_emails=num_emails;
  Alarm.delay(500);
  client.println("QUIT");
  client.stop();
}

long long timeSinceLastWUpdate = 0;
// this array keeps function pointers to all frames
// frames are the single views that slide from right to left
void (*frameCallbacks[5])(int x, int y) = {drawFrame1, drawFrame2, drawFrame3, drawFrame4, drawFrame5};

// how many frames are there?
int frameCount = 5;
// on frame is currently displayed
int currentFrame = 0;

// flag changed in the ticker function every 10 minutes
bool readyForWeatherUpdate = true;
bool readyForTimeUpdate = true;

void setup() {
  // initialize dispaly
  display.init();
  // set the drawing functions
  display.setFrameCallbacks(5, frameCallbacks);
  // how many ticks does a slide of frame take?
  display.setFrameTransitionTicks(10);

  display.clear();
  display.display();

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  pinMode(15, OUTPUT);
  pinMode(12, INPUT);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  int counter = 0;
  while ((WiFi.status() != WL_CONNECTED) && (counter < 10)) {
    delay(500);
    Serial.print(".");
    display.clear();
    display.drawXbm(34, 10, 60, 36, WiFi_Logo_bits);
    display.setColor(INVERSE);
    display.fillRect(10, 10, 108, 44);
    display.setColor(WHITE);
    drawSpinner(5, counter % 5);
    display.display();
    counter++;
  }
  if (WiFi.status() != WL_CONNECTED) smartConfig();
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  timeClient.update();
  time_t tmp=timeClient.getEpochTime();
  setTime(tmp);
  Alarm.alarmRepeat(alarmhour,alarmminute,alarmsecond, alarm);
  // update the weather information every 10 mintues only
  // forecast.io only allows 1000 calls per day
}

void loop() {
  if (digitalRead(12)== HIGH){
    setReadyForWeatherUpdate();
    setReadyForTimeUpdate();
  }
  if (millis() - timeSinceLastWUpdate > (1000L * 600)) {
    setReadyForWeatherUpdate();
    timeSinceLastWUpdate = millis();
  }
  if (readyForWeatherUpdate && display.getFrameState() == display.FRAME_STATE_FIX) {
    readyForWeatherUpdate = false;
    UpdateWeatherData();
  }

  if (readyForTimeUpdate && display.getFrameState() == display.FRAME_STATE_FIX) {
    readyForTimeUpdate = false;
    timeClient.update();
    timeString = timeClient.getFormattedTime();
    Serial.println(timeString);
  }
  //timeClient.update();
  //Serial.println(timeClient.getFormattedTime());
  //delay(1000);

  display.clear();
  display.nextFrameTick();
  display.display();
}

void setReadyForWeatherUpdate() {
  readyForWeatherUpdate = true;
}
void setReadyForTimeUpdate() {
  readyForTimeUpdate = true;
}

const char* getIcon(unsigned char No) {
  //"clear-day, clear-night, rain, snow, sleet, wind, fog, cloudy, partly-cloudy-day, or partly-cloudy-night"
  if (No == 0) {
    return clear_day_bits;
  } else if ( (No > 18) && (No < 33) ) {
    return rain_bits;
  } else if ( (No > 32) && (No < 37) ) {
    return snow_bits;
  } else if (No == 37) {
    return sleet_bits;
  } else if ( (No > 4) && (No < 18) ) {
    return wind_bits;
  } else if ( (No == 41) || (No == 42 ) ) {
    return fog_bits;
  } else if ( No == 43 ) {
    return haze_bits;
  } else if (No == 1) {
    return cloudy_bits;
  } else if ( (No == 3) || (No == 4 ) ) {
    return partly_cloudy_day_bits;
  }
  return na_bits;
}

void drawFrame1(int x, int y) {
  display.drawXbm(x + 7, y + 7, 50, 50, getIcon(codeNo_now));
  display.setFontScale2x2(false);
  display.drawString(65 + x, 0 + y, "Now");
  display.setFontScale2x2(true);
  display.drawString(64 + x, 10 + y, String(tmp_now) + "C");
  display.drawString(64 + x, 28 + y, String(hum_now) + "%");

}

/*
  void drawFrame1(int x, int y) {
   display.setFontScale2x2(false);
   display.drawString(65 + x, 8 + y, "Now");
   display.drawXbm(x+7,y+7, 50, 50, getIcon(codeNo_now));
   display.setFontScale2x2(true);
   display.drawString(64+ x, 20 + y, String(tmp_now) + "C");
  }
*/

void drawFrame2(int x, int y) {
  display.setFontScale2x2(false);
  display.drawString(65 + x, 0 + y, "Today");
  display.drawXbm(x, y, 60, 60, xbmtemp);
  display.setFontScale2x2(true);
  display.drawString(64 + x, 14 + y, String(tmp_now) + "C");
  display.setFontScale2x2(false);
  display.drawString(66 + x, 40 + y, String(tmp_min_0) + "C/" + String(tmp_max_0) + "C");
}

void drawFrame3(int x, int y) {
  display.drawXbm(x + 7, y + 7, 50, 50, getIcon(codeNo_d_1));
  display.setFontScale2x2(false);
  display.drawString(65 + x, 7 + y, "Tomorrow");
  display.setFontScale2x2(false);
  display.drawString(66 + x, 20 + y, String(tmp_min_1) + "C/" + String(tmp_max_1) + "C");
}

void drawFrame4(int x, int y) {
  display.drawXbm(x + 7, y + 7, 50, 50, email_bits);
  display.setFontScale2x2(false);
  display.drawString(65 + x, 7 + y, "Unread");
  display.drawString(65 + x, 14 + y, "emails:");
  if (num_emails!="Failed") display.setFontScale2x2(true);
  display.drawString(64 + x, 30 + y, num_emails);
}
void drawFrame5(int x, int y) {
  display.drawCircle(clockCenterX + x, clockCenterY + y, 2);
  //
  //hour ticks
  for( int z=0; z < 360;z= z + 30 ){
  //Begin at 0° and stop at 360°
    float angle = z ;
    angle = ( angle / 57.29577951 ) ; //Convert degrees to radians
    int x2 = ( clockCenterX + ( sin(angle) * clockRadius ) );
    int y2 = ( clockCenterY - ( cos(angle) * clockRadius ) );
    int x3 = ( clockCenterX + ( sin(angle) * ( clockRadius - ( clockRadius / 8 ) ) ) );
    int y3 = ( clockCenterY - ( cos(angle) * ( clockRadius - ( clockRadius / 8 ) ) ) );
    display.drawLine( x2 + x , y2 + y , x3 + x , y3 + y);
  }

  // display second hand
  float angle = timeClient.getSeconds() * 6 ;
  angle = ( angle / 57.29577951 ) ; //Convert degrees to radians
  int x3 = ( clockCenterX + ( sin(angle) * ( clockRadius - ( clockRadius / 5 ) ) ) );
  int y3 = ( clockCenterY - ( cos(angle) * ( clockRadius - ( clockRadius / 5 ) ) ) );
  display.drawLine( clockCenterX + x , clockCenterY + y , x3 + x , y3 + y);
  //
  // display minute hand
  angle = timeClient.getMinutes() * 6 ;
  angle = ( angle / 57.29577951 ) ; //Convert degrees to radians
  x3 = ( clockCenterX + ( sin(angle) * ( clockRadius - ( clockRadius / 4 ) ) ) );
  y3 = ( clockCenterY - ( cos(angle) * ( clockRadius - ( clockRadius / 4 ) ) ) );
  display.drawLine( clockCenterX + x , clockCenterY + y , x3 + x , y3 + y);
  //
  // display hour hand
  angle = timeClient.getHours() * 30 + int( ( timeClient.getMinutes() / 12 ) * 6 )   ;
  angle = ( angle / 57.29577951 ) ; //Convert degrees to radians
  x3 = ( clockCenterX + ( sin(angle) * ( clockRadius - ( clockRadius / 2 ) ) ) );
  y3 = ( clockCenterY - ( cos(angle) * ( clockRadius - ( clockRadius / 2 ) ) ) );
  display.drawLine( clockCenterX + x , clockCenterY + y , x3 + x , y3 + y);
  display.setFontScale2x2(false);
  display.drawString(55 + x, 0 + y, weekdays[timeClient.getDay()]);
  display.drawString(55 + x, 10 + y, "alarm");
  display.drawString(55 + x, 20 + y, timeClient.getFormattedTime());
  time_t epoch=timeClient.getEpochTime();
  display.drawString(55 + x, 30 + y, String(year(epoch))+'-'+monthShortStr(month(epoch)));
  display.drawString(55 + x, 40 + y, "Day"+String(day(epoch)));
  
}
void drawSpinner(int count, int active) {
  for (int i = 0; i < count; i++) {
    const char *xbm;
    if (active == i) {
      xbm = active_bits;
    } else {
      xbm = inactive_bits;
    }
    display.drawXbm(64 - (12 * count / 2) + 12 * i, 56, 8, 8, xbm);
  }
}

