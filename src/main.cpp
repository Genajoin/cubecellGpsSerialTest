#include <Arduino.h>
#include "GPS_Air530Z.h"

#define BUFFER_SIZE 500

char txpacket[BUFFER_SIZE];
int counter = 0;
int lineCounter = 0;
bool isWrite = false;
uint32_t timeCounter = 0;

/// wait to empty write Serial0 buffer
/// problematic function
void writeWait()
{
  while (Serial.availableForWrite() < 8)
  {
    // This delay is a rx buffer problem. If decrease to 5 ms - no problem.
    delay(6);
  }
}

void setup()
{
  pinMode(GPIO14, OUTPUT);
  digitalWrite(GPIO14, LOW);
  Serial.begin(115200);
  Serial1.begin(9600);
}

uint8_t getHexChar(char c)
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
      res = (getHexChar(packet[i + 1]) << 4) | getHexChar(packet[i + 2]);
      break;
    }
  }
  return res;
}

uint8_t calcCheckSum(char *packet, size_t size)
{
  uint8_t res = 0;
  for (size_t i = 0; i < size; i++)
  {
    if (packet[i] == '*')
      break;
    if (packet[i] != '$')
      res ^= packet[i];
  }
  return res;
}

/// read and check GPS data
void read()
{
  if (counter == BUFFER_SIZE)
    counter = 0;

  while (Serial1.available() > 0)
  {
    char c = Serial1.read();
    if (c == '$')
      counter = 0;
    if (c == '\n')
      return;
    if (c == '\r')
    {
      uint8_t s = calcCheckSum(txpacket, counter);
      uint8_t ss = getCheckSum(txpacket, counter);
      if (s == ss)
      {
        writeWait();
        Serial.print("1");
        lineCounter++;
      }
      else
      {
        writeWait();
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

/// Print something to Serial0
void write()
{
  writeWait();
  Serial.write('0');
  lineCounter++;
}

// switch mode every 3 second
bool writeMode()
{
  if (millis() - timeCounter > 3000)
  {
    timeCounter = millis();
    isWrite = !isWrite;
    writeWait();
    if (isWrite)
    {
      Serial.println("\r\nWrite something to Serial0.");
    }
    else
    {
      Serial.println("\r\nSilent mode to Serial0.");
    }
  }
  return isWrite;
}

// Line formating
void lineFormat()
{
  if (lineCounter > 100)
  {
    lineCounter = 0;
    Serial.println();
  }
}

/// LOOP
void loop()
{
  read();
  if (writeMode())
    write();
  lineFormat();
}
