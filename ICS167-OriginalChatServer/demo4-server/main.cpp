#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"

using namespace std;

webSocket server;

class PongGame {
public:
	int ballPosX, ballPosY, lvlWidth = 600, lvlHeight = 600, paddleWidth = 100, paddleHeight = 30, ballSpeed = 10, paddleSpeed = 10, paddleOffsetLimit = 250, p0x = 45, p0y = 300, spawnp0x = 45, spawnp0y = 300;//p0x == paddle num 0 x position
	//void resetGame();
	//void readInput(string input);
	//void movePaddle(int player, int inputSelection);
	//void updateBallPositions();
	//string generateStateStr();
private:
	float ballVelX, ballVelY;

public :void resetGame() {
	// place ball at center of play space and initialize its velocity
		ballPosX = 300;
		ballPosY = 300;
		double angleSelection = 3.14159266/2.0;//Random between [0 and 2 pi)
		ballVelX = ballSpeed * cos(angleSelection);
		ballVelY = ballSpeed * sin(angleSelection);
	}
		void readInput(string input) {
			//Player 0 info
			if (input[0] == '0') {
				//move left input
				if (input[1] == '0') {
					movePaddle(0, 0);
				}
				//not moving
				else if (input[1] == '1') {
					movePaddle(0, 1);
				}
				//moving right
				else if (input[1] == '2') {
					movePaddle(0, 2);
				}
				else {
					//??????????
				}
			}
		}
		void movePaddle(int player, int inputSelection) {
			if (player % 2 == 0) {
				//horizontal movement
				if (player < 2) {
					//movement flipped
					if (inputSelection == 0) {
						//move right
						p0y = (abs(p0y + paddleSpeed - spawnp0y) >= paddleOffsetLimit) ? spawnp0y + paddleOffsetLimit : p0y + paddleSpeed;
						//cout << "moving left.";
					}
					else if (inputSelection == 2) {
						//move left
						p0y = (abs(p0y - paddleSpeed - spawnp0y) >= paddleOffsetLimit) ? spawnp0y - paddleOffsetLimit : p0y - paddleSpeed;
						//cout << "moving right.";
					}
				}
				else {
					//movement normal
					if (inputSelection == 0) {
						//move left
					}
					else if (inputSelection == 2) {
						//move right
					}
				}
			}
			else {
				//TODO: this /shrug
				//vertical movement
				if (player < 2) {
					//movement flipped
				}
				else {

				}
			}
		}
		void updateBallPositions() {
			//nah, one step at a time
		}
		string generateStateStr() {
			ostringstream toReturn;
			toReturn << ballPosX << ',' << ballPosY << ';' << p0x << ',' << p0y;
			return toReturn.str();
		}

};

PongGame pong;
/* called when a client connects */
void openHandler(int clientID){
    ostringstream os;
	//os << "Stranger " << clientID << " has joined.";
	os << clientID;

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        server.wsSend(clientIDs[i], os.str());
    }
	pong.resetGame();
	server.wsSend(clientID, pong.generateStateStr());
	//server.wsSend(clientID, "Welcome!");
}

/* called when a client disconnects */
void closeHandler(int clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has left.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){
    ostringstream os;
	pong.readInput(message);
    os << "Stranger " << clientID << " says: " << message;

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
		if (clientIDs[i] != clientID) {}
            //server.wsSend(clientIDs[i], os.str());
    }
	server.wsSend(clientID, pong.generateStateStr());
}

/* called once per select() loop */
void periodicHandler(){
    static time_t next = time(NULL) + 10;
    time_t current = time(NULL);
    if (current >= next){
        ostringstream os;
		//Deprecated ctime API in Windows 10
		char timecstring[26];
		ctime_s(timecstring, sizeof(timecstring), &current);
		string timestring(timecstring);
        timestring = timestring.substr(0, timestring.size() - 1);
        os << timestring;

        vector<int> clientIDs = server.getClientIDs();
        for (int i = 0; i < clientIDs.size(); i++)
            server.wsSend(clientIDs[i], os.str());

        next = time(NULL) + 10;
    }
}

int main(int argc, char *argv[]){
    int port;

    cout << "Please set server port: ";
    cin >> port;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    //server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
}
