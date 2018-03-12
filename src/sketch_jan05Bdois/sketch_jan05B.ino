#include <usart.h>
#include <math.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

char flag_humidity=0;
const int buzzer = 10;
//char incendio:1;
int flagalarm1=0;
int flagalarm2=0;

int gHumiDry=25;
unsigned long int ghumidity=0;

int button = LOW; 

unsigned long int gVolume=0;
unsigned long int gTank=0;
unsigned long int gDrill=0;
unsigned long int gWater_tbl=0;

unsigned long int gVolume_bit=0;
unsigned long int gTank_bit=0;
unsigned long int gDrill_bit=0;
unsigned long int gWater_tbl_bit=0;

unsigned long int gVolume2_bit=0;
unsigned long int gTank2_bit=0;
unsigned long int gDrill2_bit=0;
unsigned long int gWater_tbl2_bit=0;

unsigned long int Water_tbl_min = 22000; //22m3 equivale a 25% do maximo do lençol freatico 90m3
unsigned long int Drill_min = 2000; //2m3 equivale a 25% do maximo do furo 8m3
unsigned long int gVolume_min= 50;

long int Vol_final;
long int Vol_empty;
long int Vol_avbl;

unsigned long int Tank_max = 60000;
unsigned long int Tank_min = 15000;

int real_time=7500;

int gHumidity_bit2=0;
unsigned int count = 0; 


