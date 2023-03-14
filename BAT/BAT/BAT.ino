// FOSSA WITH ACCESS-POINT
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
using WebServerClass = WebServer;
fs::SPIFFSFS &FlashFS = SPIFFS;
#define FORMAT_ON_FAIL true
#include <AutoConnect.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <JC_Button.h>
#include <TJpg_Decoder.h>
#include <Hash.h>
#include "qrcoded.h"
#include "Bitcoin.h"
#include "SPI.h"
//#include <Free_Fonts.h>
//#include <MontserratAlternates_SemiBold8pt7b.h>
//========================================================//
//============EDIT IF USING DIFFERENT HARDWARE============//
//========================================================//

//FORMATTING
bool format = false;  // true for formatting FOSSA memory, use once, then make false and reflash

//SCREEN
bool screenSize = false;  //True for 7", false for 3.5"

//BUTTON SETTINGS--
bool useTouch = false;  //Set to true to use touch screen tap, false for physical button.
#define buttonPin 22    //Set to GPIO for signal to button or touch, this is normally the TP(touch)IRQ IO signal feed IO. (button works on pin 22, touch 36 for ESP32-2432S028R
int debounce = 40;      //Set the debounce time (milliseconds) for the button, default should be 25, but may need modification if issues with sporadic presses are seen.

//ACCEPTOR SETTINGS--
#define RXB 35  //define the GPIO connected TO the TX of the bill acceptor
#define TXB 21  //define the GPIO connected TO the RX of the bill acceptor

#define RXC 3          //define the GPIO connected TO the SIGNAL/TX of the coin acceptor
#define INHIBITMECH 1  //define the GPIO connected TO the INHIBIT of the coin acceptor

//GENERAL SETTINGS--
String logoName = "The B.A.T";     //set your business/logo name here to display on boot. configured for < 7 characters. Any bigger, make text size smaller.
String releaseVersion = "0.1";     //set the version of the release here.
String splashJpg = "/splash.jpg";  //set the image name of the .jpg file located on your SD card. This needs to be correct size of screen and rotated correctly before saving to SD card.
bool online = true;
bool stealthMode = false;
#define SDCard 5

  //========================================================//
  //========================================================//
  //========================================================//

struct Configuration {
  bool online = false;
  bool nativeLang = false;
  bool dualLang = false;
  bool stealthMode = false;
  String lauN = "";
  String porN = "";
  String resN = "";
  String staN = "";
  String wakN = "";
  String entN = "";
  String verN = "";
  String buyN = "";
  String bitN = "";
  String herN = "";
  String insN = "";
  String exiN = "";
  String lanN = "";
  String feeN = "";
  String preN = "";
  String tapN = "";
  String totN = "";
  String scaN = "";
  String indN = "";
  uint32_t colour1 = 0xffffff;
  uint32_t colour2 = 0xffffff;
  uint32_t colour3 = 0xffffff;
  uint32_t colour4 = 0xffffff;
  uint32_t colour5 = 0xffffff;
  uint32_t background = 0x000000;
};


