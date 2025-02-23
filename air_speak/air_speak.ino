//librerias 
#include<DHT.h>
#include<SPI.h>
#include<Wire.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#include<WiFi.h>
#include<ThingSpeak.h>
//varibles para la conecion Wifi
#define WIFI_SSID "DUIROBOTICS"
#define WIFI_PASSWORD "12345678"
const char* server="api.thingspeak.com";
String apiKey="1ST36JSU0RCGEGO1";

//variables para pantalla oled 
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// VARIABLES SENSORES
const  int a=34; // pin del sensor mq135 co2
const int b=35; //pin del sensor mq09 co
const int c=19;// pin del dht11
DHT sensorDHT(c,DHT11);
int mq135;
int mq9;

WiFiClient client;
void setup() {
Serial.begin(115200);
delay(10);
Serial.println("Inicio de programa");
if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)){
  Serial.println("No se pudo Iniciar la pantalla oled");
  while(true);
}
display.clearDisplay();
display.setTextColor(WHITE);
display.setTextSize(2);
display.setCursor(0,0);
display.print(" DUIROBOTICS");
display.display();
//conecion wifi
Serial.println("Coneccion al wifi");
WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
while(WiFi.status() !=WL_CONNECTED){
  delay(500);
  Serial.print(".");
}
Serial.println(" ");
Serial.println("Wifi conectado");
Serial.println(WiFi.localIP());
ThingSpeak.begin(client);//inicializamos al cliente

}

void loop() {
  //lectura de los sensores 
  mq135=analogRead(a);//valor analogo del sensor mq135
  mq9=analogRead(b);//valor analogo del sensor mq9
  int temp=sensorDHT.readTemperature();
  int hum=sensorDHT.readHumidity();
  //valore muy elevados al inciar el programa 
  if(isnan(temp)|| isnan(hum)){
    Serial.println("Error al leer el DHT11");
    temp=0;
    hum=0;
  }
  // Calculo del sensor mq135 en ppm--
  float volt_mq135;
  float rs_mq135;
  float ratio_mq135;// relacion rs/ro
  float ro_mq135=57; // resistencia interna mq135 calculada en el aire limpio 

  volt_mq135=(5.0*mq135)/4094;// resolucion del esp32 4095
  rs_mq135=20*((5.0-volt_mq135)/volt_mq135);////calculo de la resitencia rs con un divisor de voltaje 
  ratio_mq135=rs_mq135/ro_mq135;

  //la relacion rs/ro y ppm es logaritmica lo que se representa con la ecuacion 
  // log(y)=m*log(x)+b
  //y=relacion (rs/ro)
  //m=pendiente de la recta 
  //b= punto intersecion 
  float m_mq135=-0.3679;
  float b_mq135=0.815;
  double ppm_log_mq135=(log(ratio_mq135)-b_mq135)/m_mq135;
  float ppm_co2=pow(10,ppm_log_mq135);

  /// calculos del sensor mq9 en ppm ------------------------
    float volt_mq9;
  float rs_mq9;
  float ratio_mq9;// relacion rs/ro
  float ro_mq9=19; // resistencia interna mq135 calculada en el aire limpio 

  volt_mq9=(5.0*mq9)/4094;// resolucion del esp32 4095
  rs_mq9=20*((5.0-volt_mq9)/volt_mq9);////calculo de la resitencia rs con un divisor de voltaje 
  ratio_mq9=rs_mq9/ro_mq9;

  //la relacion rs/ro y ppm es logaritmica lo que se representa con la ecuacion 
  // log(y)=m*log(x)+b
  //y=relacion (rs/ro)
  //m=pendiente de la recta 
  //b= punto intersecion 
  float m_mq9=-0.4101;
  float b_mq9=1.075;
  double ppm_log_mq9=(log(ratio_mq9)-b_mq9)/m_mq9;
  float ppm_co=pow(10,ppm_log_mq9);
  //vizualizacion en puerto serial y pantalla oled
  Serial.print("co2 ");
  Serial.println(ppm_co2);
  Serial.print("co ");
  Serial.println(ppm_co);
    Serial.print("Temperatura ");
  Serial.println(temp);
    Serial.print("Humedad ");
  Serial.println(hum);
  //pantalla oled
  display.clearDisplay();
  //titulo
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print("Calidad del Aire");
  //mq135
  display.setCursor(0,12);
  display.print("CO2 ");
  display.print(ppm_co2,2);
  display.print(" PPM");
  //mq9
  display.setCursor(0,24);
  display.print("CO ");
  display.print(ppm_co,2);
  display.print(" PPM");
    //TEMPERATURA
  display.setCursor(0,36);
  display.print("TEMP ");
  display.print(temp,1);
  display.print(" C");
      //humedad 
  display.setCursor(0,48);
  display.print("Humedad ");
  display.print(hum,1);
  display.print(" C");
  display.display();
  //Envio de datos thingspeak
  if(client.connect(server,80)){
    ThingSpeak.setField(1,ppm_co2);
    ThingSpeak.setField(2,hum);
    ThingSpeak.setField(3,temp);
    ThingSpeak.setField(4,ppm_co);
    ThingSpeak.writeFields(2828420,"1ST36JSU0RCGEGO1");
      client.stop();
  }
  delay(500);

}
