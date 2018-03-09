#include <TimeLib.h>

#include <adafruit_feather.h>
#include <adafruit_http.h>
#include <iwdg.h>

#define WLAN_SSID            "YOUR SSID"
#define WLAN_PASS            "YOUR PASS"

#define SERVER               "engage.efergy.com"     // The TCP server to connect to
#define PAGE                 "/mobile_proxy/getCurrentValuesSummary?token=<YOUR TOKEN>" // The HTTP resource to request
#define PORT                 443                     // The TCP port to use

#define USER_AGENT_HEADER    "curl/7.45.0"
int ledPin = PA15;

// Use the HTTP class
AdafruitHTTP http;
String response;

int consumption = 0;
int production = 0;
int gain = 0;
bool conn = false;

int last_response = -1;

/**************************************************************************/
/*!
    @brief  TCP/HTTP received callback
*/
/**************************************************************************/
void receive_callback(void)
{
  // If there are incoming bytes available
  // from the server, read then print them:
  response = "";
  bool squareBracketFound = false;
  int squareBracketCount = 0;
  
  while ( http.available() )
  {
    int c = http.read();
    //Serial.write( (isprint(c) || iscntrl(c)) ? ((char)c) : '.');
    if( (char)c == '[') {
      squareBracketFound = true;
      squareBracketCount++;
    }
    if( squareBracketFound && (char)c == ']') {
      squareBracketCount--;
    }
    
    if(squareBracketFound && squareBracketCount>0)
      if( isprint(c) || iscntrl(c) )
        response += ((char)c);
  }
  
  Serial.println("-----\r\n" + response + "\r\n-------\r\n");

  int ind1 = response.indexOf("}],\"sid\"");  
  String s1 = response.substring(0, ind1);   
  String s2 = response.substring(ind1+8, response.length());   
  int ind2 = s2.indexOf("}],\"sid\"");   
  String s3 = s2.substring(0, ind2);
  int i=0, j=0;   
  for(i=s1.length()-1;i>=0;i--){
    if(s1[i]==':')
      j=i;
    if(s1[i]=='{')
      break;
  }
  uint32_t epoch = s1.substring(i+2,j-4).toInt();
  s1 = s1.substring(j+1,s1.length());
  //Serial.println(epoch + "!!");

  i=0;   
  for(i=s3.length()-1;i>=0;i--){
    if(s3[i]==':')
      break;
  }
  s3 = s3.substring(i+1,s3.length());
  //Serial.println(s3 + ".");

  if(s2.indexOf("PWER_GAC")>=0){
    consumption = s1.toInt();
    production = s3.toInt();
  }
  else{
    consumption = s3.toInt();
    production = s1.toInt();
  }

  gain = production - consumption;
  
  Serial.print("CONSUMPTION: ");
  Serial.println(consumption);
  Serial.print("PRODUCTION: ");
  Serial.println(production);
  Serial.print("GAIN: ");
  Serial.println(gain);
  Serial.println("-----\r\n");

  //epoch=epoch-(3600*8)  ; //ADD 1 Hours (For GMT+1)
  epoch=epoch+3600  ; //ADD 1 Hours (For GMT+1)
  //epoch=epoch+7200  ; //ADD 2 Hours (For GMT+2)

  Serial.println(epoch);
  // print the hour, minute and second:
  int hr=hour(epoch);
  int minu=minute(epoch);

  Serial.print("HOUR: ");
  Serial.println(hr);
  Serial.print("MINUTE: ");
  Serial.println(minu);

  uint16_t hm = (hr*100 + minu);
  Serial.println(hm);

  if(hr>=7 && hr<=23){
    setDisplayR(consumption);
    setDisplayB(production);
    //setDisplayGHour(hm);
    //setDisplayG(gain>0?gain:0);
  
    if(gain>0){
      setLedGreen();
      setDisplayG(gain);
    }
    else{
      if(production<65)
        setLedWhite();
      else
        setLedRed(); 
      setDisplayGHour(hm);
    }
  }else{
      Serial.println("SLEEP!!!!!!");
      turnOffAllDisplay();  
      turnOffLed();  
  }  

  last_response = millis();
}

/**************************************************************************/
/*!
    @brief  TCP/HTTP disconnect callback
*/
/**************************************************************************/
void disconnect_callback(void)
{
  Serial.println();
  Serial.println("---------------------");
  Serial.println("DISCONNECTED CALLBACK");
  Serial.println("---------------------");
  Serial.println();
  
  //http.stop();

  conn = false;

  //_setup();
  //_loop();
  
}

/**************************************************************************/
/*!
    @brief  The setup function runs once when the board comes out of reset
*/
/**************************************************************************/
void setup()
{
  Serial.begin(115200);
/** /
  // Wait for the Serial Monitor to open
  while (!Serial)
  {
    delay(1);
  }
/**/
  
  setupLed();
  setupDisplay();

  Serial.println("HTTP Get Example (Callback Based)\r\n");

  // Print all software versions
  Feather.printVersions();
  iwdg_init(IWDG_PRE_128, 11500000);
  
  _setup();
}

void _setup(){
  // Try to connect to an AP
  while ( !connectAP() )
  {
    delay(500); // delay between each attempt
  }

  iwdg_feed();
  
  // Connected: Print network info
  Feather.printNetwork();

  // Tell the HTTP client to auto print error codes and halt on errors
  http.err_actions(true, false);

  // Set the callback handlers
  http.setReceivedCallback(receive_callback);
  http.setDisconnectCallback(disconnect_callback);

  // Connect to the HTTP server
  Serial.printf("Connecting to %s port %d ... ", SERVER, PORT);
  if(http.connectSSL(SERVER, PORT)) // Will halt if an error occurs
    Serial.println("OK");
  else
    setDisplayError();

  iwdg_feed();
  conn = true;
  // Setup the HTTP request with any required header entries
  //http.addHeader("User-Agent", USER_AGENT_HEADER);
  //http.addHeader("Accept", "text/html");
  //http.addHeader("Connection", "keep-alive");

}

/**************************************************************************/
/*!
    @brief  The loop function runs over and over again
*/
/**************************************************************************/
int counter = 1;
void loop()
{
  _loop();
}

void _loop(){
  while(1){
    iwdg_feed();
    
    if(!conn){
      Serial.println("->LOOP RESETUP()");
      _setup(); 
    }

    Serial.print("LAST RESPONSE: ");
    Serial.print(millis());
    Serial.print(" - ");
    Serial.println(last_response);
    
    if(last_response>0 && millis()-last_response>30000)
      while(1);
    if(millis()-last_response>60000)
      while(1);
  /*
    if(counter==21)
    {
      http.stop();
      _setup();
      counter=1;
      //break;
    }
    */  
    if(gain<=0){
      setDisplayGColon(counter%2==0);
    }
    
    if(counter%10==0){
      Serial.printf("Requesting '%s' ... ", PAGE);
      if(http.get(SERVER,PAGE)) // Will halt if an error occurs
        Serial.println("OK");
        
      //counter=1;
    }
    
    counter++;
    delay(1000);
  }
}

/**************************************************************************/
/*!
    @brief  Connect to the defined access point (AP)
*/
/**************************************************************************/
bool connectAP(void)
{
  // Attempt to connect to an AP
  Serial.print("Please wait while connecting to: '" WLAN_SSID "' ... ");

  if ( Feather.connect(WLAN_SSID, WLAN_PASS) )
  {
    Serial.println("Connected!");
  }
  else
  {
    Serial.printf("Failed! %s (%d)", Feather.errstr(), Feather.errnum());
    Serial.println();
  }
  Serial.println();

  return Feather.connected();
}
