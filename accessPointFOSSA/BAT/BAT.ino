// FOSSA WITH ACCESS-POINT
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
using WebServerClass = WebServer;
fs::SPIFFSFS &FlashFS = SPIFFS;
#define FORMAT_ON_FAIL true
#include <lvgl.h>
#include <Arduino_GFX_Library.h>
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
#include <Free_Fonts.h>
#include <MontserratAlternates_SemiBold8pt7b.h>
//========================================================//
//============EDIT IF USING DIFFERENT HARDWARE============//
//========================================================//

//FORMATTING
bool format = false;  // true for formatting FOSSA memory, use once, then make false and reflash

//SCREEN
bool screenSize = True;  //True for 7", false for 3.5"

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
   uint32_t w = (area->x2 - area->x1 + 1);
   uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
   gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
   gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

   lv_disp_flush_ready(disp);
}

//BUTTON SETTINGS--
bool useTouch = false;  //Set to true to use touch screen tap, false for physical button.
#define buttonPin 22    //Set to GPIO for signal to button or touch, this is normally the TP(touch)IRQ IO signal feed IO. (button works on pin 22, touch 36 for ESP32-2432S028R
int debounce = 50;      //Set the debounce time (milliseconds) for the button, default should be 25, but may need modification if issues with sporadic presses are seen.

//ACCEPTOR SETTINGS--
#define RX1 1  //define the GPIO connected TO the TX of the bill acceptor
#define TX1 3  //define the GPIO connected TO the RX of the bill acceptor

#define TX2 35          //Coinmech 35
#define INHIBITMECH 21  //Coinmech 22

//GENERAL SETTINGS--
String logoName = "The B.A.T";     //set your business/logo name here to display on boot. configured for < 7 characters. Any bigger, make text size smaller.
String releaseVersion = "0.1";     //set the version of the release here.
String splashJpg = "/splash.jpg";  //set the image name of the .jpg file located on your SD card. This needs to be correct size of screen and rotated correctly before saving to SD card.
#define SDCard 5

//========================================================//
//========================================================//
//========================================================//


struct Language {
  bool nativeLang = false;
  String lauN = "";
  String porN = "";
  String resN = "";
  String staN = "";
  String wakN = "";
  String entN = "";
  String verN = "";
  String buyN = "";
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
};

Language language;

