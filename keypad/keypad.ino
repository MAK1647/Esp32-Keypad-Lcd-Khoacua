
#include <Keypad.h>
#include "EEPROM.h"
#include<LiquidCrystal_I2C.h> 
#include <WiFi.h>
#include "HTTPClient.h"
LiquidCrystal_I2C lcd(0x27, 16, 2);
const byte ROWS = 4; //four rows
const byte COLS = 5; //four columns
char keys[ROWS][COLS] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte rowPins[ROWS] = {32,33,25,27}; 
byte colPins[COLS] = {18,19,12,14}; // tu row qua column
int i,j,lcd_count=10,count=1,key_id=0,flag,entry=10,f=0,ctrl=0,k=0,entry1=10,lcd_count1=10;
//lcd_count1,2: biến dịch con trỏ sang phải để nhập số
//count:biến reset lại vị trí trong các mảng check[]...
//flag : biến kiểm tra password ->0,1
//entry,entry1:biến kiểm tra để nhảy vào vong lậy đổi pass (nhấn D)
//ctrl : biến kiểm tra hệ thống đã vào hàm điều khiển chưa
int col_scan;                                              
char temp_press; 
//char key;
//char key1;

char check[6],pass[6]={'\0','1','2','3','4','5'},passcheck[6],passwrite[6]={'\0','0','0','0','0','0'},check2[6],passmaster[6]={'\0','1','5','9','6','3'};  
//check,check2:password khi nhập vào để điều khiển
//passmaster:pass mặc định khi reset , dùng để đổi pass khi quên pass
//passcheck:pass đọc ra từ eeprom
//passwrite:pass ghi vào eeprom
const char* ssid = "CRETA-KD";
const char* password =  "yoursolution";
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//byte ledPin = 2; 
int led0 = 34;
int BZ = 35;
long dem=0; //biến đếm để tự động thoát hàm controll

#define EEPROM_SIZE 64
int onho=1;//biến để reset gía trị ô nhớ trong eeprom
void setup(){
  lcd.init();
 // lcd.backlight();
  WiFi.begin(ssid, password);
  //Connect Wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    lcd.setCursor(0,0);
    lcd.print("KO CO WIFI");
    Serial.println("Connecting to WiFi..");
  }
    
     pinMode(led0, OUTPUT);
    pinMode(BZ, OUTPUT);
    //lcd.print("Hello, world!");
    lcd.setCursor(0,0);
    lcd.print("STATUS: LOCKED");
    lcd.setCursor(0,1);
    lcd.print("PASSWORD:");
    Serial.begin(9600);
    /////EEPROM
    EEPROM.begin(EEPROM_SIZE);
       
    EEPROM.commit();
    
}