Configuration cfg;
bool readConfig() {
  // Open the config.txt file
  File configFile = SD.open("/config.ini");
  if (!configFile) {
    printMessage("Failed to open config.ini", "", "", TFT_WHITE, TFT_WHITE, TFT_BLACK);
    return false;
  }

  // Read the file and parse the language settings
  String line;
  while (configFile.available()) {
    line = configFile.readStringUntil('\n');
    if (line.startsWith("Online")) {
      int endIndex = line.indexOf(";");
      String olValue = line.substring(line.lastIndexOf("=") + 2, endIndex);
      if (olValue == "true") {
        cfg.online = true;
      }
    } else if (line.startsWith("NativeLanguage")) {
      int endIndex = line.indexOf(";");
      String nlValue = line.substring(line.lastIndexOf("=") + 2, endIndex);
      if (nlValue == "true") {
        cfg.nativeLang = true;
      }
    } else if (line.startsWith("DualLanguage")) {
      int endIndex = line.indexOf(";");
      String dlValue = line.substring(line.lastIndexOf("=") + 2, endIndex);
      if (dlValue == "true") {
        cfg.dualLang = true;
      }
    } else if (line.startsWith("StealthMode")) {
      int endIndex = line.indexOf(";");
      String smValue = line.substring(line.lastIndexOf("=") + 2, endIndex);
      if (smValue == "true") {
        cfg.stealthMode = true;
      }
    } else if (line.startsWith("Colour1")) {
      int endIndex = line.indexOf(";");
      String colourValue = line.substring(line.lastIndexOf("=") + 2, endIndex);
      cfg.colour1 = (uint32_t)strtol(colourValue.c_str(), NULL, 16);
    } else if (line.startsWith("Colour2")) {
      int endIndex = line.indexOf(";");
      String colourValue2 = line.substring(line.lastIndexOf("=") + 2, endIndex);
      cfg.colour2 = (uint32_t)strtol(colourValue2.c_str(), NULL, 16);
    } else if (line.startsWith("Colour3")) {
      int endIndex = line.indexOf(";");
      String colourValue3 = line.substring(line.lastIndexOf("=") + 2, endIndex);
      cfg.colour3 = (uint32_t)strtol(colourValue3.c_str(), NULL, 16);
    } else if (line.startsWith("Colour4")) {
      int endIndex = line.indexOf(";");
      String colourValue4 = line.substring(line.lastIndexOf("=") + 2, endIndex);
      cfg.colour4 = (uint32_t)strtol(colourValue4.c_str(), NULL, 16);
    } else if (line.startsWith("Colour5")) {
      int endIndex = line.indexOf(";");
      String colourValue5 = line.substring(line.lastIndexOf("=") + 2, endIndex);
      cfg.colour5 = (uint32_t)strtol(colourValue5.c_str(), NULL, 16);
    } else if (line.startsWith("Background")) {
      int endIndex = line.indexOf(";");
      String bgValue = line.substring(line.lastIndexOf("=") + 2, endIndex);
      cfg.background = (uint32_t)strtol(bgValue.c_str(), NULL, 16);
    } else if (line.startsWith("Launch_Portal")) {
      int endIndex = line.indexOf(";");
      cfg.lauN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Portal_Launched")) {
      int endIndex = line.indexOf(";");
      cfg.porN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Restart_Launch")) {
      int endIndex = line.indexOf(";");
      cfg.resN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Starting_Acceptors")) {
      int endIndex = line.indexOf(";");
      cfg.staN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Waking_Up")) {
      int endIndex = line.indexOf(";");
      cfg.wakN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Entered")) {
      int endIndex = line.indexOf(";");
      cfg.entN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Version")) {
      int endIndex = line.indexOf(";");
      cfg.verN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("BUY")) {
      int endIndex = line.indexOf(";");
      cfg.buyN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("BITCOIN")) {
      int endIndex = line.indexOf(";");
      cfg.bitN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("HERE")) {
      int endIndex = line.indexOf(";");
      cfg.herN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Insert_Fiat")) {
      int endIndex = line.indexOf(";");
      cfg.insN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Exit")) {
      int endIndex = line.indexOf(";");
      cfg.exiN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Change_Language")) {
      int endIndex = line.indexOf(";");
      cfg.lanN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Fee")) {
      int endIndex = line.indexOf(";");
      cfg.feeN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Press")) {
      int endIndex = line.indexOf(";");
      cfg.preN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Tap")) {
      int endIndex = line.indexOf(";");
      cfg.tapN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Total")) {
      int endIndex = line.indexOf(";");
      cfg.totN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Scan")) {
      int endIndex = line.indexOf(";");
      cfg.scaN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("Inserted")) {
      int endIndex = line.indexOf(";");
      cfg.indN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    }
  }
  configFile.close();
  return true;
}

String lauE = "Launch portal";
String porE = "Portal launched.";
String resE = "Restart/launch portal!";
String staE = "Starting Acceptor(s)";
String wakE = "Waking up.";
String entE = "entered.";
String verE = "Version: ";
String buyE = "BUY";
String bitE = "BITCOIN";
String herE = "HERE";
String insE = "Insert notes/coins.";
String exiE = "WHEN FINISHED.";
String feeE = "Fee:";
String lanE = "change config.";
String preE = "PRESS BUTTON ";
String tapE = "TAP SCREEN ";
String totE = "Total: ";
String scaE = "SCAN ME TO RECEIVE SATS. ";
String indE = "INSERTED";

