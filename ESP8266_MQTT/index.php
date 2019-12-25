<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Document</title>

    <!-- Bootstrap core CSS -->

    <script src="https://cdnjs.cloudflare.com/ajax/libs/paho-mqtt/1.0.2/mqttws31.min.js" type="text/javascript">
    </script>
    <script type="text/javascript">
    // Create a client instance
    client = new Paho.MQTT.Client("soldier.cloudmqtt.com", 37149, "myweb");
    //Example client = new Paho.MQTT.Client("m11.cloudmqtt.com", 32903, "web_" + parseInt(Math.random() * 100, 10));

    // set callback handlers
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;
    var options = {
        useSSL: true,
        userName: "yxdhwarq",
        password: "HONEfPeA4KUM",
        onSuccess: onConnect,
        onFailure: doFail
    }

    // connect the client
    client.connect(options);

    // called when the client connects
    function onConnect() {
        // Once a connection has been made, make a subscription and send a message.
        console.log("onConnect");
        client.subscribe("/server");
        message = new Paho.MQTT.Message("off");
        message.destinationName = "/server";
        client.send(message);
        alert("Connect OK");
    }

    function doFail(e) {
        console.log(e);
        alert("Error");
    }

    // called when the client loses its connection
    function onConnectionLost(responseObject) {
        if (responseObject.errorCode !== 0) {
            console.log("onConnectionLost:" + responseObject.errorMessage);
        }
    }

    // called when a message arrives
    function onMessageArrived(message) {
        console.log("onMessageArrived:" + message.payloadString);
        var x = message.payloadString;
        if (x == "OK") {
            alert("Message:" + x);
        }
    }

    function led_on() {
        send("on");
    }

    function led_off() {
        send("off");
    }

    function send(msg) {
        message = new Paho.MQTT.Message(msg);
        message.destinationName = "/server";
        client.send(message);
    }
    </script>

</head>

<body>
    <button class="btn btn-search" onclick='led_on()'>ON</button>
    <button class="btn btn-search" onclick='led_on()'>OFF</button>
</body>

</html>