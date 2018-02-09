#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"

using namespace std;

webSocket server;
string buffer;

class PongGame {
public:
	int ballPosX, ballPosY, lvlWidth = 600, lvlHeight = 600, 
		paddleWidth = 100, paddleHeight = 30, 
		ballSpeed = 5, paddleSpeed = 10, 
		paddleOffsetLimit = 250, 
		p0x = 45, p0y = 300, spawnp0x = 45, spawnp0y = 300, p0score = 0;//p0x == paddle num 0 x position
	//void resetGame();
	//void readInput(string input);
	//void movePaddle(int player, int inputSelection);
	//void updateBallPositions();
	//string generateStateStr();
private:
	float ballVelX, ballVelY;

public :void resetGame() {
	// place ball at center of play space and initialize its velocity
		p0score = 0;
		ballPosX = 300;
		ballPosY = 300;
		double angleSelection = 3.14159266/2.0;//Random between [0 and 2 pi)
		ballVelX = ballSpeed * -sin(angleSelection);
		ballVelY = ballSpeed * cos(angleSelection);
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
			bool hitAPaddle = false;
			//first check ball change to see if it crossed a critical line. For one paddle that is only a change across 60 from above to below
			//cout << "		ballPosX = " << ballPosX << " ballVelX = " << ballVelX << '\n';
			if(ballPosX >= 60 && (ballPosX + ballVelX) <= 60){
				//cout << "\n???checking collisions with paddle 0???\n";
				//check collision with paddle 0
				bool slopeIsApproxInfinite = abs(ballVelY) < 0.005;
				//cout << ballVelX << '/' << ballVelY << '\n';
				float m = ballVelX / ballVelY;
				int estimatedPointOfCollision = !slopeIsApproxInfinite ? ballPosY + int((ballPosX - 60) * m) : ballPosY;
				//cout << "estimate point of collision at: " << estimatedPointOfCollision << '\n';
				if(abs(estimatedPointOfCollision - p0y) <= paddleWidth/2){
					//a collision happened!
					//cout << "---A collision with paddle 0 has occured!---\n";
					hitAPaddle = true;
					++p0score;
					//assigning a new velocity based on where on the paddle the collision occurred
					//cout << "p0y = " << p0y << " paddleWidth/2.0 = " << paddleWidth / 2.0 << '\n';
					double angleSelection = 210 + 120.0 / paddleWidth * (estimatedPointOfCollision - p0y + paddleWidth / 2.0);//angle between 210 and 330 depending on where hit paddle
					//cout << "angleSelection = " << angleSelection << '\n';
					ballVelX = ballSpeed * -sin(angleSelection * 3.14159266 / 180.0);
					ballVelY = ballSpeed * cos(angleSelection * 3.14159266 / 180.0);
					//cout << "ballVelX = " << ballVelX << " ballVelY = " << ballVelY << '\n';
					//assigning ball position to the point of collision
					ballPosX = 60;
					ballPosY = estimatedPointOfCollision;
					//but now we need to calculate next ball position
					//might as well let it be at the estimate point of collision instead of any fancier prediction

				}
			}
			
			if(!hitAPaddle){
				// temporary code to reflect ball back if it hits a wall
				//p1 facing wall
				if (ballPosY >= 600) {
					ballVelY *= -1;
				}
				//hitAWall = true;
				//p2 facing wall
				if (ballPosX >= 600) {
					ballVelX *= -1;
				}
				//p3 facing wall
				if (ballPosY <= 0) {
					ballVelY *= -1;
				}

				//reset game if ya done goofed
				if (ballPosX <= 0) {
					resetGame();
				}
			}

			if(!hitAPaddle){
				ballPosX += ballVelX;
				ballPosY += ballVelY;
			}
		}
		string generateStateStr() {
			ostringstream toReturn;
			toReturn << ballPosY << ',' << ballPosX << ';' << p0x << ',' << p0y << ',' << p0score;
			return toReturn.str();
		}

};

PongGame pong;
/* called when a client connects */
void openHandler(int clientID){
	buffer = "01";
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
	buffer = "05";
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
	buffer = message;
	/*
	ostringstream os;
	pong.readInput(message);
	pong.updateBallPositions();
    os << "Stranger " << clientID << " says: " << message;

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
		if (clientIDs[i] != clientID) {}
            //server.wsSend(clientIDs[i], os.str());
    }
	server.wsSend(clientID, pong.generateStateStr());
	*/
}

/* called once per select() loop */
void periodicHandler(){
    static time_t next = time(NULL) + .75;
	static unsigned long long int kappa = 0;
	//kappa++;
	//std::cout << kappa++ << std::endl;

    time_t current = time(NULL);
    if (current >= next && buffer!="05"){
        //ostringstream os;
		//Deprecated ctime API in Windows 10
		//char timecstring[26];
		//ctime_s(timecstring, sizeof(timecstring), &current);
		//string timestring(timecstring);
        //timestring = timestring.substr(0, timestring.size() - 1);
        //os << timestring;
		//ostringstream os;

		pong.readInput(buffer);
		pong.updateBallPositions();

        vector<int> clientIDs = server.getClientIDs();
		for (int i = 0; i < clientIDs.size(); i++) 
		{
			server.wsSend(clientIDs[i], pong.generateStateStr());
		}
            //server.wsSend(clientIDs[i], os.str());
		buffer = "01";
		next = time(NULL) + .75;
    }
}

int main(int argc, char *argv[]){
    int port = 8000;
	buffer = "05";

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
}
