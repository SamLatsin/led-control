    #include <DMD_STM32.h> 
    #include "st_fonts/UkrRusArial14.h";
    #include "gfx_fonts/GlametrixLight12pt7b.h"
    #include "gfx_fonts/GlametrixBold12pt7b.h"
    
    // Используется шина SPI 2   
    SPIClass dmd_spi(1);
    // serial-порт к которому подключён Wi-Fi модуль
    #define WIFI_SERIAL    Serial2
    // выбор количества LED модулей
    #define DISPLAYS_ACROSS 3
    #define DISPLAYS_DOWN 1
    // Выбор пинов матрицы
    #define DMD_PIN_A PB11
    #define DMD_PIN_B PB12
    #define DMD_PIN_nOE PB1
    #define DMD_PIN_SCLK PB10
    
    DMD dmd(DMD_PIN_A, DMD_PIN_B, DMD_PIN_nOE, DMD_PIN_SCLK, DISPLAYS_ACROSS, DISPLAYS_DOWN, dmd_spi );
    // выбор шрифтов
    DMD_Standard_Font UkrRusArial_F(UkrRusArial_14);
    DMD_GFX_Font GlametrixBold((uint8_t*)&GlametrixBold12pt7b,(uint8_t*)&GlametrixBold12pt8b_rus, 0x80, 13); 

    // функция конвертации текста
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

    // функция выполнения команды
    void execute_command(String cmd, int t)
    {
      Serial2.println(cmd);
      delay(t);
    }

    // функция запуска точки доступа
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

    // функция смены яркости
    void change_brightness(int brightness)
    {
      dmd.setBrightness(brightness);
    }

    // функция смены строки
    void change_string(String new_string)
    {
      dmd.clearScreen( true );
      char k[30];
      utf8_rus(k,new_string);
      dmd.drawMarquee(k,strlen(k),(32*DISPLAYS_ACROSS)-1,0);
    }
    
    void setup()
    {
      // открываем последовательный порт для мониторинга действий в программе
      // и передаём скорость 9600 бод
      dmd.init();
      dmd.clearScreen( true );   //выключить все пиксели
      // установка яркости матрицы (0-255)
      dmd.setBrightness(20);
      Serial.begin(9600);
      Serial.print("Serial init OK\r\n");
      // открываем Serial-соединение с Wi-Fi модулем на скорости 115200 бод
      WIFI_SERIAL.begin(9600);
      // создание точки доступа
      host_ap();
    }
     
    void loop()
    {
      // выбор изначальной строки для отображения при запуске
      String m = "ТЕСТОВАЯ СТРОКА!";
      char k[30];
      dmd.selectFont(&GlametrixBold);
      // конвертация текста
      utf8_rus(k,m);
      dmd.drawMarquee(k,strlen(k),(32*DISPLAYS_ACROSS)-1,0);
      long prev_step = millis();
      // установка начальной скорости отображения бегущей строки
      int draw_speed = 30;
      // начало основного цикла
      while(1){
        // сдвиг строки с заданной скоростью
         if ((millis() - prev_step) > draw_speed ) {
           dmd.stepMarquee(-1,0);
           prev_step=millis();
         }
         // если приходят данные из Wi-Fi модуля - отправим их в порт компьютера
         if (WIFI_SERIAL.available()) {
           String str = WIFI_SERIAL.readString();
           // распознование поступившей команды
           if (str.startsWith("\r\n+IPD")) {
             String cmd = str.substring(str.indexOf(':')+1);
             // если поступила команда смены строки
             if(cmd.startsWith("/change_string("))
             {
               Serial.println("Choosing new string:");
               String new_string = cmd.substring(cmd.indexOf('(')+1, cmd.indexOf(')'));
               change_string(new_string);
               Serial.println(new_string);
             }
             // если поступила команда смены яркости
             if(cmd.startsWith("/change_brightness("))
             {
               Serial.println("Changing brightness to:");
               int brightness = cmd.substring(cmd.indexOf('(')+1, cmd.indexOf(')')).toInt();
               change_brightness(brightness);
               Serial.println(cmd.substring(cmd.indexOf('(')+1, cmd.indexOf(')')));
             }
             // если поступила команда смены скорости
             if(cmd.startsWith("/change_speed("))
             {
               Serial.println("Changing brightness to:");
               draw_speed = cmd.substring(cmd.indexOf('(')+1, cmd.indexOf(')')).toInt();
               Serial.println(cmd.substring(cmd.indexOf('(')+1, cmd.indexOf(')')));
             }
           } 
         }
        // если приходят данные из компьютера - отправим их в Wi-Fi модуль
        if (Serial.available()) {
          WIFI_SERIAL.write(Serial.read());
        }
       }
    }