String lau;
String por;
String res;
String sta;
String wak;
String ent;
String ver;
String buy;
String bit;
String her;
String ins;
String exi;
String lan;
String fee;
String pre;
String tap;
String tot;
String sca;
String ind;

#define PARAM_FILE "/elements.json"

String qrData;
String password;
String apPassword = "admin";  //default WiFi AP password
String baseURLATM;
String secretATM;
String currencyATM = "";
String lnbitsServer;
String invoice;

int bills;
float coins;
float total;
int maxamount;
int charge;

bool billBool = true;
bool coinBool = true;

int moneyTimer = 0;
int converted = 0;

// Coin and Bill Acceptor amounts
int billAmountInt[5] = { 5, 10, 20, 50, 100 };
float coinAmountFloat[6] = { 0.05, 0.1, 0.2, 0.5, 1, 2 };
int billAmountSize = sizeof(billAmountInt) / sizeof(int);
float coinAmountSize = sizeof(coinAmountFloat) / sizeof(float);

HardwareSerial SerialPort1(1);
HardwareSerial SerialPort2(2);



/////////////////////////////////////
////////////////PORTAL///////////////
/////////////////////////////////////

bool triggerAp = false;

String content = "<h1>ATM Access-point</br>For easy variable setting</h1>";

// custom access point pages
static const char PAGE_ELEMENTS[] PROGMEM = R"(
{
  "uri": "/config",
  "title": "ATM Configuration",
  "menu": true,
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "Access Point Config",
      "style": "font-family:Arial;font-size:16px;font-weight:400;color:#191970;margin-botom:15px;"
    },
    {
      "name": "password",
      "type": "ACInput",
      "label": "Password",
      "value": "admin"
    },
    {
      "name": "text",
      "type": "ACText",
      "value": "Project options",
      "style": "font-family:Arial;font-size:16px;font-weight:400;color:#191970;margin-botom:15px;"
    },
    {
      "name": "lnurl",
      "type": "ACInput",
      "label": "ATM string for LNBits",
      "value": ""
    },
    {
      "name": "coinmech",
      "type": "ACInput",
      "label": "Coin values: ie 0.01,0.02,0.05,0.10,0.20,0.50,1,2",
      "value": ""
    },
    {
      "name": "billmech",
      "type": "ACInput",
      "label": "Note values: ie 5,10,20",
      "value": ""
    },
    {
      "name": "maxamount",
      "type": "ACInput",
      "label": "Maximum allowable withdrawl in FIAT",
      "value": "50"			   
    },
	{
      "name": "charge",
      "type": "ACInput",
      "label": "Percentage charge for service",
      "value": "10"
    },
    {
      "name": "load",
      "type": "ACSubmit",
      "value": "Load",
      "uri": "/config"
    },
    {
      "name": "save",
      "type": "ACSubmit",
      "value": "Save",
      "uri": "/save"
    },
    {
      "name": "adjust_width",
      "type": "ACElement",
      "value": "<script type='text/javascript'>window.onload=function(){var t=document.querySelectorAll('input[]');for(i=0;i<t.length;i++){var e=t[i].getAttribute('placeholder');e&&t[i].setAttribute('size',e.length*.8)}};</script>"
    }
  ]
 }
)";


static const char PAGE_SAVE[] PROGMEM = R"(
{
  "uri": "/save",
  "title": "Elements",
  "menu": false,
  "element": [
    {
      "name": "caption",
      "type": "ACText",
      "format": "Elements have been saved to %s",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    },
    {
      "name": "validated",
      "type": "ACText",
      "style": "color:red"
    },
    {
      "name": "echo",
      "type": "ACText",
      "style": "font-family:monospace;font-size:small;white-space:pre;"
    },
    {
      "name": "ok",
      "type": "ACSubmit",
      "value": "OK",
      "uri": "/config"
    }
  ]
}
)";


TFT_eSPI tft = TFT_eSPI();

WebServerClass server;
AutoConnect portal(server);
AutoConnectConfig config;
AutoConnectAux elementsAux;
AutoConnectAux saveAux;

Button BTNA(buttonPin, debounce, false, false);



/////////////////////////////////////
////////////////SETUP////////////////
/////////////////////////////////////