bool readConfig() {
  // Open the config.txt file
  File configFile = SD.open("/config.txt");
  if (!configFile) {
    Serial.println("Failed to open config.txt");
    return false;
  }

  // Read the file and parse the language settings
  String line;
  while (configFile.available()) {
    line = configFile.readStringUntil('\n');
    if (line.startsWith("nativeLang")) {
      if (line.substring(line.lastIndexOf("=") + 2) == "true") {
        language.nativeLang = true;
      }
    } else if (line.startsWith("lauN")) {
      int endIndex = line.indexOf(";");
      language.lauN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("porN")) {
      int endIndex = line.indexOf(";");
      language.porN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("resN")) {
      int endIndex = line.indexOf(";");
      language.resN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("staN")) {
      int endIndex = line.indexOf(";");
      language.staN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("wakN")) {
      int endIndex = line.indexOf(";");
      language.wakN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("entN")) {
      int endIndex = line.indexOf(";");
      language.entN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("verN")) {
      int endIndex = line.indexOf(";");
      language.verN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("buyN")) {
      int endIndex = line.indexOf(";");
      language.buyN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("herN")) {
      int endIndex = line.indexOf(";");
      language.herN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("insN")) {
      int endIndex = line.indexOf(";");
      language.insN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("exiN")) {
      int endIndex = line.indexOf(";");
      language.exiN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("lanN")) {
      int endIndex = line.indexOf(";");
      language.lanN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("feeN")) {
      int endIndex = line.indexOf(";");
      language.feeN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("preN")) {
      int endIndex = line.indexOf(";");
      language.preN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("tapN")) {
      int endIndex = line.indexOf(";");
      language.tapN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("totN")) {
      int endIndex = line.indexOf(";");
      language.totN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("scaN")) {
      int endIndex = line.indexOf(";");
      language.scaN = line.substring(line.lastIndexOf("=") + 2, endIndex);
    } else if (line.startsWith("indN")) {
      int endIndex = line.indexOf(";");
      language.indN = line.substring(line.lastIndexOf("=") + 2, endIndex);
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
String herE = "HERE!";
String insE = "Insert notes/coins.";
String exiE = "WHEN FINISHED.";
String feeE = "Fee:";
String lanE = "change language.";
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
float coinAmountFloat[6] = { 0.02, 0.05, 0.1, 0.2, 0.5, 1 };
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

bool getSats() {
  WiFiClientSecure client;
  client.setInsecure();  //Some versions of WiFiClientSecure need this

  lnbitsServer.toLowerCase();
  if (lnbitsServer.substring(0, 8) == "https://") {
    lnbitsServer = lnbitsServer.substring(8, lnbitsServer.length());
  }
  const char *lnbitsServerChar = lnbitsServer.c_str();
  const char *invoiceChar = invoice.c_str();
  const char *lncurrencyChar = lncurrency.c_str();

  Serial.println("connecting to LNbits server " + lnbitsServer);
  if (!client.connect(lnbitsServerChar, 443)) {
    Serial.println("failed to connect to LNbits server " + lnbitsServer);
    return false;
  }

  const String toPost = "{\"amount\" : 1, \"from\" :\"" + String(lncurrencyChar) + "\"}";
  const String url = "/api/v1/conversion";
  client.print(String("POST ") + url + " HTTP/1.1\r\n" + "Host: " + String(lnbitsServerChar) + "\r\n" + "User-Agent: ESP32\r\n" + "X-Api-Key: " + String(invoiceChar) + " \r\n" + "Content-Type: application/json\r\n" + "Connection: close\r\n" + "Content-Length: " + toPost.length() + "\r\n" + "\r\n" + toPost + "\n");

  while (client.connected()) {
    const String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  const String line = client.readString();
  StaticJsonDocument<150> doc;
  DeserializationError error = deserializeJson(doc, line);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return false;
  }

  converted = doc["sats"];
  return true;
}

TFT_eSPI tft = TFT_eSPI();

WebServerClass server;
AutoConnect portal(server);
AutoConnectConfig config;
AutoConnectAux elementsAux;
AutoConnectAux saveAux;

Button BTNA(buttonPin, debounce, false, false);

static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
   uint32_t w = (area->x2 - area->x1 + 1);
   uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
   gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
   gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

   lv_disp_flush_ready(disp);
}



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
  tft.setFreeFont(&MontserratAlternates_SemiBold8pt7b);
  BTNA.begin();
  readConfig();
  setLang();
  tft.init();
  tft.setRotation(1);
  tft.invertDisplay(false);
  tft.fillScreen(TFT_BLACK);
  TJpgDec.drawSdJpg(0, 0, splashJpg);
  delay(3000);
  tft.fillScreen(TFT_BLACK);
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
  SerialPort1.begin(300, SERIAL_8N2, TX1, RX1);
  SerialPort2.begin(4800, SERIAL_8N1, TX2);

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
  tft.fillScreen(TFT_BLACK);
  setLang();
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
  readConfig();
  if (!language.nativeLang) {
    lau = lauE;
    por = porE;
    res = resE;
    sta = staE;
    wak = wakE;
    ent = entE;
    ver = verE;
    buy = buyE;
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
    lau = language.lauN;
    por = language.porN;
    res = language.resN;
    sta = language.staN;
    wak = language.wakN;
    ent = language.entN;
    ver = language.verN;
    buy = language.buyN;
    her = language.herN;
    ins = language.insN;
    exi = language.exiN;
    fee = language.feeN;
    lan = language.lanN;
    pre = language.preN;
    tap = language.tapN;
    tot = language.totN;
    sca = language.scaN;
    ind = language.indN;
  }
}

void printMessage(String text1, String text2, String text3, int ftcolor, int ftcolor2, int bgcolor) {
  tft.fillScreen(bgcolor);
  tft.setTextColor(ftcolor, bgcolor);
  tft.setTextSize(2);
  tft.setCursor(30, 40);
  tft.println(text1);
  tft.setCursor(30, 120);
  tft.println(text2);
  tft.setCursor(30, 200);
  tft.setTextColor(ftcolor2, bgcolor);
  tft.setTextSize(1);
  tft.println(text3);
}

void showRate(bool cleared) {
  if (!cleared) {
    amountToShow = String(dataIn.toFloat() / 100);
    noSats = String((converted / 100) * dataIn.toFloat());
  } else {
    noSats = "0";
    dataIn = "0";
    amountToShow = "0";
  }

  tft.setTextSize(3);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.setCursor(20, 280);
  tft.println(amountToShow);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(75, 75);
  tft.println(noSats.toInt());
}

void logo() {
  tft.fillScreen(TFT_WHITE);
  tft.setTextSize(5);
  tft.setCursor(140, 50);
  tft.setTextColor(TFT_ORANGE);
  tft.println(logoName);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(80, 160);
  tft.setTextSize(3);
  tft.println("Bitcoin ATM");
  tft.setCursor(10, 300);
  tft.setTextSize(1);
  tft.setTextColor(TFT_PURPLE);
  tft.println(ver + releaseVersion);
}

void feedmefiat() {
  tft.setTextColor(TFT_ORANGE);
  tft.setCursor(110, 10);
  tft.setTextSize(2);
  tft.println("Bitcoin ATM");
  tft.setTextSize(3);
  tft.setCursor(180, 80);
  tft.println(buy);
  tft.setCursor(100, 150);
  tft.println("BITCOIN");
  tft.setCursor(150, 220);
  tft.println(her);
  delay(100);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(180, 80);
  tft.println(buy);
  tft.setCursor(100, 150);
  tft.println("BITCOIN");
  tft.setCursor(150, 220);
  tft.println(her);
  delay(100);
  tft.setTextColor(TFT_BLUE);
  tft.setCursor(180, 80);
  tft.println(buy);
  tft.setCursor(100, 150);
  tft.println("BITCOIN");
  tft.setCursor(150, 220);
  tft.println(her);
  delay(100);
  tft.setTextColor(TFT_ORANGE);
  tft.setCursor(180, 80);
  tft.println(buy);
  tft.setCursor(100, 150);
  tft.println("BITCOIN");
  tft.setCursor(150, 220);
  tft.println(her);
  delay(100);
  tft.setCursor(10, 300);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.println(ins);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(10, 280);
  tft.println(fee + String(charge) + "%");
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
  tft.setTextSize(2);
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
      if (BTNA.isPressed() && total == 0) {
        readConfig();
        language.nativeLang = !language.nativeLang;
        setLang();
        tft.fillScreen(TFT_BLACK);
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