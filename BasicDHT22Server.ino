/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
*/

//Libraries
#include <DHT.h>
#include <Vector.h>



//Constants
#define PARSEP ","
#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define DHTPIN0 32    // pin of DHT22 number 0
#define DHTPIN1 34    // pin of DHT22 number 1
#define DHTPIN2 36    // pin of DHT22 number 2
#define DHTPIN3 38    // pin of DHT22 number 3

DHT dht0(DHTPIN0, DHTTYPE); // Initialize DHT sensor number 0 for normal 16mhz Arduino
DHT dht1(DHTPIN1, DHTTYPE); // 1
DHT dht2(DHTPIN2, DHTTYPE); // 2
DHT dht3(DHTPIN3, DHTTYPE); // 3

DHT dhts[4] = {dht0,dht1,dht2,dht3};



//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value



//Commands
const int nOperations = 2;
String operations[nOperations] = {"NOP","DHT"};
int indexOfOperation(String op)
{
  for (int i=0; i<nOperations; i++)
  {
    if (operations[i] == op)
    {
      return i;
    }
  }
  return 0;
}

void partition(String str, String sep, String & bef, String & aft)
{
  //char pieces[2];
  int index = -1;
  int l_str = str.length();
  int l_sep = sep.length();
  String sub = "";
  while (not (sub == sep))
  {
    index = index + 1;
    if (index+l_sep > l_str){
      bef=str;
      aft="";
      return;// pieces;
    }
    sub = str.substring(index,index+l_sep);
  }
  bef=str.substring(0,index);
  aft=str.substring(index+l_sep);
  return;// pieces;
}

void flash(int port, int ms)
{
  digitalWrite(port,HIGH);
  delay(ms);
  digitalWrite(port,LOW);
}

void setup()
{
  Serial.begin(9600);
  dhts[0].begin();
  dhts[1].begin();
  //dhts[2].begin();
  //dhts[3].begin();
}

void cmd_dht(String parameters)
{
  float temp,hum;
  String id, channel;
  int channel_type;
  partition(parameters,PARSEP,id,channel);
  
  if (channel=="T")
  {
    channel_type=0;
  }
  else if (channel == "H")
  {
    channel_type=1;
  }
  else if (channel == "TH" || channel == "HT")
  {
    channel_type=2;
  }
  else
  {
    Serial.println("Invalid channel. Must be 'T','H','TH', or 'HT'");
    return;
  }

  if (id=="0"||id=="1"||id=="2"||id=="3")
  {
    switch(channel_type)
    {
      case 0:
      temp = dhts[id.toInt()].readTemperature();
      if (id == "0")
      {
        flash(33,10000);
      }
      Serial.println(temp);
      break;

      case 1:
      hum = dhts[id.toInt()].readHumidity();
      if (id == "0")
      {
        flash(35,100);
      }
      Serial.println(hum);
      break;

      case 2:
      temp = dhts[id.toInt()].readTemperature();
      hum = dhts[id.toInt()].readHumidity();
      if (id == "0")
      {
        flash(33,100);
        flash(35,100);
      }
      Serial.print(temp);
      Serial.print(",");
      Serial.println(hum);
      break;

      default:
      break; 
    }
  }
  else
  {
    Serial.println("Invalid ID. Must be integer in [0,3]");
    return;
  }
}

void router(String comm)
{
  String bef,aft;
  String sep = ":";
  partition(comm,sep,bef,aft);

  int op = indexOfOperation(bef);
  switch(op)
  {
    case 0: // NOP or unrecognized command
    Serial.println("NOP");
    break;

    case 1: // DHT
    cmd_dht(aft);
    break;

    default:
    break;
  }
}

void loop() 
{
  Serial.flush();
  String comm = "";
  if(Serial.available())
  {
    char received;
    while (received != '\r')
    {
      if(Serial.available())
      {
        received = Serial.read();
        if (received == '\n' || received == ' ' || received == '\t') // ignore whitespace
        {}
        else if (received == '\r')
        {}
        else
        {
          comm += received;
        }        
      }
    }
  router(comm);
  }
}

   
