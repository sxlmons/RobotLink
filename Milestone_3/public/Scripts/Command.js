
// creates the connect POST request URL
function connect() {
    var action = "/connect";
    var ip = document.getElementById("ip").value;
    var port = document.getElementById("port").value;

    action += "?ip=" + ip + "&port=" + port;

    document.getElementById("connect-form").action = action;
    document.getElementById("connect-form").submit();
}

// calls POST fetch and changes displays based on response
function sendCommand(command) {

    const form = document.getElementById("command-form");

    if (!form.checkValidity()) {
        form.reportValidity();
        return; 
    }

    var action = "/telecommand";
    var dir = document.getElementById("direction").value;
    var dur = document.getElementById("duration").value;
    var speed = document.getElementById("speed").value;

    action += "?direction=" + dir + "&duration=" + dur + 
                "&speed=" + speed + "&command=" + command;

    document.getElementById("displayBox").innerText = `Sending ${command} command...`;

    fetch(action, { method: "POST" })
        .then(response => {
            if (!response.ok) {
                throw new Error(`Server returned ${response.status}`);
            }
            return response.text();
        })
        .then(data => {
            document.getElementById("displayBox").innerText = data || "No response from server.";
        })
        .catch(error => {
            document.getElementById("displayBox").innerText = `Error: ${error.message}`;
        });
}

// calls GET fetch and changes displays based on response 
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