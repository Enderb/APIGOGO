/*ICE #1A
   Using HCDE_IoT_4_GeoLocation.ino as a starting point, augment the sketch to include
   a new data type (a struct) called MetData designed to contain name:value pairs from
   http://openweathermap.org/. You will need to get an API key to use this service, the
   key is free for low volume development use. Get your key at http://openweathermap.org/api
   by subscribing to the Current Weather Data api. Examples of the API call are provided in
   the API documnentation.

   Wrap your HTTP request in a function called getMet(), which will be similar to getIP()
   and getGeo(), and print out the data as a message to the Serial Monitor using Imperial
   units. You should report temperature, humidity, windspeed, wind direction, and cloud
   conditions. Call the API by city name.
   
   One possible solution is provided below, though you should try to find your own solution
   first based upon previous examples.
*/

/*ICE #1B
   To complete this ICE, demonstrate your understanding of this sketch by running
   the code and providing complete inline commentary, describing how each line of
   code functions in detail. The completed project will comment every line. Some of
   this has already been done for you in earlier progressions of this project.

   This sketch . . . (briefly relate what this sketch does here)
   
   If your code differs from this solution, fine. Just comment that code thoroughly.
*/

//These are the libraries we are using in this sketch
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

//Provide network SSID
const char* ssid = "University of Washington";
//Provide the network password
const char* pass = "";

//Provide the openweathermap API key
String weatherKey = "fc112caaa3b9a2b5013f3099074252a9";
String lastfmkey = "b69e7ed240c2d62c4d2a5d14d0fd8166";
String wsdotAccessCode = "20b50f16-95fc-4aa1-9ce5-ebeff14fe7dc";

//This is a struct storing geolocation data
typedef struct {
  String ip;
  String cc;
  String cn;
  String rc;
  String rn;
  String cy;
  String tz;
  String ln;
  String lt;
} GeoData;

//This is a struct storing weather data
typedef struct {
  String tp;
  String pr;
  String hd;
  String ws;
  String wd;
  String cd;
} MetData;

//This is a struct for storing top tracks data
typedef struct {
  String t1;
  String a1;
  String t2;
  String a2;
  String t3;
  String a3;
  String t4;
  String a4;
  String t5;
  String a5;  
} TopTracks;

//This is a struct for storing commute data
typedef struct {
  String starting;
  String ending;
  String average;
  String commTime;
} Commute;

//Initialize variables based on the structs above
GeoData location;
MetData conditions;
TopTracks tracks;
Commute comm;

void setup() {
  //Set baud rate to 115200
  Serial.begin(115200);
  //Delay for 10 milliseconds
  delay(10);

  //Print network we are connecting to
  Serial.print("Connecting to "); Serial.println(ssid);

  //Set up wifi mode, send in network data, and connect
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  //Loop while we fail to connect to the wifi, printing a dot each time
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Print when successfully connected to wifi
  Serial.println(); Serial.println("WiFi connected"); Serial.println();
  //Print the internal IP address of the device
  Serial.print("Your ESP has been assigned the internal IP address ");
  //Print the internal IP address of the device
  Serial.println(WiFi.localIP());

  //Call the getIP() function to get the external IP address, store it in a string, and get the coordinates of it with the getGeo() function
  String ipAddress = getIP();
  getGeo(ipAddress);

  //Print an empty line
  Serial.println();

  //Print external IP address
  Serial.println("Your external IP address is " + location.ip);
  //Print the country the device is in
  Serial.print("Your ESP is currently in " + location.cn + " (" + location.cc + "),");
  //Print approximate city and state
  Serial.println(" in or near " + location.cy + ", " + location.rc + ".");
  //Print timezone
  Serial.print("You are in the " + location.tz + " timezone ");
  //Print rough coordinates
  Serial.println("and located at (roughly) ");
  //Print rough coordinates
  Serial.println(location.lt + " latitude by " + location.ln + " longitude.");

  //Use the getMet() function to get weather data, by passing in the approximate city
  getTracks();

  //Print a blank string
  Serial.println();
  //Print the weather conditions in the approximate city, and the city
  Serial.println("With " + conditions.cd + ", the temperature in " + location.cy + ", " + location.rc);
  //Print the temperature, humidy, and wind conditions
  Serial.println("is " + conditions.tp + "F, with a humidity of " + conditions.hd + "%. The winds are blowing");
  //Continue printing wind conditions, including speed
  Serial.println(conditions.wd + " at " + conditions.ws + " miles per hour, and the ");
  //Print the barometric conditions
  Serial.println("barometric pressure is at " + conditions.pr + " millibars.");
}