void loop(){
  if ((WiFi.status() == WL_CONNECTED)){   // key_id=0;
    while(entry<=4)// Password Setting Loop begins
      {
         
         SetPassword();
      }
      while(entry1<=4)// Password Setting Loop begins
      {
         
         SetPassword2();
      }
    char key = keypad.getKey();
    controll(key);
    
    if (key) { //khi có phím nhấn
        Serial.println(key);
        keyscan(key);
        if(key=='0'||key=='1'||key=='2'||key=='3'||key=='4'||key=='5'||key=='6'||key=='7'||key=='8'||key=='9')
          {
            check[count]=key;
            count++;
            
          }
        if((count==6)&&(ctrl==0))
          {
            checkPassword();
            Serial.println(flag);
            
            if(flag==0)
              {
                f=0;
                k=0;
                lcd.clear();
                lcd.setCursor(5,0);
                lcd.print("A:UP");
                lcd.setCursor(0,1);
                lcd.print("B:STOP C:DOWN");
                ctrl=1;
                //digitalWrite(BZ, HIGH);
                delay(100);
               // digitalWrite(BZ, LOW);
               // delay(100);
                //digitalWrite(BZ, HIGH);
               // delay(100);
               // digitalWrite(BZ, LOW);
                lcd_count=10;
              }
            else
              { 
                
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("WRONG PASSWORD");
                digitalWrite(BZ, HIGH);
                delay(2000);
                digitalWrite(BZ, LOW);
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("STATUS: LOCKED");
                lcd.setCursor(0,1);
                lcd.print("PASSWORD:");           
                //f++;
                ctrl=0;
                 
                lcd_count=10;
              }
            count=1; // Resetting the counter variable
            
          }
          else if((key=='*')&&(ctrl==1))
          {
            lcd.setCursor(5,0);
            lcd.print("A:UP");
            lcd.setCursor(0,1);
            lcd.print("B:STOP C:DOWN");
            count=1;
          }
          else if((key=='D')&&(ctrl==0))
          {
            lcd.setCursor(0,0);
            lcd.print("NHAP MAT KHAU CU");
            lcd.setCursor(0,1);
            lcd.print("PASSWORD:"); 
            entry=0;
            count=1; // Resetting the counter variable
            
          }
          else if((key=='D')&&(ctrl==1))
          {
            lcd.setCursor(5,0);
            lcd.print("A:UP");
            lcd.setCursor(0,1);
            lcd.print("B:STOP C:DOWN");
            count=1;
          }
           else if((key=='#')&&(ctrl==0))//xoa
           {
            lcd.setCursor(0,0);
                lcd.print("STATUS: LOCKED");
                lcd.setCursor(0,1);
                lcd.print("PASSWORD:");    
                count=1;   
           }
           else if((key=='#')&&(ctrl==1))
            {
              lcd.setCursor(5,0);
               lcd.print("A:UP");
               lcd.setCursor(0,1);
               lcd.print("B:STOP C:DOWN");
               count=1;
            }
            else if((key=='*')&&(ctrl==0))
            {
               lcd.setCursor(0,0);
                lcd.print("STATUS: LOCKED");
                lcd.setCursor(0,1);
                lcd.print("PASSWORD:");    
                count=1;   
            }
          
          
         
          
    }
    
}
else
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("KO KET NOI WIFI");
  lcd.setCursor(0,1);
  lcd.print("RESETING");
  
  delay(10000);
 
  ESP.restart();
}
}
// Hàm dò phím bấm để tăng vị trí con trỏ màn hình thứ nhất

char keyscan(char temp_press)// Subroutine to Scan Keypad for a Keypress
  {
    
       
       
           
           
                key_id=1;
                
                //temp_press=keypress(i,j);
                lcd.setCursor(lcd_count++,1);
                if(lcd_count<=15)
                  {
                    if((temp_press=='0'||temp_press=='1'||temp_press=='2'||temp_press=='3'||temp_press=='4'||temp_press=='5'||temp_press=='6'||temp_press=='7'||temp_press=='8'||temp_press=='9')&&ctrl==0)
                      {
                        lcd.print('*');
                        entry++;
                      }
                     else if((temp_press=='0'||temp_press=='1'||temp_press=='2'||temp_press=='3'||temp_press=='4'||temp_press=='5'||temp_press=='6'||temp_press=='7'||temp_press=='8'||temp_press=='9')&&ctrl==1)
                     {
                       lcd_count=10;
                     }
                  }
                if(temp_press=='*'||temp_press=='D'||temp_press=='#')
                  {
                    
                    lcd.clear();
                    lcd_count=10;
                  }
                if(temp_press=='A'||temp_press=='B'||temp_press=='C')
                  { 
                                                       
                    lcd_count=10;
                    
                  }
                 
                  
                
                delay(300);
                
              
          
       
  }//Hàm dò phím bấm để tăng vị trí con trỏ màn hình thứ hai khi đổi pass
  char keyscan2(char temp_press)// Subroutine to Scan Keypad for a Keypress
  {
    
       
       
           
           
                key_id=1;
                
                //temp_press=keypress(i,j);
                lcd.setCursor(lcd_count1++,1);
                if(lcd_count1<=15)
                  {
                    if((temp_press=='0'||temp_press=='1'||temp_press=='2'||temp_press=='3'||temp_press=='4'||temp_press=='5'||temp_press=='6'||temp_press=='7'||temp_press=='8'||temp_press=='9')&&ctrl==0)
                      {
                        lcd.print('*');
                        entry1++;
                      }
                     else if((temp_press=='0'||temp_press=='1'||temp_press=='2'||temp_press=='3'||temp_press=='4'||temp_press=='5'||temp_press=='6'||temp_press=='7'||temp_press=='8'||temp_press=='9')&&ctrl==1)
                     {
                       lcd_count1=10;
                     }
                  }
                if(temp_press=='*'||temp_press=='D'||temp_press=='#')
                  {
                    
                    lcd.clear();
                    lcd_count1=10;
                  }
                if(temp_press=='A'||temp_press=='B'||temp_press=='C')
                  { 
                                                       
                    lcd_count1=10;
                    
                  }
                 
                  
                
                delay(300);
                
              
          
       
  }
  //Hàm kiểm tra password khi chưa vào đổ pass
