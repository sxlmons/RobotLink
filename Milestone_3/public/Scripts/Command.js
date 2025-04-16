function connect() {
    var action = "/connect";
    var ip = document.getElementById("ip").value;
    var port = document.getElementById("port").value;

    action += "?ip=" + ip + "&port=" + port;

    document.getElementById("connect-form").action = action;
    document.getElementById("connect-form").submit();
}

function sendCommand(command) {

    const form = document.getElementById("command-form");

    // Check HTML5 validity
    if (!form.checkValidity()) {
        form.reportValidity(); // Show validation messages
        return; // Don't submit if invalid
    }

    var action = "/telecommand";
    var dir = document.getElementById("direction").value;
    var dur = document.getElementById("duration").value;
    var speed = document.getElementById("speed").value;

    action += "?direction=" + dir + "&duration=" + dur + 
                "&speed=" + speed + "&command=" + command;

    //document.getElementById("command-form").action = action;
    //document.getElementById("command-form").submit();

    document.getElementById("displayBox").innerText = `Sending ${command} command...`;

    fetch(action, { method: "POST" })
        .then(response => {
            if (!response.ok) {
                throw new Error(`Server returned ${response.status}`);
            }
            return response.text(); // assuming your server returns a string
        })
        .then(data => {
            document.getElementById("displayBox").innerText = data || "No response from server.";
        })
        .catch(error => {
            document.getElementById("displayBox").innerText = `Error: ${error.message}`;
        });
}

function getTelemetry() {
    document.getElementById("displayBox").innerText = "Requesting telemetry data...";
  
    fetch("/telemetry_request")
      .then(response => response.text())
      .then(data => {
        document.getElementById("telemetryBox").innerText = data;
        document.getElementById("displayBox").innerText = "Telemetry data updated.";
      })
      .catch(err => {
        document.getElementById("displayBox").innerText = `Telemetry Error: ${err.message}`;
      });
  }