function connect() {
    var action = "/connect";
    var ip = document.getElementById("ip").value;
    var port = document.getElementById("port").value;

    action += "?ip=" + ip + "&port=" + port;

    document.getElementById("connect-form").action = action;
    document.getElementById("connect-form").submit();
}

function clickTest() {
    document.getElementById("click-test").action = "/testSend";
    document.getElementById("click-test").submit();
}