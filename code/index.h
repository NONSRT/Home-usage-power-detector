#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* webpage = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
    <title>ESP32 Power Detector</title>
    <style>
    html, body {
        height: 100%;
    }

    html {
        display: table;
        margin: auto;
    }

    body {
        display: table-cell;
        vertical-align: center;
    }

    h1 {text-align: center;}

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
    <h1>Home power usage monitor</h1>
    <i class="fa fa-lightbulb-o" style="font-size:48px; color:#ffca04;"></i>
    <h2>
        Your room is now <span style="color: red;"><span id="light">Loading...</span></span>
    </h2>
    <h2>
        Light switch is turn <span style="color: red;"><span id="LightSwitchStatus">Loading...</span></span>
        <label class="switch">
            <input type="checkbox" id="LightSwitchCheck" onclick="LightSwitch()">
            <span class="slider round"></span>
        </label>
    </h2>
    <h2>
        Current Power Flee: <span style="color: red;"><span id="lightFlee">Loading...</span></span>THB
    </h2>
    <br>
    <i class="fa fa-thermometer-half" style="font-size:48px; color:#059e8a;"></i>
    <h2>
        Temperature: <span style="color: red;"><span id="temperature">Loading...</span>&deg</span> 
        Heat Index: <span style="color: red;"><span id="heatIndex">Loading...</span>&deg</span>
    </h2>
    <h2>Your room is <span style="color: red;"><span id="heatStatus">Loading...</span></span>
    <h2> 
        Air conditioner is <span style="color: red;"><span id="airStatus">Loading...</span></span>
        <label class="switch">
            <input type="checkbox" id="AirSwitchCheck" onclick="AirconSwitch()">
            <span class="slider round"></span>
        </label>
    </h2>
    <h2>
        Air Flee: <span style="color: red;"><span id="airFlee">Loading...</span></span>THB
    </h2>
    <br>
    <i class="fa fa-flash" style="font-size:48px;color:#00F0FF;"></i>
    <h2>Power usage Flee: <span style="color: red;"><span id="CTPowerFlee">Loading...</span></span>THB</h2>

    <h2>Power Flee/Hour: <span style="color: red;"><span id="PowerFleePerHr">Loading...</span></span>THB</h2>
    <h2>Power Flee/Day: <span style="color: red;"><span id="PowerFleePerDay">Loading...</span></span>THB</h2>

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

            fetch("/LightSwitchStatus")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("LightSwitchStatus").textContent = data;
                });
        }

        lightMonitor();
        setInterval(lightMonitor, 22600); // Update temperature every 22.6 seconds

        function LightSwitch() {
            var LightCheckBox = document.getElementById("LightSwitchCheck");
            var Lightxhr = new XMLHttpRequest();
            if (LightCheckBox.checked == true){
                Lightxhr.open("GET", "/lightSlider?value=ON", true);
            } else {
                Lightxhr.open("GET", "/lightSlider?value=OFF", true);
            }
            Lightxhr.send();

            fetch("/LightSwitchStatus")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("LightSwitchStatus").textContent = data;
                });
        }

        function temperature() {
            fetch("/temperature")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("temperature").textContent = data;
                });
            
            fetch("/heatIndex")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("heatIndex").textContent = data;
                });

            fetch("/heatStatus")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("heatStatus").textContent = data;
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
        setInterval(temperature, 22600); // Update temperature every 22.6 seconds

        function AirconSwitch() {
            var AirCheckBox = document.getElementById("AirSwitchCheck");
            var Airxhr = new XMLHttpRequest();
            if (AirCheckBox.checked == true){
                Airxhr.open("GET", "/airSlider?value=ON", true);
            } else {
                Airxhr.open("GET", "/airSlider?value=OFF", true);
            }
            Airxhr.send();

            fetch("/airStatus")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("airStatus").textContent = data;
                });
        }

        function CTPower() {
            fetch("/CTPowerFlee")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("CTPowerFlee").textContent = data;
                });
        }

        CTPower();
        setInterval(CTPower, 22600); // Update temperature every 22.6 seconds

        function PowFleePer() {
            fetch("/PowerFleePerHr")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("PowerFleePerHr").textContent = data;
                });

            fetch("/PowerFleePerDay")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("PowerFleePerDay").textContent = data;
                });
        }

        PowFleePer();
        setInterval(PowFleePer, 22600); // Update temperature every 22.6 seconds
    </script>
</body>
</html>
)=====";

#endif