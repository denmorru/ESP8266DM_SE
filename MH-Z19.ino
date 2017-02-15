void readCO2() {
        // CO2
        bool header_found {false};
        char tries {0};

        UART_SERVER_PORT.write(cmd, 9);
        memset(response, 0, 7);

        // Looking for packet start
        while(UART_SERVER_PORT.available() && (!header_found)) {
                if(UART_SERVER_PORT.read() == 0xff ) {
                        if(UART_SERVER_PORT.read() == 0x86 ) header_found = true;
                }
        }

        if (header_found) {
                UART_SERVER_PORT.readBytes(response, 7);

                byte crc = 0x86;
                for (char i = 0; i < 6; i++) {
                        crc+=response[i];
                }
                crc = 0xff - crc;
                crc++;

                if ( !(response[6] == crc) ) {
                        COMM_DEBUG_PORT.println("CO2: CRC error: " + String(crc) + " / "+ String(response[6]));
                } else {
                        unsigned int responseHigh = (unsigned int) response[0];
                        unsigned int responseLow = (unsigned int) response[1];
                        unsigned int ppm = (256*responseHigh) + responseLow;
                        co2 = ppm;
                        COMM_DEBUG_PORT.println("CO2:" + String(co2));
                }
        } else {
                COMM_DEBUG_PORT.println("CO2: Header not found");
        }

}

void showLevelCO2(){
      webPageContent+="<div>";
      webPageContent+="<div style='width:100%;line-height:90px;float:left;'> ";
      webPageContent+="<a style='background-color:";
      if(co2<=400){webPageContent+="#00FF00;";}
      if(co2>400 and co2<=1000){webPageContent+="#FFFF00;";}
      if(co2>1000){webPageContent+="#FF0000;";}
      webPageContent+=buttonstyle;
      webPageContent+="'>";
      webPageContent+=co2;
      webPageContent+="</a>";
      webPageContent+="</div>";
      webPageContent+="</div>";  
}

