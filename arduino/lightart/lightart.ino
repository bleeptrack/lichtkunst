#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>            
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);   //Web server object. Will be listening in port 80 (default for HTTP) 

 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(18, D3, NEO_GRB + NEO_KHZ800);
uint8_t count = 0;
boolean mode = false;
boolean isOn = true;
uint8_t user1 = 2;
uint8_t user2 = 0;
uint16_t twitter = 512;
uint16_t instagram = 640;
uint16_t fitbit = 384;
 
void setup() {
  Serial.begin(115200);
  
  WiFi.begin("ssid", "password"); //Connect to the WiFi network
  
  
  while (WiFi.status() != WL_CONNECTED) { //Wait for connection
  
  delay(500);
  Serial.println("Waiting to connect...");
  
  }
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP to access the server

  server.on("/", handleSpecificArg);   //Associate the handler function to the path
  
  server.begin();                                       //Start the server
  Serial.println("Server listening");   
  strip.begin();
}
 
void loop() {
  server.handleClient(); 
  //notification(0,i);
  if(isOn){
    if(count == 0){
      count = random(10,50);
      mode = !mode;
    }
    if(mode){
      randBlink();
    }else{
      randChase();
    }
    count--;
  }else{
    clear();
    strip.show();
  }
}

void handleSpecificArg() { 

String message = "";

  if (server.arg("status") != ""){     //Parameter not found  
    message = "status = ";
    message += server.arg("status");     //Gets the value of the query parameter 
    if(server.arg("status").toInt()==1){
      isOn = true;
    }else{
      isOn = false;
    }
  }

  uint32_t c = 0;
  uint8_t i = 0;

  if (server.arg("notify") != "" && server.arg("user") != ""){     //Parameter not found  
    message = "notify = ";
    message += server.arg("notify");     //Gets the value of the query parameter 
    if(server.arg("notify")=="twitter"){
      c = twitter;
    }
    if(server.arg("notify")=="instagram"){
      c = instagram;
    }
    if(server.arg("notify")=="fitbit"){
      c = fitbit;
    }
    if(server.arg("user")=="user1"){
      i = user1;
    }
    if(server.arg("user")=="user2"){
      i = user2;
    }
    notification(i,c);
  }
  
  
  server.send(200, "text/plain", message);          //Returns the HTTP response

}

static void layerChase(uint16_t img, uint16_t hue) {
    for(uint16_t l=0; l<3; l++){
      fade(img,l,hue);
    }
}

static void notification(uint8_t img, uint16_t hue){
  uint8_t col[3];
  hsb2rgb(hue,255,255,col);
  for(int i = 0; i<2; i++){
    setImage(img,0,strip.Color(col[0], col[1], col[2]));
    setImage(img,1,strip.Color(col[0], col[1], col[2]));
    setImage(img,2,strip.Color(col[0], col[1], col[2]));
    delay(300);
    clear();
    strip.show();
    delay(150);
  }
}

static void randBlink(){
  uint8_t i = random(0,3);
  uint8_t l = random(0,3);
  uint16_t c = random(0,769);
  fade(i,l,c);
}

static void randChase(){
  uint8_t i = random(0,3);
  uint16_t c = random(0,769);
  layerChase(i,c);
}

static void fade(uint16_t img, uint16_t layer, uint16_t hue){
  uint8_t col[3];
  for(uint16_t b=0; b<=500; b++){
        server.handleClient();
        clear();
        uint16_t brit = b; 
        if(b>255){
          brit = 255-(b-255);
        }
        hsb2rgb(hue,255,brit,col);
        setImage(img,layer,strip.Color(col[0], col[1], col[2]));
        delay(10);
  }
}

void hsb2rgb(uint16_t index, uint8_t sat, uint8_t bright, uint8_t color[3]) {
    uint8_t temp[5], n = (index >> 8) % 3;
// %3 not needed if input is constrained, but may be useful for color cycling and/or if modulo constant is fast
    uint8_t x = ((((index & 255) * sat) >> 8) * bright) >> 8;
// shifts may be added for added speed and precision at the end if fast 32 bit calculation is available
    uint8_t s = ((256 - sat) * bright) >> 8;
    temp[0] = temp[3] =              s;
    temp[1] = temp[4] =          x + s;
    temp[2] =          bright - x    ;
    color[0]  = temp[n + 2];
    color[1] = temp[n + 1];
    color[2]  = temp[n    ];
}


 
static void chase(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels()+4; i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip.show();
      delay(500);
  }
}

static void clear() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0); // Erase pixel a few steps back
  }
}

static void setImage(uint16_t img, uint16_t layer, uint32_t c) {
   strip.setPixelColor((6*img)+layer, c); // Erase pixel a few steps back
   strip.setPixelColor((6*img)+(5-layer), c); // Erase pixel a few steps back
   strip.show();
}
