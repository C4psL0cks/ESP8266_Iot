/* [NEED] --- Include something need, example WiFi library, Arduino --- */

#include <SocketIOClient.h>

/* [NEED] --- Declare something need, example WiFi library --- */

SocketIOClient client;

StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();

extern String RID;
extern String Rname;
extern String Rcontent;

char hostip[] = "YOUR_HOST_IP";
int port = YOUR_PORT_NUMBER;

void setup() {

  /* [NEED] --- Initial anything that you want --- */

  /* [NEED] --- Try to connect to WiFi --- */

  if (!client.connect(hostip, port)) {
    Serial.println("connection failed");
  }
  if (client.connected()) {
    client.send("connection", "message", );
  }
}

void loop() {
  if (client.monitor()) {

    // Ex. Sending side(package from server)
    //     socket.emit("EVENT_NAME", { "ATTRIBUTE_OF_OBJ" : SOMETHING });

    if (RID == "EVENT_NAME") {
      if (Rname == "ATTRIBUTE_OF_OBJ") {
        Serial.print("Got " + Rcontent);
        // Output : Got SOMETHING
      }
    }
  }
