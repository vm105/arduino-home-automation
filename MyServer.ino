/*
  MyServer

  A home automation server

  Created On July 28 2016
  By Varun Malhotra

*/

#include <SPI.h>
#include <Ethernet.h>
#define PASSWORD "varun"
#define LAMP1 8

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

bool accessDenied = true;

//IPAddress ip(128, 61, 99, 208);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {

  // reniew lease

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }




  // start the Ethernet connection and the server:
  Ethernet.begin(mac);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  digitalWrite(LAMP1, LOW); // set to low at beginning
}


void loop() {

  int status = Ethernet.maintain(); // maintains DHCP lease and checks status

  // if lease reniewal fails
  if (status== 1 || status == 3) {

    Ethernet.begin(mac);
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());    
  }
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    char line[100];
    int index = 0;

    while (client.connected()) {
      accessDenied = true;
      if (client.available()) {
        char c = client.read();
        if(index < 100) {

          line[index++] = c;

        }
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header

          //bool menu = strstr(line, "my_action") != 0;
          bool authorized = strstr(line, PASSWORD) || strstr(line, "my_action") != 0;
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");

          client.println();
          client.print("<meta name=viewport content=width=device-width, initial-scale=1.0>");
          client.println("<!DOCTYPE HTML>");

          client.println("<html>");
          client.println("<h1> <center> My Room! </h1>");

          client.print("<body bgcolor=#ff3333>");
          client.println("<br>");

          if ((!authorized)) {

            client.print("<form method=get action=/?>");
            client.print("<center>Password: <br> <center>");
            client.print("<input type=password name=password>");
            client.print("<br> <input type=submit value=Go!>");
            client.print("</form>");

          } else {



            //client.println("<h1><center>LED Control</h1></center><br><center><form method=get action=/?><input type=radio name=L1 value=1>On<br><input type=radio name=L1 value=0>Off<br><input type=submit value=submit></form></center>");
            //break;
            client.print("<form method=get action=/my_action?>");
            client.print("<center><button name=LED type=submit value=blink>blink!</button><br>");
            client.print("<center><button name=LED type=submit value=on>ON</button><br>");
            client.print("<center><button name=LED type=submit value=off>OFF</button>");
            client.print("<center></form>");

            client.print("</body>");
            client.println("</html>");
          }


          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }


        if(strstr(line, "LED=blink")!=0) {

        digitalWrite(2, HIGH);
        delay(1);
        digitalWrite(2, LOW);


        } else if (strstr(line, "LED=on") != 0) {

           digitalWrite(LAMP1, HIGH);

        } else if (strstr(line, "LED=off")!=0) {

          digitalWrite(LAMP1, LOW);

        } else if (strstr(line, PASSWORD)) {
          accessDenied = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    //close the connection:
    //Serial.println(line);



    client.stop();
    Serial.println("client disconnected");
  }
}
