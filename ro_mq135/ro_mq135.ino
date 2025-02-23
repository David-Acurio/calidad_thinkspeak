const int a=34;
 float volt_rs;//valor en valor en voltaje del sensor
 float rs_aire;// rs en el aire limpio
 float r_o;//
 float sensorvalue=0;// valor del sensor en rango 0-4094
void setup() {
  Serial.begin(115200);
  Serial.println("Calculo Valor Ro");
  delay(500);

}

void loop() {
 // toma de 100 muestras 
 for(int x=0; x<100;x++){
  sensorvalue=sensorvalue+analogRead(a);
 }
sensorvalue=sensorvalue/100.0;
//calculo del ro
volt_rs=(sensorvalue*5.0)/4095.0;
rs_aire=20*((5.0/volt_rs-1));
r_o=rs_aire/3.8;
Serial.print("valor sensor  ");
Serial.println(sensorvalue);

Serial.print("volt_sensor ");
Serial.println(volt_rs);

Serial.print("Ro ");
Serial.println(r_o);
delay(1000);
}