void checkPassword() 
  {
    flag=0;
    passcheck[0] = EEPROM.read(0);//Đọc từ EEPROM
    passcheck[1] = EEPROM.read(1);
    passcheck[2] = EEPROM.read(2);
    passcheck[3] = EEPROM.read(3);
    passcheck[4] = EEPROM.read(4);
    passcheck[5] = EEPROM.read(5);
    Serial.println(check[1]);
    Serial.println( passcheck[1]);
    
    for(i=1;i<=5&&flag==0;i++)
      {
        if(check[i]==passcheck[i])
          {
            flag=0;
            check[i]=check[i]+1;
          }
        else 
          {
            flag=1;
          }
      }
  }
  //Hàm điều khiển cửa
  void controll(char temp_press2)
  {
    if (ctrl==0)
      {
        //digitalWrite(led0, LOW);
        delay(1); 
      }
    else
      {
        dem=dem+1;
        if (temp_press2=='A')
          {
            //digitalWrite(led0, LOW);  
            dem=0;
            HTTPClient http;
 
            http.begin("http://192.168.0.15:8000/control?user=3011&function=control&data=0"); //Specify the URL stop
            http.GET(); 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("CUA DANG MO");
            delay(2000);
            lcd.clear();
            lcd.setCursor(5,0);
               lcd.print("A:UP");
               lcd.setCursor(0,1);
               lcd.print("B:STOP C:DOWN");
            
          }
        else if (temp_press2=='B')
          {
           // digitalWrite(led0, HIGH);
            dem=0;
            HTTPClient http;
 
            http.begin("http://192.168.0.15:8000/control?user=3011&function=control&data=3"); //Specify the URL stop
            http.GET(); 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("CUA DANG DUNG"); 
            delay(2000);
            lcd.clear();
            lcd.setCursor(5,0);
               lcd.print("A:UP");
               lcd.setCursor(0,1);
               lcd.print("B:STOP C:DOWN");
          }      
        else if (temp_press2=='C')
          {
            //digitalWrite(led0, HIGH);
            dem=0;
            HTTPClient http;
 
            http.begin("http://192.168.0.15:8000/control?user=3011&function=control&data=1"); //Specify the URL stop
            http.GET(); 
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("CUA DANG DONG"); 
            delay(2000);
            lcd.clear();
            lcd.setCursor(5,0);
               lcd.print("A:UP");
               lcd.setCursor(0,1);
               lcd.print("B:STOP C:DOWN");
          }
         else if(dem == 5000000)
       {
        dem = 0;
        ctrl=0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("STATUS: LOCKED");
        lcd.setCursor(0,1);
        lcd.print("PASSWORD:");
     }        
      }
     
     }
     //Set pass word khi nhấn phím D