void loop() {
}

//Function to get tracks data from Last.FM
void getTracks() {
  //Initialize and HTTP Client object
  HTTPClient theClient;

  //API CALL: http://ws.audioscrobbler.com/2.0/?method=geo.gettoptracks&country=venezuela&limit=10&api_key=b69e7ed240c2d62c4d2a5d14d0fd8166&format=json
  //Initialize a string to build the API call to Last.FM
  String apiCall = "http://ws.audioscrobbler.com/2.0/?method=geo.gettoptracks";
  //Add city to API call, and limit to 10 tracks
  apiCall += "&country=venezuela&limit=10";
  //Building API call
  apiCall += "&api_key=";
  //Store the API key
  apiCall += lastfmkey;
  //Get in JSON format
  apiCall += "&format=json";
  
  //Make API call
  theClient.begin(apiCall);
  //Check HTTP response
  int httpCode = theClient.GET();
  //Check if somebody came back
  if (httpCode > 0) {
    //Check if header response is OK
    if (httpCode == HTTP_CODE_OK) {
      //Store the payload from open weather map
      String payload = theClient.getString();
      //Initialize a JSON buffer
      DynamicJsonBuffer jsonBuffer;
      //Parse payload and turn into a JSON object
      JsonObject& root = jsonBuffer.parseObject(payload);
      //Check if we failed to get a JSON object, and print a message
      if (!root.success()) {
        Serial.println("parseObject() failed in getTracks().");
        return;
      }

      //Store the first track name
      tracks.t1 = root["tracks"]["track"][0]["name"].as<String>();

      //Store the first artist name
      tracks.a1 = root["tracks"]["track"][0]["artist"]["name"].as<String>();

      //Store the second track name
      tracks.t2 = root["tracks"]["track"][1]["name"].as<String>();

      //Store the second artist name
      tracks.a2 = root["tracks"]["track"][1]["artist"]["name"].as<String>();

      //Store the third track name
      tracks.t3 = root["tracks"]["track"][2]["name"].as<String>();

      //Store the third artist name
      tracks.a3 = root["tracks"]["track"][2]["artist"]["name"].as<String>();

      //Store the fourth track name
      tracks.t4 = root["tracks"]["track"][3]["name"].as<String>();

      //Store the fourth artist name
      tracks.a4 = root["tracks"]["track"][3]["artist"]["name"].as<String>();

      //Store the fifth track name
      tracks.t5 = root["tracks"]["track"][4]["name"].as<String>();

      //Store the fifth artist name
      tracks.a5 = root["tracks"]["track"][4]["artist"]["name"].as<String>();
    }
  }
  //Print out an error if we fail to get a good HTTP response
  else {
    Serial.printf("Something went wrong with connecting to the endpoint in getTracks().");
  }
}

//Function to get tracks data from Last.FM
void getCommuteTime() {
  //Initialize and HTTP Client object
  HTTPClient theClient;

  //API CALL: http://wsdot.wa.gov/Traffic/api/TravelTimes/TravelTimesREST.svc/GetTravelTimeAsJson?AccessCode=20b50f16-95fc-4aa1-9ce5-ebeff14fe7dc&TravelTimeID=79
  //Initialize a string to build the API call to WSDOT
  String apiCall = " http://wsdot.wa.gov/Traffic/api/TravelTimes/TravelTimesREST.svc/GetTravelTimeAsJson?AccessCode=";
  //Store the Access Code key
  apiCall += wsdotAccessCode;
  //Get Redmond to Seattle route
  apiCall += "&TravelTimeID=79";
  
  //Make API call
  theClient.begin(apiCall);
  //Check HTTP response
  int httpCode = theClient.GET();
  //Check if somebody came back
  if (httpCode > 0) {
    //Check if header response is OK
    if (httpCode == HTTP_CODE_OK) {
      //Store the payload from open weather map
      String payload = theClient.getString();
      //Initialize a JSON buffer
      DynamicJsonBuffer jsonBuffer;
      //Parse payload and turn into a JSON object
      JsonObject& root = jsonBuffer.parseObject(payload);
      //Check if we failed to get a JSON object, and print a message
      if (!root.success()) {
        Serial.println("parseObject() failed in getTracks().");
        return;
      }

      //Store the commute start
      comm.starting = root["StartPoint"]["Description"];

      //Store the commute end
      comm.ending = root["EndPoint"]["Description"];
      
    }
  }
  //Print out an error if we fail to get a good HTTP response
  else {
    Serial.printf("Something went wrong with connecting to the endpoint in getTracks().");
  }
}


