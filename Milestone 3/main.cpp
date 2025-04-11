#define CROW_MAIN

#include <iostream>
#include <fstream>

#include "crow_all.h"

using namespace std;

void getIndex(crow::response& res);
void getImage(crow::response &res, string fileName);
void getScript(crow::response &res, string scriptName);
void sendFile(crow::response &res, ifstream &in);

int main() {
    
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")
	([](const crow::request &req, crow::response &res){ 
		getIndex(res);
	});

    // /connect?ip=192.X.X.10&port=123
    CROW_ROUTE(app, "/connect").methods(crow::HTTPMethod::POST)
    ([](const crow::request &req, crow::response &res) {
        string ip = req.url_params.get("ip");
		string port = req.url_params.get("port");
        cout << "IP: " << ip << ", Port:" << port << endl;
        res.code = 410;
        res.end();
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