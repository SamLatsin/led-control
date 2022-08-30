    #include <DMD_STM32.h> 
    #include "st_fonts/UkrRusArial14.h";
    #include "gfx_fonts/GlametrixLight12pt7b.h"
    #include "gfx_fonts/GlametrixBold12pt7b.h"
    
    // SPI bus 2 is used 
    SPIClass dmd_spi(1);
    // serial port to which the Wi-Fi module is connected
    #define WIFI_SERIAL    Serial2
    // choice of the number of LED modules
    #define DISPLAYS_ACROSS 3
    #define DISPLAYS_DOWN 1
    // Matrix pin selection
    #define DMD_PIN_A PB11
    #define DMD_PIN_B PB12
    #define DMD_PIN_nOE PB1
    #define DMD_PIN_SCLK PB10
    
    DMD dmd(DMD_PIN_A, DMD_PIN_B, DMD_PIN_nOE, DMD_PIN_SCLK, DISPLAYS_ACROSS, DISPLAYS_DOWN, dmd_spi );
    // font selection
    DMD_Standard_Font UkrRusArial_F(UkrRusArial_14);
    DMD_GFX_Font GlametrixBold((uint8_t*)&GlametrixBold12pt7b,(uint8_t*)&GlametrixBold12pt8b_rus, 0x80, 13); 

    // text conversion function
    int utf8_rus(char* dest, String src) {
      uint8_t i, j;
      for ( i =0, j =0; src[i]; i++) {
       if ((src[i] == 0xD0 )&& src[i+1])  { dest[j++] = src[++i] - 0x10;}
        else if ((src[i] == 0xD1 )&& src[i+1]) {dest[j++] = src[++i] + 0x30;  }
        else dest[j++] = src[i];
      }
      dest[j] ='\0';
      return j;
    }

    // command execution function
    void execute_command(String cmd, int t)
    {
      Serial2.println(cmd);
      delay(t);
    }

    // hotspot trigger function
    void host_ap()
    {
      execute_command("AT",100);
      execute_command("AT+CWMODE=2", 100);
      execute_command("AT+CWSAP=\"LED_CONTROL\",\"1234567890\",5,3", 100);
      execute_command("AT+RST", 5000);
      execute_command("AT+CIPMUX=1", 100);
      execute_command("AT+CIPSERVER=1,8888", 100);
      execute_command("AT+CIFSR", 100);
    }

    // brightness change function
    void change_brightness(int brightness)
    {
      dmd.setBrightness(brightness);
    }

    // String change function
    void change_string(String new_string)
    {
      dmd.clearScreen( true );
      char k[30];
      utf8_rus(k,new_string);
      dmd.drawMarquee(k,strlen(k),(32*DISPLAYS_ACROSS)-1,0);
    }
    
    void setup()
    {
      // open a serial port to monitor actions in the program
      // and transmit the speed of 9600 baud
      dmd.init();
      dmd.clearScreen( true );   // turn off all pixels
      // setting the brightness of the matrix (0-255)
      dmd.setBrightness(20);
      Serial.begin(9600);
      Serial.print("Serial init OK\r\n");
      // open Serial connection with Wi-Fi module at 115200 baud
      WIFI_SERIAL.begin(9600);
      // creating an access point
      host_ap();
    }
     
    void loop()
    {
      // select initial string to display at startup
      String m = "TEST STRING!";
      char k[30];
      dmd.selectFont(&GlametrixBold);
      // text conversion
      utf8_rus(k,m);
      dmd.drawMarquee(k,strlen(k),(32*DISPLAYS_ACROSS)-1,0);
      long prev_step = millis();
      // setting the initial speed of the ticker display
      int draw_speed = 30;
      // start of the main loop
      while(1){
        // line shift at a given speed
         if ((millis() - prev_step) > draw_speed ) {
           dmd.stepMarquee(-1,0);
           prev_step=millis();
         }
         // if data comes from the Wi-Fi module, we will send it to the computer port
         if (WIFI_SERIAL.available()) {
           String str = WIFI_SERIAL.readString();
           // command recognition
           if (str.startsWith("\r\n+IPD")) {
             String cmd = str.substring(str.indexOf(':')+1);
             // if a line change command is received
             if(cmd.startsWith("/change_string("))
             {
               Serial.println("Choosing new string:");
               String new_string = cmd.substring(cmd.indexOf('(')+1, cmd.indexOf(')'));
               change_string(new_string);
               Serial.println(new_string);
             }
             // if a brightness change command is received
             if(cmd.startsWith("/change_brightness("))
             {
               Serial.println("Changing brightness to:");
               int brightness = cmd.substring(cmd.indexOf('(')+1, cmd.indexOf(')')).toInt();
               change_brightness(brightness);
               Serial.println(cmd.substring(cmd.indexOf('(')+1, cmd.indexOf(')')));
             }
             // if a speed change command is received
             if(cmd.startsWith("/change_speed("))
             {
               Serial.println("Changing brightness to:");
               draw_speed = cmd.substring(cmd.indexOf('(')+1, cmd.indexOf(')')).toInt();
               Serial.println(cmd.substring(cmd.indexOf('(')+1, cmd.indexOf(')')));
             }
           } 
         }
        // if data comes from the computer - send it to the Wi-Fi module
        if (Serial.available()) {
          WIFI_SERIAL.write(Serial.read());
        }
       }
    }
