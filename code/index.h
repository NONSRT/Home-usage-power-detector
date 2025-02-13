#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* webpage = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Power Detector</title>
</head>
<body>
    <h1>ESP32</h1>
    <p>Light Status: <span style="color: red;"><span id="light"><span id="powerFlee">Loading...</span></span></span></p>
    <script>
        function lightMonitor() {
            fetch("/light")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("light").textContent = data;
                });
        }

        lightMonitor();
        setInterval(lightMonitor, 45000); // Update temperature every 4 seconds
    </script>

    <p>Temperature: <span style="color: red;"><span id="temperature">Loading...</span></span></p>
    <script>
        function temperature() {
            fetch("/temperature")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("temperature").textContent = data;
                });
        }

        temperature();
        setInterval(temperature, 60000); // Update temperature every 4 seconds
    </script>
</body>
</html>
)=====";

#endif