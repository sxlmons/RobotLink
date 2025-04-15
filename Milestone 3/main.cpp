#define CROW_MAIN

#include <iostream>
#include <fstream>
#include <memory>

#include "crow_all.h"

#include "MySocket/MySocket.h"
#include "Packet/Packet.h"

using namespace std;

void getIndex(crow::response& res);
void getImage(crow::response &res, string fileName);
void getScript(crow::response &res, string scriptName);
void getPage(crow::response &res, string page);
void sendFile(crow::response &res, ifstream &in);

PktDef::CmdType getCommandType(string command);

std::unique_ptr<MySocket> clientSocket;
int packetCounter;

int main() {
    
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
	([](const crow::request &req, crow::response &res){ 
		clientSocket = nullptr;
		packetCounter = 0;
		getIndex(res);
	});

    // /connect?ip=192.X.X.10&port=123
    CROW_ROUTE(app, "/connect").methods(crow::HTTPMethod::POST)
	([](const crow::request& req, crow::response& res) {
		string ip = req.url_params.get("ip");
		int port = std::stoi(std::string(req.url_params.get("port")));

		cout << "IP: " << ip << " Port: " << port << endl;

		try {
			clientSocket = std::make_unique<MySocket>(CLIENT, ip, port, UDP, 1024);
		} catch(const std::exception& e) {
			cout << string(e.what());
			res.code = 501;
			getIndex(res);
		}

		getPage(res, "Command");
	});

    CROW_ROUTE(app, "/telecommand").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req, crow::response& res) {

		int direction = std::stoi(std::string(req.url_params.get("direction")));
		int duration = std::stoi(std::string(req.url_params.get("duration")));
		int speed = std::stoi(std::string(req.url_params.get("speed")));
		string command = req.url_params.get("command");
		
		PktDef packet;
		packet.SetPktCount(1); // FIX this 
		
		packet.SetCmd(getCommandType(command));

		packet.driverBody.Direction = direction;
		packet.driverBody.Duration = duration;
		packet.driverBody.Speed = speed;

		packet.SetBodyData(reinterpret_cast<char*>(&packet.driverBody), sizeof(packet.driverBody));

		char* packetBuffer = packet.GenPacket();

		clientSocket->SendData(packetBuffer, packet.GetLength());

		char buff[1024];
		clientSocket->GetData(buff);
		PktDef responsePacket(buff);
		//direction, duration, speed, command
		res.write(responsePacket.CreateResponseMessage(direction, duration, speed, command));
		res.end();
    });

    CROW_ROUTE(app, "/telemetry_request").methods(crow::HTTPMethod::GET)
    ([](const crow::request& req, crow::response& res) {
        PktDef packet;
		packet.SetPktCount(1);
		// figure out packet counter 
		packet.SetCmd(PktDef::RESPONSE);

		packet.SetBodyData(reinterpret_cast<char*>(&packet.driverBody), sizeof(packet.driverBody));

		char* packetBuffer = packet.GenPacket();

		clientSocket->SendData(packetBuffer, packet.GetLength());

		char buff[1024];
		clientSocket->GetData(buff);
		PktDef responsePacket(buff);

		if (responsePacket.cmdPacket.header.Ack == 1) {
			char buffTele[1024];
			clientSocket->GetData(buffTele);
			PktDef telePacket(buffTele);
			res.write(telePacket.GetTelemetryData());
			res.end();
		}

		res.write("Bad Telemetry Packet");
		res.end();
    });

	CROW_ROUTE(app, "/Scripts/<string>")
	([](const crow::request &req, crow::response &res, string scriptName) {
		getScript(res, scriptName);
	});

    app.port(23500).multithreaded().run();

    return 1;
}

PktDef::CmdType getCommandType(string command) {

	if (command == "drive")
		return PktDef::DRIVE;
	else if (command == "sleep")
		return PktDef::SLEEP;
	else  
		return PktDef::RESPONSE;
	
	// returns response as default becuase it doesnt mess with anything 
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