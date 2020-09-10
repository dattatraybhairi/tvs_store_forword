/**
   This code listens Sensortag data for 15 seconds and transmit data to thingsboard.

   Hardware SPI NODEMCU:
   MISO -> GPIO12.
   MOSI -> GPIO13.
   SCLK/SCK -> GPIO14.
   CSN/SS - > GPIO15. 

*/

#include "cc2500_REG.h"
#include "cc2500_VAL.h"
#define GPO0 5
#include <SPI.h>

#define PACKET_LENGTH 64
#define T2G 200     //change channel here
#define presence_channel 100
#define ACK_channel 150

#define No_of_tags 100
#define data_per_tag 6

#define sw_a 0
#define sw_b 2
//#define sw_c A5

#define LED_2 10
#define LED_3 16

#define reception_time 5000

bool received = false;
uint8_t RSSI_offset;

#define tx true
#define rx false

unsigned long currentMillis1 = 0, currentMillis3 = 0, currentMillis4 = 0;
//long sendInterval = 100; // in milliseconds
byte val = 0;
unsigned long currentMillis;

int16_t rssi_val;
int recv;

#define radioPktBuffer_len 70
uint8_t radioPktBuffer[radioPktBuffer_len];
uint8_t tempPktBuffer[radioPktBuffer_len];
bool mode;
boolean to_be_send = false;
uint8_t seq_no;
uint8_t unique_count = 0;
uint8_t health_count = 0;
uint8_t unique_pair_data[No_of_tags * data_per_tag];
uint8_t event_log[No_of_tags * 12];
uint8_t event_count = 0;
uint8_t health_data[No_of_tags * data_per_tag];
uint8_t loop_count = 0;
uint8_t test_counter = 0;
//=========================================================================
uint8_t sbuf[5] = {0, 0, 0, 0, 0};
unsigned long CLK_count = 0;
uint8_t T3_B, T2_B, T1_B;

void setup() {

  pinMode(sw_a, OUTPUT);
  pinMode(sw_b, OUTPUT);
  //pinMode(sw_c,OUTPUT);

  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);


  Serial.begin(115200);
  pinMode(GPO0, INPUT);
  // Setup
  pinMode(SS, OUTPUT);
  SPI.begin();
  digitalWrite(SS, HIGH);
  //  Serial.print("\n Starting Code");
  init_CC2500();
  //Read_Config_Regs();
  //  Serial.println("read end\t");
  received = false;
  switch_antenna(1);
  init_channel(T2G);
  send_rx_strobe();
  memset(tempPktBuffer, 0, sizeof(tempPktBuffer));
  digitalWrite(LED_3, HIGH);
}

void loop() {
  CLK_count++;
  loop_count++;
  test_counter++;
  clearData();
  //Serial.print(CLK_count); Serial.println("  ");
  T1_B = CLK_count % 256;
  //Serial.print(T1_B); Serial.println("  ");
  T2_B = (CLK_count - T1_B) / 256;
  //Serial.print(T2_B); Serial.print("  ");
  T2_B = T2_B % 256;
  //Serial.println(T2_B);
  T3_B = (CLK_count - T2_B * 256 - T1_B) / 65536;
  //Serial.print(T3_B); Serial.print("  ");
  T3_B = T3_B % 65536;
  //Serial.println(T3_B);
  //delay(5);
  //Serial.println("= >");
  // Reception Time = 30 secs
  switch_antenna(1);
  init_channel(T2G);
  send_rx_strobe();
  currentMillis4 = millis();
  //Serial.println("= > In Reception: ");
  while (millis() - currentMillis4 < reception_time)
  {
    CC2500_listenForPacket();
    if (received)
    {
      received = false;
      //find_uinque_pairs();
      find_uinque_pairs_II();
      send_rx_strobe();
    }
    yield();
  }

  digitalWrite(LED_2, HIGH); delay(10); digitalWrite(LED_2, LOW);


  //Serial.println("= > Out of Loop: ");
  /*
    if (test_counter == 5)
    {
    event_log[0] = 0; event_log[1] = 4;
    event_log[2] = 0; event_log[3] = 10;
    event_log[4] = 0; event_log[5] = 214; event_log[6] = 163;
    event_log[7] = 5; event_log[8] = 1;
    }

    if (test_counter == 10)
    {
    event_log[0] = 0; event_log[1] = 142;
    event_log[2] = 0; event_log[3] = 104;
    event_log[4] = 0; event_log[5] = 216; event_log[6] = 180;
    event_log[7] = 10; event_log[8] = 1;
    }

    if (test_counter == 20)
    {
    event_log[0] = 0; event_log[1] = 4;
    event_log[2] = 0; event_log[3] = 10;
    event_log[4] = 0; event_log[5] = 217; event_log[6] = 4;
    event_log[7] = 20; event_log[8] = 1;
    }

    if (test_counter > 20)
    {
    test_counter = 21;
    }
  */
  //  dummy_data();

  // Patch
  for (uint8_t i = 0; i < event_count; i++)
  {
    if (event_log[i*12+4] != 0)
    {
      event_log[i*12+4] = event_log[i*12+4] - 1;
    }
  }
  //
  //Battery Voltage Patch
  for (uint8_t i = 0; i < event_count; i++)
  {
    if (event_log[i*12+10] == 0)
    {
      event_log[i*12+10] = 75;
    }
    if (event_log[i*12 + 10] > 90)
    {
      event_log[i*12+10] = 83;
    }
    event_log[i*12+10] = event_log[i*12+10] + 10;
  }
  //
  for (uint8_t i = 0; i < 100; i++)
  {
    //    for (uint8_t j = 0; j < 6; j++)
    //    {
    //      Serial.write(unique_pair_data[i * 6 + j]);
    //      //Serial.print(unique_pair_data[i * 6 + j]);Serial.print("  ");
    //    }
    for (uint8_t j = 0; j < 12; j++)
    {
      Serial.write(event_log[i * 12 + j]); //Serial.print(" ");
    }
  }
  //Serial.println();
  Serial.write(127);
  //digitalWrite(LED_3, LOW);

  //Serial.println("********** Send Packet *************");


  read_configuration();

  // Transmit Configuration/presence information once every 30 sec
  init_channel(presence_channel);
  send_rx_strobe();

  if (loop_count == 2)
  {
    loop_count = 0;
    //Serial.println("= Transmitting: ");
    for (int n = 0; n < 2500; n++)
    {
      CC2500_sendPacket(5);
      delay(1);
    }
  }
  //  // Send data to Raspberry Pi
  //  Serial.println();
  //  Serial.print(" => CLk: ");Serial.print(T3_B);Serial.print("  ");Serial.print(T2_B);Serial.print("  ");Serial.println(T1_B);
//      Serial.println("================ Event Data ================");
//      for (uint8_t i = 0; i < event_count; i++)
//      {
//        for (uint8_t j = 0; j < 12; j++)
//        {
//          Serial.print(event_log[i * 12 + j]); Serial.print(" ");
//        }
//        Serial.println();
//      }
//      Serial.println();
  //  Serial.println("================ Health Data ================");
  //  for (uint8_t i = 0; i < health_count; i++)
  //  {
  //    for (uint8_t j = 0; j < 6; j++)
  //    {
  //      Serial.print(health_data[i*6+j]);Serial.print(" ");
  //    }
  //    Serial.println();
  //  }
  send_rx_strobe();
  yield();
}
