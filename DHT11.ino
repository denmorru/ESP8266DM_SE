void DHT11_read() {
      h = dht.readHumidity();
      t = dht.readTemperature();
      float t0= -273.15;
      if (isnan(h) || isnan(t)) {
        t= t0;
        h= -1;
      }else{
      COMM_DEBUG_PORT.print("DHT11 > temperature: ");
      COMM_DEBUG_PORT.print(t,2);
      COMM_DEBUG_PORT.print(" deg C, ");
      COMM_DEBUG_PORT.print("humidity: ");
      COMM_DEBUG_PORT.print(h,2);
      COMM_DEBUG_PORT.print(" %, ");
      }
        if ( t > t0) { 
            COMM_DEBUG_PORT.println("DHT11 read done");
        } else {
            COMM_DEBUG_PORT.println("DHT11 read failed");
        }

}

void DHT11_showLevel(){
      webPageContent+="<div style='width:100%;line-height:90px;float:left;' device='DHT11' sensor='temperature' value='";
      webPageContent+=t;
      webPageContent+="'>";
      webPageContent+="<a style='background-color:";
      if(t<=0){webPageContent+="#999999;";}
      if(t>0 and t<=23){webPageContent+="#FFFF00;";}
      if(t>23 and t<=28){webPageContent+="#00FF00;";}
      if(t>28){webPageContent+="#FF0000;";}
      webPageContent+=buttonstyle;
      webPageContent+="'>DHT11 temperature: ";
      webPageContent+=t;
      webPageContent+="</a>";
      webPageContent+="</div>";
      webPageContent+="<div style='width:100%;line-height:90px;float:left;' device='DHT11' sensor='humidity' value='";
      webPageContent+=h;
      webPageContent+="'>";
      webPageContent+="<a style='background-color:";
      if(h<=0){webPageContent+="#999999;";}
      if(h>0 and h<=40){webPageContent+="#FFFF00;";}
      if(h>40 and h<=60){webPageContent+="#00FF00;";}
      if(h>60){webPageContent+="#FF0000;";}
      webPageContent+=buttonstyle;
      webPageContent+="'>DHT11 humidity: ";
      webPageContent+=h;
      webPageContent+="</a>";
      webPageContent+="</div>";
}

