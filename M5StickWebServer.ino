#include <M5StickCPlus.h>
#include <WiFi.h>
const char* ssid = "Lxxxxxx"; /* Add your router's SSID */
const char* password = "xxxxxxx"; /*Add the password */

int screenValue; 


WiFiServer espServer(80); /* Instance of WiFiServer with port number 80 */
/* 80 is the Port Number for HTTP Web Server */

/* A String to capture the incoming HTTP GET Request */
String request;

void setup()
{
  M5.begin();
  pinMode(M5_BUTTON_HOME, INPUT_PULLUP);
  pinMode(M5_BUTTON_RST, INPUT_PULLUP);
  M5.Axp.ScreenBreath(10);
  Serial.begin(115200); /* Begin Serial Communication with 115200 Baud Rate */
  M5.Lcd.fillScreen(BLACK);
  
  Serial.print("\n");
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA); /* Configure ESP32 in STA Mode */
  WiFi.begin(ssid, password); /* Connect to Wi-Fi based on the above SSID and Password */
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("*");
    delay(100);
  }
  Serial.print("\n");
  Serial.print("Connected to Wi-Fi: ");
  Serial.println(WiFi.SSID());
  delay(100);
  /* The next four lines of Code are used for assigning Static IP to ESP32 */
  /* Do this only if you know what you are doing */
  /* You have to check for free IP Addresses from your Router and */
  /* assign it to ESP32 */
  /* If you are comfortable with this step, */
  /* please un-comment the next four lines and make necessary changes */
  /* If not, leave it as it is and proceed */
  //IPAddress ip(192,168,1,6);   
  //IPAddress gateway(192,168,1,1);   
  //IPAddress subnet(255,255,255,0);   
  //WiFi.config(ip, gateway, subnet);
  delay(2000);
  Serial.print("\n");
  Serial.println("Starting ESP32 Web Server...");
  espServer.begin(); /* Start the HTTP web Server */
  Serial.println("ESP32 Web Server Started");
  Serial.print("\n");
  Serial.print("The URL of ESP32 Web Server is: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.print("\n");
  Serial.println("Use the above URL in your Browser to access ESP32 Web Server\n");
}

void loop()
{
  M5.update();
  WiFiClient client = espServer.available(); /* Check if a client is available */
  if(!client)
  {
    return;
  }

  Serial.println("New Client!!!");
  boolean currentLineIsBlank = true;
  while (client.connected())
  {
    if (client.available())
    {
      char c = client.read();
      request += c;
      Serial.write(c);
        /* if you've gotten to the end of the line (received a newline */
        /* character) and the line is blank, the http request has ended, */
        /* so you can send a reply */
      if (c == '\n' && currentLineIsBlank)
      {
        /* Extract the URL of the request */
        /* We have four URLs. If IP Address is 192.168.1.6 (for example),
        * then URLs are: 
        * 192.168.1.6/GPIO16ON
        * 192.168.1.6/GPIO16OFF
        * 192.168.1.6/GPIO17ON
        * 192.168.1.6/GPIO17OFF
        */
        /* Based on the URL from the request, turn the LEDs ON or OFF */
        if (request.indexOf("/screenRed") != -1) 
        {
          Serial.println("Pantalla vermella");
          M5.Lcd.fillScreen(RED);
          screenValue = 0;
        } 
        if (request.indexOf("/screenBlue") != -1)
        {
          Serial.println("Pantalla azul");
          M5.Lcd.fillScreen(BLUE);
          screenValue = 1;
        }
//Isto de abaixo non funcionou
//        if (request.indexOf("/screenBlack") != -1)
//        {
//          Serial.println("Pantalla negra");
//          M5.Lcd.fillScreen(BLACK);
//          screenValue = 2;
//        }
        
        
        /* HTTP Response in the form of HTML Web Page */
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println(); //  IMPORTANT

        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        
        client.println("<head>");
        client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
        client.println("<link rel=\"icon\" href=\"data:,\">");
        
        client.println("<style>");
        
        client.println("html { font-family: Courier New; display: inline-block; margin: 0px auto; text-align: center;}");
        client.println(".button {border: none; color: white; padding: 10px 20px; text-align: center;");
        client.println("text-decoration: none; font-size: 25px; margin: 2px; cursor: pointer;}");
        client.println(".button1 {background-color: #FF0000;}");
        client.println(".button2 {background-color: #0000FF;}");
        //isto non saiu ben
        //client.println(".button3 {background-color: #000000;}");
        
        client.println("</style>");
        
        client.println("</head>");
        
        client.println("<body>");
        
        client.println("<h2>ESP32 Web Server</h2>");
        
        if(screenValue == 1) 
        {
          client.println("<p>screen: Azul</p>");
          client.print("<p><a href=\"/screenRed\"><button class=\"button button1\">Click to turn Vermella</button></a></p>");  
        } 
        else 
        {
          client.println("<p>screen: Vermella</p>");
          client.print("<p><a href=\"/screenBlue\"><button class=\"button button2\">Click to turn Azul</button></a></p>"); 
        }
        //E isto tampouco saiu
//        else
//        {
//          client.println("<p>screen: Negra</p>");
//          client.print("<p><a href=\"/screenBlack\"><button class=\"button button3\">Click to turn Black</button></a></p>");
//          }
        
             
        client.println("</body>");
        
        client.println("</html>");
        
        break;
        
    }
    if(c == '\n')
    {
      currentLineIsBlank = true;
    }
    else if(c != '\r')
    {
      currentLineIsBlank = false;
    }
    //client.print("\n");
    
    }
  }
 
  delay(1);
  request = "";
  //client.flush();
  client.stop();
  Serial.println("Client disconnected");
  Serial.print("\n");
}