void setup() {
  if (!SD.begin(SDCard)) {
    printMessage("SD Card error", "", "", TFT_WHITE, TFT_WHITE, TFT_BLACK);
    while (1) delay(0);
  }
  tft.setSwapBytes(true);
  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(printLogo);
  tft.setTextSize(1);
  //tft.setFreeFont(&MontserratAlternates_SemiBold8pt7b);
  BTNA.begin();
  readConfig();
  setLang();
  tft.init();
  tft.setRotation(1);
  tft.invertDisplay(false);
  tft.fillScreen(cfg.background);
  TJpgDec.drawSdJpg(0, 0, splashJpg);
  delay(4000);
  logo();

  int timer = 0;
  while (timer < 2000) {
    BTNA.read();
    if (BTNA.wasReleased()) {
      timer = 5000;
      triggerAp = true;
    }
    timer = timer + 100;
    delay(100);
  }
  SerialPort1.begin(300, SERIAL_8N2, RXB, TXB);
  SerialPort2.begin(4800, SERIAL_8N1, RXC);

  pinMode(INHIBITMECH, OUTPUT);

  FlashFS.begin(FORMAT_ON_FAIL);
  SPIFFS.begin(true);
  if (format == true) {
    SPIFFS.format();
  }
  // get the saved details and store in global variables
  File paramFile = FlashFS.open(PARAM_FILE, "r");
  if (paramFile) {
    StaticJsonDocument<2500> doc;
    DeserializationError error = deserializeJson(doc, paramFile.readString());

    const JsonObject maRoot0 = doc[0];
    const char *maRoot0Char = maRoot0["value"];
    password = maRoot0Char;

    const JsonObject maRoot1 = doc[1];
    const char *maRoot1Char = maRoot1["value"];
    const String lnurlATM = maRoot1Char;
    baseURLATM = getValue(lnurlATM, ',', 0);
    secretATM = getValue(lnurlATM, ',', 1);
    currencyATM = getValue(lnurlATM, ',', 2);

    const JsonObject maRoot2 = doc[2];
    const char *maRoot2Char = maRoot2["value"];
    const String billmech = maRoot2Char;
    if (billmech != "") {
      billAmountInt[0] = getValue(billmech, ',', 0).toInt();
      billAmountInt[1] = getValue(billmech, ',', 1).toInt();
      billAmountInt[2] = getValue(billmech, ',', 2).toInt();
      billAmountInt[3] = getValue(billmech, ',', 3).toInt();
      billAmountInt[4] = getValue(billmech, ',', 4).toInt();
      billAmountInt[5] = getValue(billmech, ',', 5).toInt();
    }
    const JsonObject maRoot3 = doc[3];
    const char *maRoot3Char = maRoot3["value"];
    const String coinmech = maRoot2Char;
    if (coinmech != "") {
      coinAmountFloat[0] = getValue(coinmech, ',', 0).toFloat();
      coinAmountFloat[1] = getValue(coinmech, ',', 1).toFloat();
      coinAmountFloat[2] = getValue(coinmech, ',', 2).toFloat();
      coinAmountFloat[3] = getValue(coinmech, ',', 3).toFloat();
      coinAmountFloat[4] = getValue(coinmech, ',', 4).toFloat();
      coinAmountFloat[5] = getValue(coinmech, ',', 5).toFloat();
    }

    const JsonObject maRoot4 = doc[4];
    const char *maRoot4Char = maRoot4["value"];
    const String maxamountstr = maRoot4Char;
    maxamount = maxamountstr.toInt();
    const JsonObject maRoot5 = doc[5];
    const char *maRoot5Char = maRoot5["value"];
    const String chargestr = maRoot5Char;
    charge = chargestr.toInt();
  } else {
    triggerAp = true;
  }
  paramFile.close();
  server.on("/", []() {
    content += AUTOCONNECT_LINK(COG_24);
    server.send(200, "text/html", content);
  });

  elementsAux.load(FPSTR(PAGE_ELEMENTS));
  elementsAux.on([](AutoConnectAux &aux, PageArgument &arg) {
    File param = FlashFS.open(PARAM_FILE, "r");
    if (param) {
      aux.loadElement(param, { "password", "lnurl", "coinmech", "billmech", "maxamount", "charge" });
      param.close();
    }

    if (portal.where() == "/config") {
      File param = FlashFS.open(PARAM_FILE, "r");
      if (param) {
        aux.loadElement(param, { "password", "lnurl", "coinmech", "billmech", "maxamount", "charge" });
        param.close();
      }
    }
    return String();
  });
  saveAux.load(FPSTR(PAGE_SAVE));
  saveAux.on([](AutoConnectAux &aux, PageArgument &arg) {
    aux["caption"].value = PARAM_FILE;
    File param = FlashFS.open(PARAM_FILE, "w");
    if (param) {
      // save as a loadable set for parameters.
      elementsAux.saveElement(param, { "password", "lnurl", "coinmech", "billmech", "maxamount", "charge" });
      param.close();
      // read the saved elements again to display.
      param = FlashFS.open(PARAM_FILE, "r");
      aux["echo"].value = param.readString();
      param.close();
    } else {
      aux["echo"].value = "Filesystem failed to open.";
    }
    return String();
  });
  config.auth = AC_AUTH_BASIC;
  config.authScope = AC_AUTHSCOPE_AUX;
  config.ticker = true;
  config.autoReconnect = true;
  config.apid = "Device-" + String((uint32_t)ESP.getEfuseMac(), HEX);
  config.psk = password;
  config.menuItems = AC_MENUITEM_CONFIGNEW | AC_MENUITEM_OPENSSIDS | AC_MENUITEM_RESET;
  config.title = logoName;
  config.reconnectInterval = 1;

  if (triggerAp == true) {
    digitalWrite(11, LOW);
    printMessage(por, "", "", TFT_WHITE, TFT_WHITE, TFT_BLACK);
    config.immediateStart = true;
    portal.join({ elementsAux, saveAux });
    portal.config(config);
    portal.begin();
    while (true) {
      portal.handleClient();
    }
    timer = 2000;
  }
  if (currencyATM == "") {
    digitalWrite(11, LOW);
    printMessage(res, lau, "", TFT_WHITE, TFT_WHITE, TFT_BLACK);
    delay(99999999);
  }
}