void SetPassword() 
  {
    char key1 = keypad.getKey();
    
    if(key1){ // lhi có phím nhấn
    keyscan(key1);
    if(key1=='0'||key1=='1'||key1=='2'||key1=='3'||key1=='4'||key1=='5'||key1=='6'||key1=='7'||key1=='8'||key1=='9')
          {
            check2[count]=key1;
            count++;
            
          }
    
       
         if((count==6)&&(ctrl==0))
          {
            checkPassword2();
            Serial.println(flag);
            
            if(flag==0)
              {
               lcd.clear();
                lcd.print("NEW PASSWORD");
                lcd.setCursor(0,1);
                lcd.print("5 DIGITS:");
               
                delay(100);
               
                entry1=0;
                entry=10;
                
                delay(100);
                lcd_count1=10;
              
              }
            else
              { 
                
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("WRONG PASSWORD");
               
                delay(2000);
               
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("STATUS: LOCKED");
                lcd.setCursor(0,1);
                lcd.print("PASSWORD:");           
                //f++;
                ctrl=0;
                entry=10;
                lcd_count1=10;
                lcd_count=10;
                 
                
              }
            count=1; // Reset lại biến đếm
            
          }
          else if((key1=='*')&&(ctrl==1))
          {
            lcd.clear();
                lcd.print("NEW PASSWORD");
                lcd.setCursor(0,1);
                lcd.print("5 DIGITS:");
            count=1;
          }
          else if((key1=='D')&&(ctrl==0))
          {
            lcd.clear();
                lcd.print("NEW PASSWORD");
                lcd.setCursor(0,1);
                lcd.print("5 DIGITS:");
            count=1; // Resetting the counter variable
            
          }
          else if((key1=='D')&&(ctrl==1))
          {
            lcd.clear();
                lcd.print("NEW PASSWORD");
                lcd.setCursor(0,1);
                lcd.print("5 DIGITS:");
            count=1;
          }
           else if((key1=='#')&&(ctrl==0))//xoa
           {
            lcd.clear();
                lcd.print("NEW PASSWORD");
                lcd.setCursor(0,1);
                lcd.print("5 DIGITS:"); 
                count=1;   
           }
           else if((key1=='#')&&(ctrl==1))
            {
              lcd.clear();
                lcd.print("NEW PASSWORD");
                lcd.setCursor(0,1);
                lcd.print("5 DIGITS:");
               count=1;
            }
            else if((key1=='*')&&(ctrl==0))
            {
               lcd.clear();
                lcd.print("NEW PASSWORD");
                lcd.setCursor(0,1);
                lcd.print("5 DIGITS:");
                count=1;   
            }

        
  } //Set pass word khi nhảy vào đổi pass
  }
  void SetPassword2() {
    char key2 = keypad.getKey();
    if(key2){ // khi có phím nhấn
      keyscan2(key2);
    
       
        if(key2=='A'||key2=='C'||key2=='B'||key2=='D'||key2=='*'||key2=='#') // Condition to Check for an Invalid Keypress 
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("INVALID KEYS");
            delay(1000);
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("NEW PASSWORD");
            lcd.setCursor(0,1);
            lcd.print("5 DIGITS:");
            entry1=0;
          }
       
    
        else 
          {
            passwrite[entry1]=key2;
            EEPROM.write(onho, passwrite[entry1]);//Ghi vào EEPROM
            //EEPROM.write(onho, passwrite[entry]);
            onho=onho+1;
            EEPROM.commit();

          }
      
   // key_id=0;
    if((entry1==5))
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("PASSWORD IS");
        lcd.setCursor(0,1);
        lcd.print("CHANGED");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("STATUS: LOCKED");
        lcd.setCursor(0,1);
        lcd.print("PASSWORD:");
        lcd_count=10;
        ctrl=0;
        onho=1;//set lai onho
      }
  }
  }
  // //Hàm kiểm tra password khi  vào đổi pass
  void checkPassword2() 
  {
    flag=0;
    passcheck[0] = EEPROM.read(0);
    passcheck[1] = EEPROM.read(1);
    passcheck[2] = EEPROM.read(2);
    passcheck[3] = EEPROM.read(3);
    passcheck[4] = EEPROM.read(4);
    passcheck[5] = EEPROM.read(5);
    //Serial.println(check[1]);
    //Serial.println( passcheck[1]);
    
    for(i=1;i<=5&&flag==0;i++)
      {
        if((check2[i]==passcheck[i])||(check2[i]==passmaster[i]))
          {
            flag=0;
            check2[i]=check2[i]+1;
          }
        else 
          {
            flag=1;
          }
      }
  }
  
