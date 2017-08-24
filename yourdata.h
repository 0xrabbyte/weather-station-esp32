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
*/  
// Change this and enter your pop3 email account informations
char serverName[]         = "";
int  popport              = 995 or 110;
char user[]               = "email address";
char password[]           = "password";
// your city and API KEY
//Replace cityID with your city
//Replace apikey with your API KEY
//Changzhou=CN101191105
String url = "/v5/weather?city=cityID&key=apikey";
// your SSID & Password
char ssid[] = "SSID";     //Replace with your SSID
char pass[] = "Password";     //Replace with your Password
//change this with your root ca on your email server by explorers
const char* test_root_ca_email = "" ;
//This is the data of the alarm
int alarmhour = 7;
int alarmminute = 30;
int alarmsecond = 0;
// Initialize the oled display for address 0x3C
// sda-pin=5 and sdc-pin=4
SSD1306 display(0x3C, 5, 4);