void loop() {
  // Turn on machines
  SerialPort1.write(184);
  digitalWrite(INHIBITMECH, HIGH);
  tft.fillScreen(cfg.background);
  moneyTimerFun();
  makeLNURL();
  qrShowCodeLNURL(sca + pre + exi);
}

bool printLogo(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap) {
  // Stop further decoding as image is running off bottom of screen
  if (y >= tft.height()) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}


void setLang() {
  if (!cfg.nativeLang) {
    lau = lauE;
    por = porE;
    res = resE;
    sta = staE;
    wak = wakE;
    ent = entE;
    ver = verE;
    buy = buyE;
    bit = bitE;
    her = herE;
    ins = insE;
    exi = exiE;
    fee = feeE;
    lan = lanE;
    pre = preE;
    tap = tapE;
    tot = totE;
    sca = scaE;
    ind = indE;
  } else {
    readConfig();
    lau = cfg.lauN;
    por = cfg.porN;
    res = cfg.resN;
    sta = cfg.staN;
    wak = cfg.wakN;
    ent = cfg.entN;
    ver = cfg.verN;
    buy = cfg.buyN;
    bit = cfg.bitN;
    her = cfg.herN;
    ins = cfg.insN;
    exi = cfg.exiN;
    fee = cfg.feeN;
    lan = cfg.lanN;
    pre = cfg.preN;
    tap = cfg.tapN;
    tot = cfg.totN;
    sca = cfg.scaN;
    ind = cfg.indN;
  }
}


void printText(const char *text, uint8_t textSize = 1, uint32_t textColor = TFT_WHITE, int16_t x = -1, int16_t y = -1) {
  String textString = String(text);

  // Set the text parameters
  tft.setTextSize(textSize);
  tft.setTextColor(textColor);

  // Calculate the width and height of the text
  uint16_t textWidth = tft.textWidth(textString);
  uint16_t textHeight = tft.fontHeight(textSize);

  // Use the center coordinates if x and y are not specified
  if (x == -1) {
    x = (tft.width() - textWidth) / 2;
  }
  if (y == -1) {
    y = (tft.height() - textHeight) / 2;
  }

  // Set the cursor position and print the string
  tft.setCursor(x, y);
  tft.print(textString);
}