ISR(TIMER2_OVF_vect) {
  count++;               //Increments the interrupt counter
  if(count > 999){
    count = 0;           //Resets the interrupt counter
  }
  serialRX();
  TCNT2 = 130;           //Reset Timer to 130 out of 255
  TIFR2 = 0x00;          //Timer2 INT Flag Reg: Clear Timer Overflow Flag
};
void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000/frequency/2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length/ 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to 
  //// get the total number of cycles to produce
 for (long i=0; i < numCycles; i++){ // for the calculated length of time...
    digitalWrite(targetPin,HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin,LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait againf or the calculated delay value
  }
}
void buttonsee()
{
  button = !button;
  if(button==HIGH){
    if(gTank<Tank_max){
  digitalWrite(24, LOW);
  digitalWrite(28, HIGH);
  digitalWrite(26, HIGH);
  digitalWrite(22, HIGH);
  if(gDrill <= Drill_min)
  {
    digitalWrite(28, LOW);
   }
   if(gWater_tbl <= Water_tbl_min)
   {

    digitalWrite(26, LOW);
    }
  }
  }
 else{ 
   if(gTank>=Tank_min){
  digitalWrite(28, LOW);
  digitalWrite(26, LOW);
   }
  digitalWrite(22, LOW);
 }
}
void msr_levels(void)
{
  gTank_bit = analogRead(2);
  delay(10);
  gDrill_bit = analogRead(6);
  delay(10);
  gWater_tbl_bit = analogRead(8);
  delay(10);
  gVolume_bit = analogRead(0);
  delay(10);
  if((abs(gTank_bit-gTank2_bit)>= 2) || (abs(gDrill_bit-gDrill2_bit)>= 1) || (abs(gWater_tbl_bit-gWater_tbl2_bit)>= 5) || (abs(gVolume_bit-gVolume2_bit)>= 6)){

    gTank= map(gTank_bit,0,1023,0,60000);
    gDrill= map (gDrill_bit,0,1023,0,8000);
    gWater_tbl = map(gWater_tbl_bit,0,1023,0,90000);
    gVolume=map(gVolume_bit,0,1023,0,200);
    if( flagalarm1==0 && flagalarm2==0){
    Serial.print("Tanque: ");
    Serial.print(gTank);
    Serial.print("\t Furo ");
    Serial.print(gDrill);
    Serial.print("\t Lencol ");
    Serial.println(gWater_tbl);
    Serial.print("\t Caudal ");
    Serial.println(gVolume);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tanque: ");
    lcd.print(gTank);
    lcd.print("L");
    lcd.setCursor(0,1);
    lcd.print("Lencol: ");
    lcd.print(gWater_tbl);
    lcd.print("L");
    Serial.print("teste");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Furo: ");
    lcd.print(gDrill);
    lcd.print("L");
    lcd.setCursor(0,1);
    lcd.print("Humidade: ");
    lcd.print(ghumidity);
    lcd.print("%");
    delay(2000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Caudal: ");
    lcd.print(gVolume);
    lcd.print("L/min");
    Drill_mng();
    Water_tbl_mng ();
    delay(2000);
    }
  }
  if (ghumidity < gHumiDry){
    flag_humidity = 1;
  } 
  else
  flag_humidity=0;
  gTank2_bit=gTank_bit;
  gDrill2_bit=gDrill_bit;
  gWater_tbl2_bit=gWater_tbl_bit;
  gVolume2_bit=gVolume_bit;
}

void msr_humidity (void){


  int gHumidity_bit = analogRead (13);

  if((abs(gHumidity_bit-gHumidity_bit2)>= 8)){
     ghumidity = map (gHumidity_bit,0,1023,0,100);
    Serial.print("\t\t Humidade ");
    Serial.println(ghumidity);
  }
  gHumidity_bit2=gHumidity_bit;


}
void watering(void)
{
       if (flag_humidity==1)
       {
    //send_message ();
       if(button==LOW)
     digitalWrite(24, HIGH);//turn_on_valve_watering();    
    while(gVolume<gVolume_min)
    {
      for(int i=0;i<6;i++){
      buzz(buzzer,2000,250+i*10);
      Serial.println ("Fuga de agua");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ALARME");
      lcd.setCursor(0,1);
      lcd.print("Fuga de Agua");
      delay(250-i*10);
      } 
      flagalarm2=1;
      msr_levels();
      digitalWrite(24, LOW);
    }
digitalWrite(24, HIGH);
      msr_humidity();
    flagalarm2=0;   
    delay (500);
    msr_levels();
       }
}

void Company_water_mng (long int Vol_final)
{
  while(gTank< Vol_final && (gDrill< Drill_min) && (gWater_tbl< Water_tbl_min))
  {
      Serial.println ("Liga agua companhia");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ALARME");
      lcd.setCursor(0,1);
      lcd.print("LIGA AGUA DA COMPANHIA");
          digitalWrite(22, HIGH);
       for(int k=0;k<6;k++){
         buzz(buzzer,2000,500);  
         delay(300);
         buzz(buzzer,1000,500);
         }      
      flagalarm1=1;
    msr_levels();
//turn_on_Company_valve();		
  }
  flagalarm1=0;
  digitalWrite(22, LOW);//turn_off_Company_valve();
}

void Drill_mng (void)
{
  if(gDrill > Drill_min && gTank <= Tank_min)
  {
    digitalWrite(28, HIGH);//turn_on_drill_valve();
    }
    else
    digitalWrite(28, LOW);//turn_off_drill_valve();
}

void Water_tbl_mng (void)
{
  if(gWater_tbl > Water_tbl_min && gTank <= Tank_min){

    digitalWrite(26, HIGH);//turn_on_water_tbl_valve();
    }
    else
    digitalWrite(26, LOW);//turn_off_water_tbl_valve();
}

void water_needed_calc (void){



  if (((real_time<2130 && real_time >=2100)||(real_time<5030 && real_time >= 5000)) && (gTank < 48000) )//em m3
  {
    //send_warning ();
    Company_water_mng (48000);
  }

  if (((real_time<2230 && real_time >=2200)||(real_time<6000 && real_time >= 5030))&& (gTank < 54000) )
  {
    //send_warning ();
    Company_water_mng (54000);
  }
  if (((real_time<2300 && real_time >=2230)||(real_time<6030 && real_time >= 6000)) && (gTank < Tank_max) )
  {
    //send_warning ();
    Company_water_mng (60000);
  }
}


void setup(){
  if(!Serial.available()){
pinMode(buzzer,OUTPUT);
  //uart
  Serial.begin(9600);
 TCCR2B = 0x00;        //Disbale Timer2 while we set it up
  TCNT2  = 130;         //Reset Timer Count to 130 out of 255
  TIFR2  = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
  TIMSK2 = 0x01;        //Timer2 INT Reg: Timer2 Overflow Interrupt Enable
  TCCR2A = 0x00;        //Timer2 Control Reg A: Normal port operation, Wave Gen Mode normal
  TCCR2B = 0x05;        //Timer2 Control Reg B: Timer Prescaler set to 128

  lcd.begin(16, 2);
  delay(1000);
  Serial.flush();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Sistema de Rega");
  lcd.setCursor(0,1);
  lcd.print("ISEP - 2014/2015");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Autores:");
  delay(2000);
  lcd.clear();
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayRight();
  }
  for(int i=0;i<20;i++){
  lcd.setCursor(0,0);
  lcd.print("Henrique Figueiredo");
  lcd.scrollDisplayLeft();
  delay(300);
   lcd.setCursor(0,1);
  lcd.print("    1120401");
  }
  delay(1000);
  lcd.clear();
   for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayRight(); 
  }
  for(int i=0;i<16;i++){
  lcd.setCursor(0,0);
  lcd.print("Ruben Bertelo");
  lcd.setCursor(0,1);
  lcd.scrollDisplayLeft();
  delay(300);
  lcd.print("    1120803");
  }
  delay(1000);
  lcd.clear();
  pinMode(10, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(28, OUTPUT);
  attachInterrupt(5, buttonsee, CHANGE);

  }

}

