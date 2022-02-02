#include <Arduino.h>
#include "GPS_Air530Z.h"
#define BUFFER_SIZE 500
char txpacket[BUFFER_SIZE];
int counter = 0;
int lineCounter = 0;

void setup()
{
  pinMode(GPIO14, OUTPUT);
  digitalWrite(GPIO14, LOW);

  Serial.begin(115200);
  Serial1.begin(9600);
}

uint8_t getChar(char c)
{
  if (c <= '9')
    return c - '0';
  if (c <= 'F')
    return c - 'A' + 10;
  return c - 'a' + 10; // <= f
}

uint8_t getCheckSum(char *packet, size_t size)
{
  uint8_t res = 0;
  for (size_t i = 0; i < size; i++)
  {
    if (packet[i] == '*' && i + 2 < size)
    {
      res = (getChar(packet[i + 1]) << 4) | getChar(packet[i + 2]);
      break;
    }
  }
  return res;
}

uint8_t calcCheckSum(char *packet, size_t size)
{
  uint8_t res = 0;
  for (size_t i = 1; i < size; i++)
  {
    res ^= packet[i];
  }
  return res;
}

void t2()
{

  while (Serial1.available() > 0)
  {
    char c = Serial1.read();
    if (c == '$')
      counter = 0;
    if (c == '\n')
      return;
    if (c == '\r')
    {
      uint8_t s = calcCheckSum(txpacket, counter - 3);
      uint8_t ss = getCheckSum(txpacket, counter);
      if (s == ss)
      {
        Serial.print(".");
        lineCounter++;
      }
      else
      {
        Serial.println();
        Serial.print("ERROR ");
        Serial.print(s, HEX);
        Serial.print("<>");
        Serial.print(ss, HEX);
        Serial.print(" ");
        Serial.write((uint8_t *)txpacket, counter);
        Serial.println();
        counter = 0;
      }
    }
    txpacket[counter++] = c;
  }
}

void lc()
{
  if (lineCounter > 120)
  {
    lineCounter = 0;
    Serial.println();
  }
}

void t3()
{
  while (Serial.availableForWrite() == 0) {
      delay(16);
  }
  Serial.write('+');
  lineCounter++;
}

void loop()
{
  t2();
  t3();
  lc();
}