void printMessage(String text1, String text2, String text3, int ftcolor, int ftcolor2, int bgcolor) {
  tft.fillScreen(bgcolor);
  tft.setTextColor(ftcolor, bgcolor);
  tft.setTextSize(3);
  tft.setCursor(30, 40);
  tft.println(text1);
  tft.setCursor(30, 120);
  tft.println(text2);
  tft.setCursor(30, 200);
  tft.setTextColor(ftcolor2, bgcolor);
  tft.setTextSize(2);
  tft.println(text3);
}

void logo() {
  tft.fillScreen(cfg.background);
  printText("Bitcoin ATM", 4, cfg.colour5, -1, 20);
  printText(logoName.c_str(), 7, cfg.colour4, -1, 115);
  String versionRelease = String(ver.c_str()) + String(releaseVersion.c_str());
  printText(versionRelease.c_str(), 2, cfg.colour4, 10, 300);
}

void feedmefiat() {
  printText(buy.c_str(), 7, cfg.colour1, -1, 20);
  printText(bit.c_str(), 7, cfg.colour1, -1, 115);
  printText(her.c_str(), 7, cfg.colour1, -1, 220);
  delay(100);
  printText(buy.c_str(), 7, cfg.colour2, -1, 20);
  printText(bit.c_str(), 7, cfg.colour2, -1, 115);
  printText(her.c_str(), 7, cfg.colour2, -1, 220);
  delay(100);
  printText(buy.c_str(), 7, cfg.colour3, -1, 20);
  printText(bit.c_str(), 7, cfg.colour3, -1, 115);
  printText(her.c_str(), 7, cfg.colour3, -1, 220);
  delay(100);
  printText(ins.c_str(), 2, cfg.colour5, 10, 300);
  printText((fee + String(charge) + "%").c_str(), 2, cfg.colour4, 10, 280);
}


void qrShowCodeLNURL(String message) {
  tft.fillScreen(TFT_WHITE);
  qrData.toUpperCase();
  const char *qrDataChar = qrData.c_str();
  QRCode qrcoded;
  uint8_t qrcodeData[qrcode_getBufferSize(20)];
  qrcode_initText(&qrcoded, qrcodeData, 11, 0, qrDataChar);

  for (uint8_t y = 0; y < qrcoded.size; y++) {
    for (uint8_t x = 0; x < qrcoded.size; x++) {
      if (qrcode_getModule(&qrcoded, x, y)) {
        tft.fillRect(120 + 4 * x, 40 + 4 * y, 4, 4, TFT_BLACK);
      } else {
        tft.fillRect(120 + 4 * x, 40 + 4 * y, 4, 4, TFT_WHITE);
      }
    }
  }

  tft.setCursor(40, 290);
  tft.setTextSize(1);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.println(message);

  bool waitForTap = true;
  while (waitForTap) {
    BTNA.read();
    if (BTNA.wasReleased()) {
      waitForTap = false;
    }
  }
}

void moneyTimerFun() {
  bool waitForTap = true;
  bool langChange = true;
  coins = 0;
  bills = 0;
  total = 0;

  while (waitForTap || total == 0) {
    if (total == 0) {
      feedmefiat();
      BTNA.read();
      if (cfg.dualLang && BTNA.isPressed() && total == 0) {
        cfg.nativeLang = !cfg.nativeLang;
        setLang();
        tft.fillScreen(cfg.background);
        langChange = false;
      }
    }
    if (SerialPort1.available()) {
      int x = SerialPort1.read();
      for (int i = 0; i < billAmountSize; i++) {
        if ((i + 1) == x) {
          bills = bills + billAmountInt[i];
          total = (coins + bills);
          printMessage(billAmountInt[i] + currencyATM + " " + ind, tot + String(total) + " " + currencyATM, pre + exi, TFT_WHITE, TFT_ORANGE, TFT_BLACK);
        }
      }
    }
    if (SerialPort2.available()) {
      int x = SerialPort2.read();
      for (int i = 0; i < coinAmountSize; i++) {
        if ((i + 1) == x) {
          coins = coins + coinAmountFloat[i];
          total = (coins + bills);
          printMessage(coinAmountFloat[i] + currencyATM + " " + ind, tot + String(total) + currencyATM, pre + exi, TFT_WHITE, TFT_ORANGE, TFT_BLACK);
        }
      }
    }
    BTNA.read();
    if (BTNA.wasReleased() && (total > 0 || total == maxamount)) {
      waitForTap = false;
    }
  }

  total = (coins + bills) * 100;

  // Turn off machines
  SerialPort1.write(185);
  digitalWrite(INHIBITMECH, LOW);
}

