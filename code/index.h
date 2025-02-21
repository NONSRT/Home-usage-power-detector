#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* webpage = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Power Detector</title>
    <style>
    .switch {
        position: relative;
        display: inline-block;
        width: 60px;
        height: 34px;
    }

    .switch input { 
        opacity: 0;
        width: 0;
        height: 0;
    }

    .slider {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: black;
        -webkit-transition: .4s;
        transition: .4s;
    }

    .slider:before {
        position: absolute;
        content: "";
        height: 26px;
        width: 26px;
        left: 4px;
        bottom: 4px;
        background-color: white;
        -webkit-transition: .4s;
        transition: .4s;
    }

    input:checked + .slider {
        background-color: #ffca04;
    }

    input:focus + .slider {
        box-shadow: 0 0 1px #ffca04;
    }

    input:checked + .slider:before {
        -webkit-transform: translateX(26px);
        -ms-transform: translateX(26px);
        transform: translateX(26px);
    }

    /* Rounded sliders */
    .slider.round {
        border-radius: 34px;
    }

    .slider.round:before {
        border-radius: 50%;
    }
    </style>
</head>
<body>
    <h1>ESP32</h1>
    <h2>
        Light Status: <span style="color: red;"><span id="light">Loading...</span></span> 
        <label class="switch">
            <input type="checkbox">
            <span class="slider round"></span>
        </label>
    </h2>
    <h2>Current Power Flee: <span style="color: red;"><span id="lightFlee">Loading...</span></span>THB</h2>
    <script>
        function lightMonitor() {
            fetch("/light")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("light").textContent = data;
                });
            
            fetch("/lightFlee")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("lightFlee").textContent = data;
                });
        }

        lightMonitor();
        setInterval(lightMonitor, 22600); // Update temperature every 4 seconds
    </script>

    <h2>
        Temperature: <span style="color: red;"><span id="temperature">Loading...</span></span>
    </h2>
    <h2> 
        Air conditioner is <span style="color: red;"><span id="airStatus">Loading...</span></span>
        <label class="switch">
            <input type="checkbox" id="myCheck" onclick="AirconSwitch()">
            <span class="slider round"></span>
        </label>
    </h2>
    <h2>
        Air Flee: <span style="color: red;"><span id="airFlee">Loading...</span></span>
    </h2>
    <script>
        function temperature() {
            fetch("/temperature")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("temperature").textContent = data;
                });

            fetch("/airFlee")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("airFlee").textContent = data;
                });
                
            fetch("/airStatus")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("airStatus").textContent = data;
                });
        }

        temperature();
        setInterval(temperature, 22600); // Update temperature every 4 seconds

        function AirconSwitch() {
            var AirCheckBox = document.getElementById("myCheck");
            var xhr = new XMLHttpRequest();
            if (AirCheckBox.checked == true){
                xhr.open("GET", "/slider?value=ON", true);
            } else {
                xhr.open("GET", "/slider?value=OFF", true);
            }
            xhr.send();

            fetch("/airStatus")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("airStatus").textContent = data;
                });
        }
    </script>
</body>
</html>
)=====";

#endif