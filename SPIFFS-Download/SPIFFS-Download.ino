#include <WiFi.h>
#include<HTTPClient.h>
#include "FS.h"
#include "SPIFFS.h"
#include <esp_task_wdt.h>


//WiFi credentials
char const* ssid = "MANTRI 1st FLOOR";
char const* password = "#MANTRI+";

char const* url = "https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/wdts.html#_CPPv419esp_task_wdt_delete12TaskHandle_t";
// char const* url = "https://raw.githubusercontent.com/rashida048/Datasets/refs/heads/master/mushrooms.csv";
// char const* url = "https://raw.githubusercontent.com/datasciencedojo/datasets/refs/heads/master/titanic.csv";

char const* filePath = "/test.csv";
char const* msg = "Hello!";



//Declaring custom functions
void writeFile( const char *path);
void readFile(const char *path);


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(500);

  }

  Serial.println("Conneted");

  
  //SPIFFS init
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  Serial.println("SPIFFS Mount successful/nFormatting SPIFFS");


  //disable WDT during formatting SPIFFS

    esp_task_wdt_config_t config = {
      .timeout_ms = 10000,  // Set the timeout to 10 seconds
      .trigger_panic = NULL,    // Specify the task function
  };

  esp_task_wdt_init(&config);
  esp_task_wdt_deinit();

  //formatting SPIFFS
  if(SPIFFS.format())
  {
    Serial.println("Formatting Successful.");
  }
  else
  {
    Serial.println("Formatting Error.");
    
  }
  //enable WDT  
  esp_task_wdt_init(&config);

  Serial.println("SPIFFS Info:");
  Serial.printf("Total Bytes: %u\n", SPIFFS.totalBytes());
  Serial.printf("Used Bytes: %u\n", SPIFFS.usedBytes());
  writeFile( filePath);

  //Displaying contents of test.txt
  readFile(filePath);
}

void loop() {
  // put your main code here, to run repeatedly:

}


/*--------------------------Custom functions--------------------------*/

void writeFile( const char *path) 
{
  uint8_t buff[128] = {0};

  //http client connection
  HTTPClient client;
  client.setTimeout(3000);
  
  for(int retry = 0   ;retry < 5;retry++)
  {

    client.begin(url);
    int httpCode = client.GET();

    //checking http response
    Serial.println("\n\n");
    if(httpCode>=400)
    {
      Serial.print("Error!");
      Serial.println(httpCode);
      return;
    }
    else
    {
      Serial.print("Connection established.");
      Serial.print(" HTTP Code: ");
      Serial.println(httpCode);
    }

    // get tcp stream
    WiFiClient *stream = client.getStreamPtr();

    //Open test.txt file
    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file) {
      Serial.println("- failed to open file for writing");
      return;
    }

    long fileSize = client.getSize();
    Serial.print("File Size: ");
    Serial.print(fileSize/1024);
    Serial.println("KB");

    // read all data from server
    long startTime = millis();    //Stat time
    if(fileSize>1550000)
    {
      Serial.println("Unable to download because file size exceeds 1.5MB.");
      return;
    }

    while (client.connected() && stream->available()) 
    {
      int c = stream->readBytes(buff, sizeof(buff));
      file.write(buff, sizeof(buff));
      delay(1);

      if(stream->available() == 0)
      {
        Serial.println("Stream.available() = 0");
      }   

      if(!client.connected())
      {
        Serial.println("Client disconnected");
      }

    }

    long endTime = millis();    //end time 
    

    long size = file.size();
    file.close();
    if(size<fileSize)
    {
      Serial.println("Download incomplete. Retrying");
      delay(1000);
    }
    else
    {
      Serial.println("Download Successfull.");

      float time = (endTime - startTime);
      float speed = size/time; 
      Serial.print("Size: ");
      Serial.print(size/1024);
      Serial.println(" KB");
      
      Serial.print("Speed: ");
      Serial.print(speed);
      Serial.println("KBps");
      client.end();
      break;
    }
  }
}


void readFile(const char *path) 
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = SPIFFS.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}