void serialRX() {
  while (Serial.available()) {
    int inByte = Serial.read();
   if (inByte == '0') {
  digitalWrite(22, LOW);
   }
   if (inByte == '1') {
  digitalWrite(22, HIGH);
      Serial.println ("Liga agua companhia");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ALARME");
      lcd.setCursor(0,1);
      lcd.print("LIGA AGUA DA COMPANHIA");
       for(int k=0;k<6;k++){
         buzz(buzzer,2000,500);  
         delay(300);
         buzz(buzzer,1000,500);
         }
      delay(1000);
   }
   if (inByte == '2') {
  digitalWrite(24, HIGH);
   flag_humidity=1;
   }
  if (inByte == '3') 
    digitalWrite(28, HIGH);
  if (inByte == '4')
    digitalWrite(26, HIGH);
  }
}
void loop(){

  msr_humidity();
  msr_levels();
  /*      while(Serial.available()){
   //delay(1000);
   char a= Serial.read();
   Serial.flush();
   realtime[0+i]=a;
   i++;
   if(a='\n'){
   delay(1000);
   real_time=atoi(realtime);
   Serial.println(real_time, DEC);
   Serial.flush();
   }
   }*/

    if(((real_time>7000)&&(real_time<8000))||((real_time>2300)&&(real_time<2399))){
      if ( gTank>45000){ //45 -> 75% do deposito...deve chegar para rega!
      msr_humidity ();
          if (flag_humidity == 1)
            watering ();
            if(flag_humidity==0)
             digitalWrite(24, LOW);	
    }
    if (( gTank<45000) && (gWater_tbl < Water_tbl_min) && (gDrill < Drill_min)){
       Company_water_mng (20000);
    }
  }
  //       

  if (gTank>Tank_min && gTank<Tank_max){

    if (gWater_tbl > Water_tbl_min){
        Water_tbl_mng ();
      }
      else
   digitalWrite(26, LOW);				 
    if (gDrill > Drill_min){
        Drill_mng ();
      }
      else
   digitalWrite(28, LOW);	
							 
    if ((gDrill < Drill_min) && (gWater_tbl < Water_tbl_min)){
      //water_needed_calc ();
    }

  }
  //se nivel do tank menor que os 25 
  if (gTank < Tank_min)
  {
    if (gWater_tbl > Water_tbl_min){
        Water_tbl_mng ();
     
    }

    if (gDrill > Drill_min){
        Drill_mng ();
      }
    if ((gDrill< Drill_min) && (gWater_tbl< Water_tbl_min))
    {
         
      Company_water_mng (Tank_min);
      
    }
      flagalarm2=0;


  }
}
