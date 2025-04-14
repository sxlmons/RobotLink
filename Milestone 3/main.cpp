#define CROW_MAIN

#include <iostream>
#include <fstream>

#include "crow_all.h"

#include "MySocket/MySocket.h"
#include "Packet/Packet.h"

using namespace std;

void getIndex(crow::response& res);
void getImage(crow::response &res, string fileName);
void getScript(crow::response &res, string scriptName);
void getPage(crow::response &res, string page);
void sendFile(crow::response &res, ifstream &in);

int main() {
    
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
	([](const crow::request &req, crow::response &res){ 
		getIndex(res);
	});

    // /connect?ip=192.X.X.10&port=123
    CROW_ROUTE(app, "/connect").methods(crow::HTTPMethod::POST)
	([](const crow::request& req, crow::response& res) {
		string ip = req.url_params.get("ip");
		string port = req.url_params.get("port");

		MySocket client(CLIENT, ip, stoi(port), UDP, 1024);

		getPage(res, "Command");
	});

	CROW_ROUTE(app, "/testSend")
	([](const crow::request& req, crow::response& res) {
	
		MySocket client(CLIENT, "127.0.0.1", 23500, UDP, 1024);
	
		PktDef packet;
		packet.SetPktCount(1);
		packet.SetCmd(PktDef::DRIVE);

		packet.driverBody.Direction = 1;
		packet.driverBody.Duration = 10;
		packet.driverBody.Speed = 80;
		packet.SetBodyData(reinterpret_cast<char*>(&packet.driverBody), sizeof(packet.driverBody));

		char* packetBuffer = packet.GenPacket();

		std::cout << "Packet length: " << packet.GetLength() << std::endl;
		std::cout << "Sending UDP packet..." << std::endl;
		client.SendData(packetBuffer, packet.GetLength());

		cout << "FINISHED HERE" << endl;
		getPage(res, "Command");
	});

/*
    CROW_ROUTE(app, "/telecommand/").methods(crow::HTTPMethod::POST)
    ([]() {


    });

    CROW_ROUTE(app, "/telemetry_request/")
    ([]() {
        

    });
*/
	CROW_ROUTE(app, "/Scripts/<string>")
	([](const crow::request &req, crow::response &res, string scriptName) {
		getScript(res, scriptName);
	});

    app.port(23500).multithreaded().run();

    return 1;
}

void getIndex(crow::response &res) {
	ifstream in("../public/Pages/index.html", ifstream::in);
	sendFile(res, in);
}
void getImage(crow::response &res, string fileName) {
	ifstream in("../public/Images/" + fileName, ifstream::in);
	sendFile(res, in);
}
void getScript(crow::response &res, string scriptName) {
	ifstream in("../public/Scripts/" + scriptName, ifstream::in);
	sendFile(res, in);
}
void getPage(crow::response &res, string page) {
	ifstream in("../public/Pages/" + page + ".html", ifstream::in);
	sendFile(res, in);
}
void sendFile(crow::response &res, ifstream &in) {
	if (in) {
		ostringstream contents;
		contents << in.rdbuf();
		in.close();
		res.write(contents.str());
	} else {
		res.code = 404;
		res.write("Not Found");
	}
	res.end();
}