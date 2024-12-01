
#include <Wire.h>
#include <DHT.h>
#include <DHT_U.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <esp_now.h>
#include <WiFi.h>

// Rover 1 MAC: cc:db:a7:3e:bf:28
// Rover 2 MAC: Pending...
// Receiver MAC: 08:a6:f7:b1:0d:c4

#define led_pin 2

uint8_t receiverAddress[] = {0x08, 0xA6, 0xF7, 0xB1, 0x0D, 0xC4};

// Motor control pins
const int motor1Pin1 = 13;
const int motor1Pin2 = 25;
const int motor2Pin1 = 26;
const int motor2Pin2 = 27;

// GPS
HardwareSerial gpsSerial(2);
TinyGPSPlus gps;
float lat, lon;

// DHT11
#define DHTPIN 21
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int hum;
double temp;

int carMove = 0;

// Communication
//  Variable to store if sending data was successful
String success;

typedef struct struct_message
{
  int move;
  double temp;
  int hum;
  float lat;
  float lon;

} struct_message;

struct_message incomingReadings;

struct_message myData;

esp_now_peer_info_t peerInfo;

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

// Callback when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Move: ");
  Serial.println(incomingReadings.move);
  carMove = incomingReadings.move;
}

void forward()
{
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void backward()
{
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void right()
{
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}
void left()
{
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void stop()
{
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}

void getReadings()
{
  while (gpsSerial.available() > 0)
  {
    gps.encode(gpsSerial.read());
  }

  // Print GPS data
  if (gps.location.isUpdated())
  {
    lat = gps.location.lat();
    lon = gps.location.lng();
    // Serial.print("Latitude: ");
    // Serial.println(gps.location.lat(), 6);
    // Serial.print("Longitude: ");
    // Serial.println(gps.location.lng(), 6);
  }

  // Read DHT11 data
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(temp))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX2 = GPIO16, TX2 = GPIO17

  // Initialize motor control pins
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(led_pin, OUTPUT);

  // Initialize DHT11
  dht.begin();

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, receiverAddress, 6);

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received

  digitalWrite(led_pin, HIGH);
  delay(10000);
  digitalWrite(led_pin, LOW);
  delay(500);
  digitalWrite(led_pin, HIGH);
  delay(100);
  digitalWrite(led_pin, LOW);
}

void loop()
{

  getReadings();

  myData.temp = temp;
  myData.hum = hum;
  myData.lat = lat;
  myData.lon = lon;

  switch (carMove)
  {
  case 1:
    forward();
    break;
  case 2:
    backward();
    break;
  case 3:
    right();
    break;
  case 4:
    left();
    break;
  case 0:
    stop();
    break;
  default:
    stop();
    break;
  }

  // Send message via ESP-NOW

  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&myData, sizeof(myData));

  // Print DHT11 data
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" *C");
  Serial.print("Latitude: ");
  Serial.println(lat, 6);
  Serial.print("Longitude: ");
  Serial.println(lon, 6);
  delay(2000);
}