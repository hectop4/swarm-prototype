#include <esp_now.h>
#include <WiFi.h>

// Receiver MAC: 08:a6:f7:b1:0d:c4
// Rover 1 MAC: cc:db:a7:3e:bf:28

uint8_t Rover1Address[] = {0xCC, 0xDB, 0xA7, 0x3E, 0xBF, 0x28};

double tempIn;
int humIn;
float latIn;
float lonIn;

esp_now_peer_info_t peerInfo; 

typedef struct struct_message
{
  double temp;
  int hum;
  float lat;
  float lon;

} struct_message;

struct_message myData;

String success;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  tempIn = myData.temp;
  humIn = myData.hum;
  latIn = myData.lat;
  lonIn = myData.lon;
}
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0)
  {
    success = "Delivery Success :)";
  }
  else
  {
    success = "Delivery Fail :(";
  }
}


void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

    // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, Rover1Address, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop()
{

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(Rover1Address, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  Serial.print("Temperature: ");
  Serial.println(tempIn);
  Serial.print("Humidity: ");
  Serial.println(humIn);
  Serial.print("Latitude: ");
  Serial.println(latIn);
  Serial.print("Longitude: ");
  Serial.println(lonIn);
  delay(1000);
}