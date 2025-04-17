#define CROW_MAIN

#include <iostream>
#include <fstream>
#include <memory>

#include "crow_all.h"

#include "MySocket/MySocket.h"
#include "Packet/Packet.h"

using namespace std;

void getIndex(crow::response& res);
void getScript(crow::response &res, string scriptName);
void getCommandPage(crow::response &res);
void sendFile(crow::response &res, ifstream &in);

PktDef::CmdType getCommandType(string command);

int main() {
    
    crow::SimpleApp app;

	// shared state for socket connection and packet counter across routes
	// ensures safe access in all lambdas
	auto clientSocket = std::make_shared<std::unique_ptr<MySocket>>(nullptr);
    auto packetCounter = std::make_shared<int>(0);

	// index route called on start up 
	CROW_ROUTE(app, "/")
	([clientSocket, packetCounter](const crow::request &req, crow::response &res){ 
		// resets the packet counter and socket pointer 
		// meant for a reset on refresh to index 
		*clientSocket = nullptr;
		*packetCounter = 0;
		// index page shows connect to robot screen 
		getIndex(res);
	});

	// query string POST method route
	CROW_ROUTE(app, "/connect").methods(crow::HTTPMethod::POST)
	([clientSocket](const crow::request& req, crow::response& res) {
		// gets the ip and int port from the url
		string ip = req.url_params.get("ip");
		int port = std::stoi(std::string(req.url_params.get("port")));

		try {
			// sets the client pointer to a new instance of my socket
			// with the inputted user ip and port number 
			*clientSocket = std::make_unique<MySocket>(CLIENT, ip, port, UDP, 1024);
		} catch(const std::exception& e) {
			cout << string(e.what());
			// refresh index page if setting the pointer throws an exception 
			getIndex(res);
		}

		// if successful set of client socket info send them to command page 
		getCommandPage(res);
	});

	// another query string POST method route 
	CROW_ROUTE(app, "/telecommand").methods(crow::HTTPMethod::POST)
	([clientSocket, packetCounter](const crow::request& req, crow::response& res) {
		// gets the direction, duration, speed, and command form the url 
		int direction = std::stoi(std::string(req.url_params.get("direction")));
		int duration = std::stoi(std::string(req.url_params.get("duration")));
		int speed = std::stoi(std::string(req.url_params.get("speed")));
		string command = req.url_params.get("command");

		// makes new packet instance 
		PktDef packet;
		// increments and sets the packet counter 
		(*packetCounter)++;
		packet.SetPktCount(*packetCounter); 
		// sets the command based on the user input 
		packet.SetCmd(getCommandType(command));

		// sets all of the drive body info 
		packet.driverBody.Direction = direction;
		packet.driverBody.Duration = duration;
		packet.driverBody.Speed = speed;
		// set the body data 
		packet.SetBodyData(reinterpret_cast<char*>(&packet.driverBody), sizeof(packet.driverBody));

		// generate the packet and return the byte buffer for sending 
		char* packetBuffer = packet.GenPacket();

		// send the packet info to the length of the packet 
		(*clientSocket)->SendData(packetBuffer, packet.GetLength());

		// create a buffer for receiving and fill with getData function 
		char buff[1024];
		(*clientSocket)->GetData(buff);
		// create a packet with the response data 
		PktDef responsePacket(buff);

		// create and write a message to display based on the inputs if ACKed
		if (responsePacket.cmdPacket.header.Ack == 1) {
			res.write(responsePacket.CreateResponseMessage(direction, duration, speed, command));
			res.end();
		}

		// display bad packet if not 
		res.write("Bad Command Packet");
		res.end();
	});

	// GET method route for getting the robots telemetry data 
	CROW_ROUTE(app, "/telemetry_request").methods(crow::HTTPMethod::GET)
	([clientSocket, packetCounter](const crow::request& req, crow::response& res) {
		// make packet and increment counter 
		PktDef packet;
		(*packetCounter)++;
		packet.SetPktCount(*packetCounter); 
		// set the command flag to response 
		packet.SetCmd(PktDef::RESPONSE);

		// set data, generate packet, send packet to request telemetry
		packet.SetBodyData(reinterpret_cast<char*>(&packet.driverBody), sizeof(packet.driverBody));
		char* packetBuffer = packet.GenPacket();
		(*clientSocket)->SendData(packetBuffer, packet.GetLength());

		// make and fill buffer, then create response packet 
		char buff[1024];
		(*clientSocket)->GetData(buff);
		PktDef responsePacket(buff);

		// if packet is acknowledged get the second telemetry one 
		if (responsePacket.cmdPacket.header.Ack == 1) {
			// if it is ACKed 
			// repeat process for receiving packet 
			char buffTele[1024];
			(*clientSocket)->GetData(buffTele);
			PktDef telePacket(buffTele);
			// then write the data to the response and send 
			res.write(telePacket.GetTelemetryData());
			res.end();
		}

		// if the packet is not ACKed return back packet to UI
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

// return response based on input 
PktDef::CmdType getCommandType(string command) {
	if (command == "drive")
		return PktDef::DRIVE;
	else if (command == "sleep")
		return PktDef::SLEEP;
	else  
		return PktDef::RESPONSE;
	
	// returns response as default becuase it doesnt mess with anything 
}

// functions for getting files and sending them back to web browser 
void getIndex(crow::response &res) {
	ifstream in("../public/Pages/index.html", ifstream::in);
	sendFile(res, in);
}
void getScript(crow::response &res, string scriptName) {
	ifstream in("../public/Scripts/" + scriptName, ifstream::in);
	sendFile(res, in);
}
void getCommandPage(crow::response &res) {
	ifstream in("../public/Pages/Command.html", ifstream::in);
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