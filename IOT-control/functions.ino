void printWifiStatus() {
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void postData(String VARID, float data) {
  uint8_t content_len[6] = {0};
  String TxData;
  char charDATA[20] = "";
  floatToString(charDATA, data, 2, 7 );

  String dataString = "{\"value\": ";
  dataString += charDATA;
  dataString += '}';

  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("send POST");

    // send the HTTP PUT request
    client.print("POST /api/v1.6/variables/");
    client.print(VARID);
    client.println("/values HTTP/1.1");
    client.println("Host: things.ubidots.com");
    client.print("X-Auth-Token: ");
    client.println(TOKEN);
    client.print("Content-Length:");
    client.println(itoa(dataString.length(), (char*)content_len, 10));
    client.println("Content-Type: application/json");
    client.println("Connection: close\r\n");
    client.print(dataString);
    client.println("\r\n");

    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}
void httpRequest(String VARID) {
  Serial.println();
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");

    // send the HTTP PUT request
    client.print("GET /api/v1.6/variables/");
    client.print(VARID);
    client.println(" HTTP/1.1");
    client.println("Host: things.ubidots.com");
    client.print("X-Auth-Token: ");
    client.println(TOKEN);
    client.println("Content-Type: application/json");
    client.println("Connection: close\r\n");

    // note the time that the connection was made
    lastConnectionTime = millis();
    getIsConnected = true;
  }
  else {
    Serial.println("Connection failed");
    getIsConnected = false;
  }
}

int getInt(String input) {
  int temp;
  char carray[20];
  //Serial.println(input);
  input.toCharArray(carray, sizeof(carray));
  //Serial.println(carray);
  temp = atoi(carray);
  return temp;
}


char * floatToString(char * outstr, double val, byte precision, byte widthp)
{
  char temp[16];
  byte i;

  // compute the rounding factor and fractional multiplier
  double roundingFactor = 0.5;
  unsigned long mult = 1;
  for (i = 0; i < precision; i++)
  {
    roundingFactor /= 10.0;
    mult *= 10;
  }
  temp[0] = '\0';
  outstr[0] = '\0';

  if (val < 0.0)
  {
    strcpy(outstr, "-\0");
    val = -val;
  }

  val += roundingFactor;

  strcat(outstr, itoa(int(val), temp, 10)); //prints the int part
  if ( precision > 0)
  {
    strcat(outstr, ".\0"); // print the decimal point
    unsigned long frac;
    unsigned long mult = 1;
    byte padding = precision - 1;

    while (precision--)
      mult *= 10;

    if (val >= 0)
      frac = (val - int(val)) * mult;
    else
      frac = (int(val) - val ) * mult;

    unsigned long frac1 = frac;

    while (frac1 /= 10)
      padding--;

    while (padding--)
      strcat(outstr, "0\0");

    strcat(outstr, itoa(frac, temp, 10));
  }

  // generate space padding
  if ((widthp != 0) && (widthp >= strlen(outstr)))
  {
    byte J = 0;
    J = widthp - strlen(outstr);

    for (i = 0; i < J; i++)
    {
      temp[i] = ' ';
    }

    temp[i++] = '\0';
    strcat(temp, outstr);
    strcpy(outstr, temp);
  }
  return outstr;
}


int parser_value(String buf, String Param)
{
int firstSign;
int secondSign;
int endSign;
String tmpVal;
int value;

firstSign = buf.indexOf(Param);
secondSign = buf.indexOf(firstSign,':');
endSign = buf.indexOf(secondSign,ENDPARSE);

tmpVal = buf.substring(secondSign + 1 , endSign);
tmpVal.trim();
value = tmpVal.toInt();

return value;
}