/////////////////////////////////////
/////////////UTIL STUFF//////////////
/////////////////////////////////////

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  const int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void to_upper(char *arr) {
  for (size_t i = 0; i < strlen(arr); i++) {
    if (arr[i] >= 'a' && arr[i] <= 'z') {
      arr[i] = arr[i] - 'a' + 'A';
    }
  }
}

////////////////////////////////////////////
///////////////LNURL STUFF//////////////////
////USING STEPAN SNIGREVS GREAT CRYTPO//////
////////////THANK YOU STEPAN////////////////
////////////////////////////////////////////

void makeLNURL() {
  int randomPin = random(1000, 9999);
  byte nonce[8];
  for (int i = 0; i < 8; i++) {
    nonce[i] = random(256);
  }

  byte payload[51];  // 51 bytes is max one can get with xor-encryption

  size_t payload_len = xor_encrypt(payload, sizeof(payload), (uint8_t *)secretATM.c_str(), secretATM.length(), nonce, sizeof(nonce), randomPin, float(total));
  String preparedURL = baseURLATM + "?atm=1&p=";
  preparedURL += toBase64(payload, payload_len, BASE64_URLSAFE | BASE64_NOPADDING);

  Serial.println(preparedURL);
  char Buf[200];
  preparedURL.toCharArray(Buf, 200);
  char *url = Buf;
  byte *data = (byte *)calloc(strlen(url) * 2, sizeof(byte));
  size_t len = 0;
  int res = convert_bits(data, &len, 5, (byte *)url, strlen(url), 8, 1);
  char *charLnurl = (char *)calloc(strlen(url) * 2, sizeof(byte));
  bech32_encode(charLnurl, "lnurl", data, len);
  to_upper(charLnurl);
  qrData = charLnurl;
}

int xor_encrypt(uint8_t *output, size_t outlen, uint8_t *key, size_t keylen, uint8_t *nonce, size_t nonce_len, uint64_t pin, uint64_t amount_in_cents) {
  // check we have space for all the data:
  // <variant_byte><len|nonce><len|payload:{pin}{amount}><hmac>
  if (outlen < 2 + nonce_len + 1 + lenVarInt(pin) + 1 + lenVarInt(amount_in_cents) + 8) {
    return 0;
  }

  int cur = 0;
  output[cur] = 1;  // variant: XOR encryption
  cur++;

  // nonce_len | nonce
  output[cur] = nonce_len;
  cur++;
  memcpy(output + cur, nonce, nonce_len);
  cur += nonce_len;

  // payload, unxored first - <pin><currency byte><amount>
  int payload_len = lenVarInt(pin) + 1 + lenVarInt(amount_in_cents);
  output[cur] = (uint8_t)payload_len;
  cur++;
  uint8_t *payload = output + cur;                                  // pointer to the start of the payload
  cur += writeVarInt(pin, output + cur, outlen - cur);              // pin code
  cur += writeVarInt(amount_in_cents, output + cur, outlen - cur);  // amount
  cur++;

  // xor it with round key
  uint8_t hmacresult[32];
  SHA256 h;
  h.beginHMAC(key, keylen);
  h.write((uint8_t *)"Round secret:", 13);
  h.write(nonce, nonce_len);
  h.endHMAC(hmacresult);
  for (int i = 0; i < payload_len; i++) {
    payload[i] = payload[i] ^ hmacresult[i];
  }

  // add hmac to authenticate
  h.beginHMAC(key, keylen);
  h.write((uint8_t *)"Data:", 5);
  h.write(output, cur);
  h.endHMAC(hmacresult);
  memcpy(output + cur, hmacresult, 8);
  cur += 8;

  // return number of bytes written to the output
  return cur;
}