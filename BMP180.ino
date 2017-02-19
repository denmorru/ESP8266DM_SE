void BMP180_read() {
       char status;
       double Tp;
       double Pp;  //hPa
       BMP180d = 0; 
       status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(Tp);
    if (status != 0)
    {
      COMM_DEBUG_PORT.print("temperature: ");
      COMM_DEBUG_PORT.print(Tp,2);
      COMM_DEBUG_PORT.print(" deg C, ");
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(Pp,Tp);
        if (status != 0)
        {
          COMM_DEBUG_PORT.print("absolute pressure: ");
          COMM_DEBUG_PORT.print(Pp,2);
          COMM_DEBUG_PORT.print(" hPa (mb), ");
          COMM_DEBUG_PORT.print(Pp*0.750061561303,2);
          COMM_DEBUG_PORT.println(" mmHg");
          BMP180d =Pp*0.75014766658; //mmHg
        }
        else COMM_DEBUG_PORT.println("error retrieving pressure measurement\n");
      }
      else COMM_DEBUG_PORT.println("error starting pressure measurement\n");
    }
    else COMM_DEBUG_PORT.println("error retrieving temperature measurement\n");
  }
else COMM_DEBUG_PORT.println("error starting temperature measurement\n");
   if (BMP180d>0) { 
            COMM_DEBUG_PORT.println("BMP180d:" + String(BMP180d));
        } else {
            COMM_DEBUG_PORT.println("BMP180d: read failed");
        }

}

void BMP180_showLevel(){
      webPageContent+="<div style='width:100%;line-height:90px;float:left;'>BMP180: ";      
      webPageContent+="</div>";  
      webPageContent+="<div style='width:100%;line-height:90px;float:left;'> ";
      webPageContent+="<a style='background-color:";
      if(BMP180d<=0){webPageContent+="#999999;";}
      if(BMP180d>0 and BMP180d<=700){webPageContent+="#FFFF00;";}
      if(BMP180d>700 and BMP180d<=800){webPageContent+="#00FF00;";}
      if(BMP180d>800){webPageContent+="#FF0000;";}
      webPageContent+=buttonstyle;
      webPageContent+="'>";
      webPageContent+=BMP180d;
      webPageContent+="</a>";
      webPageContent+="</div>";
 
}

