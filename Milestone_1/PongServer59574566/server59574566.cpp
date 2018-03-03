#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include <map>
#include <chrono>
#include <queue>
using namespace std;	

webSocket server;
bool started = false;
__int64 latency = 0;

struct bufferMessage {
	string info;
	__int64 timestamp;
};

queue <bufferMessage> inbuffer;
queue <bufferMessage> outbuffer;

//structure to hold two time values and an estimated ping value
struct messageTimer {
	__int64 before, after, expected;
};

//hard coded container for the 4 players' times
messageTimer* playersPings = new messageTimer[4];

class PongGame {
public:
	int playerWhoLastHitBall = 4,
		ballPosX = 300, ballPosY = 300, lvlWidth = 600, lvlHeight = 600,
		paddleWidth = 100, paddleHeight = 30,
		ballSpeed = 5, paddleSpeed = 10,
		paddleOffsetLimit = 250,
		p0x = 45, p0y = 300, spawnp0x = 45, spawnp0y = 300, p0score = 0,//p0x == paddle num 0 x position
		p1x = 300, p1y = 555, spawnp1x = 300, spawnp1y = 555, p1score = 0,
		p2x = 555, p2y = 300, spawnp2x = 555, spawnp2y = 300, p2score = 0,
		p3x = 300, p3y = 45, spawnp3x = 300, spawnp3y = 45, p3score = 0;

	std::map<int, std::string> usernames;
	bool name_assigned = false;
	//void resetGame();
	//void readInput(string input);
	//void movePaddle(int player, int inputSelection);
	//void updateBallPositions();
	//string generateStateStr();
private:
	float ballVelX, ballVelY;

public :void resetGame() {
	// place ball at center of play space and initialize its velocity
		//p0score = 0;
	
		ballPosX = 300;
		ballPosY = 300;
		double angleSelection = 3.14159266/2.0;//Random between [0 and 2 pi) to do later I guess
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
			else if (input[0] == '1') {
				//move left input
				if (input[1] == '0') {
					movePaddle(1, 0);
				}
				//not moving
				else if (input[1] == '1') {
					movePaddle(1, 1);
				}
				//moving right
				else if (input[1] == '2') {
					movePaddle(1, 2);
				}
			}
			else if (input[0] == '2') {
				//move left input
				if (input[1] == '0') {
					movePaddle(2, 0);
				}
				//not moving
				else if (input[1] == '1') {
					movePaddle(2, 1);
				}
				//moving right
				else if (input[1] == '2') {
					movePaddle(2, 2);
				}
			}
			else if (input[0] == '3') {
				//move left input
				if (input[1] == '0') {
					movePaddle(3, 0);
				}
				//not moving
				else if (input[1] == '1') {
					movePaddle(3, 1);
				}
				//moving right
				else if (input[1] == '2') {
					movePaddle(3, 2);
				}
			}
		}
		void movePaddle(int player, int inputSelection) {
			if (player % 2 == 0) {
				if (player == 0) {
					//horizontal movement
					if (inputSelection == 0) {
						//move right
						p0y = (abs(p0y + paddleSpeed - spawnp0y) >= paddleOffsetLimit) ? spawnp0y + paddleOffsetLimit : p0y + paddleSpeed;
					}
					else if (inputSelection == 2) {
						//move left
						p0y = (abs(p0y - paddleSpeed - spawnp0y) >= paddleOffsetLimit) ? spawnp0y - paddleOffsetLimit : p0y - paddleSpeed;
					}
				}
				else {
					//player is 2
					//horizontal movement
					if (inputSelection == 0) {
						//move right
						p2y = (abs(p2y + paddleSpeed - spawnp2y) >= paddleOffsetLimit) ? spawnp2y + paddleOffsetLimit : p2y + paddleSpeed;
					}
					else if (inputSelection == 2) {
						//move left
						p2y = (abs(p2y - paddleSpeed - spawnp2y) >= paddleOffsetLimit) ? spawnp2y - paddleOffsetLimit : p2y - paddleSpeed;
					}
				}
			}
			else {
				//vertical movement
				if (player == 1) {
					if (inputSelection == 0) {
						//move left
						p1x = (abs(p1x - paddleSpeed - spawnp1x) >= paddleOffsetLimit) ? spawnp1x - paddleOffsetLimit : p1x - paddleSpeed;
					}
					else if (inputSelection == 2) {
						//move right
						p1x = (abs(p1x + paddleSpeed - spawnp1x) >= paddleOffsetLimit) ? spawnp1x + paddleOffsetLimit : p1x + paddleSpeed;
					}
				}
				else {
					//player 3
					if (inputSelection == 0) {
						//move left
						p3x = (abs(p3x - paddleSpeed - spawnp3x) >= paddleOffsetLimit) ? spawnp3x - paddleOffsetLimit : p3x - paddleSpeed;
					}
					else if (inputSelection == 2) {
						//move right
						p3x = (abs(p3x + paddleSpeed - spawnp3x) >= paddleOffsetLimit) ? spawnp3x + paddleOffsetLimit : p3x + paddleSpeed;
					}
				}
			}
		}
		void updateBallPositions() {
			bool hitAPaddle = false;
			if (ballPosX >= 60 && (ballPosX + ballVelX) <= 60) {
				bool slopeIsApproxInfinite = abs(ballVelY) < 0.005;
				float m = ballVelX / ballVelY;
				int estimatedPointOfCollision = !slopeIsApproxInfinite ? ballPosY + int((ballPosX - 60) * m) : ballPosY;
				if (abs(estimatedPointOfCollision - p0y) <= paddleWidth / 2) {
					hitAPaddle = true;
					playerWhoLastHitBall = 0;
					double angleSelection = 210 + 120.0 / paddleWidth * (estimatedPointOfCollision - p0y + paddleWidth / 2.0);
					ballVelX = ballSpeed * -sin(angleSelection * 3.14159266 / 180.0);
					ballVelY = ballSpeed * cos(angleSelection * 3.14159266 / 180.0);
					ballPosX = 60;
					ballPosY = estimatedPointOfCollision;
				}
			}else if (ballPosX <= 540 && (ballPosX + ballVelX) >= 540) {
				bool slopeIsApproxInfinite = abs(ballVelY) < 0.005;
				float m = ballVelX / ballVelY;
				int estimatedPointOfCollision = !slopeIsApproxInfinite ? ballPosY + int((ballPosX - 540) * m) : ballPosY;
				if (abs(estimatedPointOfCollision - p2y) <= paddleWidth / 2) {
					hitAPaddle = true;
					playerWhoLastHitBall = 2;
					double angleSelection = 150 - 120.0 / paddleWidth * (estimatedPointOfCollision - p2y + paddleWidth / 2.0);
					ballVelX = ballSpeed * -sin(angleSelection * 3.14159266 / 180.0);
					ballVelY = ballSpeed * cos(angleSelection * 3.14159266 / 180.0);
					ballPosX = 540;
					ballPosY = estimatedPointOfCollision;
				}
			}
			else if (ballPosY >= 60 && (ballPosY + ballVelY) <= 60) {
				bool slopeIsApproxZero = abs(ballVelX) < 0.005;
				float m = ballVelY / ballVelX;
				int estimatedPointOfCollision = !slopeIsApproxZero ? ballPosX + int((ballPosY - 60) * m) : ballPosX;
				if (abs(estimatedPointOfCollision - p3x) <= paddleWidth / 2) {
					hitAPaddle = true;
					playerWhoLastHitBall = 3;
					double angleSelection = 60 - 120.0 / paddleWidth * (estimatedPointOfCollision - p3x + paddleWidth / 2.0);
					ballVelX = ballSpeed * -sin(angleSelection * 3.14159266 / 180.0);
					ballVelY = ballSpeed * cos(angleSelection * 3.14159266 / 180.0);
					ballPosX = estimatedPointOfCollision;
					ballPosY = 60;
				}
			}
			else if (ballPosY <= 540 && (ballPosY + ballVelY) >= 540) {
				bool slopeIsApproxZero = abs(ballVelX) < 0.005;
				float m = ballVelY / ballVelX;
				int estimatedPointOfCollision = !slopeIsApproxZero ? ballPosX + int((ballPosY - 540) * m) : ballPosX;
				if (abs(estimatedPointOfCollision - p1x) <= paddleWidth / 2) {
					hitAPaddle = true;
					playerWhoLastHitBall = 1;
					double angleSelection = 120 + 120.0 / paddleWidth * (estimatedPointOfCollision - p1x + paddleWidth / 2.0);
					ballVelX = ballSpeed * -sin(angleSelection * 3.14159266 / 180.0);
					ballVelY = ballSpeed * cos(angleSelection * 3.14159266 / 180.0);
					ballPosX = estimatedPointOfCollision;
					ballPosY = 540;
				}
			}

			if(!hitAPaddle){
				ballPosX += ballVelX;
				ballPosY += ballVelY;
				//reset game if ya done goofed
				if (ballPosX <= 0 || ballPosX >= 600 || ballPosY <= 0 || ballPosY >= 600) {
					if (playerWhoLastHitBall != 4 && p0score < 10 && p1score < 10 && p2score < 10 && p3score < 10) {
						switch (playerWhoLastHitBall)
						{
						case 0:
							++p0score;
							break;
						case 1:
							++p1score;
							break;
						case 2:
							++p2score;
							break;
						case 3:
							++p3score;
							break;
						default:
							break;
						}
						playerWhoLastHitBall = 4;
					}
					resetGame();
				}
			}
		}
		void AssignName(int id, string name)
		{
			usernames.insert(std::make_pair(id, name));
		}
		string generateStateStr() {
			ostringstream toReturn;
			toReturn << ballPosY << ',' << ballPosX 
				<< ';' << p0x << ',' << p0y << ',' << p0score
				<< ';' << p1x << ',' << p1y << ',' << p1score
				<< ';' << p2x << ',' << p2y << ',' << p2score
				<< ';' << p3x << ',' << p3y << ',' << p3score;
			return toReturn.str();
		}

};

PongGame pong;
/* called when a client connects */
void openHandler(int clientID){
	
	vector<int> clientIDs = server.getClientIDs();
	cout << "new size of accepted client list: " << clientIDs.size() << "\n";
	if (clientIDs.size() == 4) {
		cout << "\nstarting game!\n";
		
		started = true;

		pong.resetGame();
		cout << "assigning player nums.\n";
		for (int i = 0; i < clientIDs.size(); i++) {  //also gives them all other names
			ostringstream temp;
			temp << i;
			cout << "comparing i: " << i << endl;;
			ostringstream list;
			list << "N;";
			for (int k = 0; k != pong.usernames.size(); k++)
			{
				if (k != i)
				{
					
					list << pong.usernames[k];
					list << ";";

				}
					
					
			}
			//server.wsSend(clientIDs[i], list.str());
			server.wsSend(clientIDs[i], temp.str());
			
		}
		cout << "sending stateStrings to all clients.\n\n";
		for (int i = 0; i < clientIDs.size(); i++) {
			server.wsSend(clientIDs[i], pong.generateStateStr());
		}
		
		
	}
	//server.wsSend(clientID, "Welcome!");
}

/* called when a client disconnects */
void closeHandler(int clientID){
	cout << "telling connection number " << clientID << " to scram.\n\n";
	

	started = false;
    /*ostringstream os;
    os << "Stranger " << clientID << " has left.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
	*/
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){


	//special message check: Name Message
	if (message[0] == 'N')
	{
		string name;
		name = message.substr(1);
		
		pong.AssignName(clientID, name);
		cout << "Player " << clientID << " is " << name;
	}
	//special message check: Return Message (for ping calculation)
	else if (message[0] == 'R') {

	}

	bufferMessage buffer;
	buffer.info = message;
	chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch());
	buffer.timestamp = ms.count();
	inbuffer.push(buffer);
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
    //static time_t next = time(NULL) + 1;
    //time_t current = time(NULL);

	while (outbuffer.size() > 0 && started)
	{
		chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch());

		if (ms.count() > outbuffer.front().timestamp + latency)
		{
			vector<int> clientIDs = server.getClientIDs();
			for (int i = 0; i < clientIDs.size(); i++)
			{
				//send the state message to client in stack, append the total calculation time to the end of the passed message
				server.wsSend(clientIDs[i], outbuffer.front().info + ";" + std::to_string(chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch()).count() - outbuffer.front().timestamp));
				
				//next begin serverside ping calculation
				//first log the current time and the client ID, before sending the ping request
				//playersPings[i]. = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch()).count();
				//send a ping request to client as well
				server.wsSend(clientIDs[i], "R");
			}

			outbuffer.pop();
		}
		else
			break;
	}


    if (started && pong.usernames.size() == 4){

		if (!pong.name_assigned && pong.usernames.size() == 4)
		{
			cout << "\n\n\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << pong.usernames.size() << "\n\n\n";
			vector<int> clientIDs = server.getClientIDs();
			for (int i = 0; i < clientIDs.size(); i++)
			{
				ostringstream list;
				list << "N;";

				cout << "Client ID: " << i << "name: " << pong.usernames[i] << endl;
				for (int k = 0; k != pong.usernames.size(); k++)
				{

						list << pong.usernames[k];
						list << ";";

					


				}
				server.wsSend(clientIDs[i], list.str());

			}
			pong.name_assigned = true;
		}
        //ostringstream os;
		//Deprecated ctime API in Windows 10
		//char timecstring[26];
		//ctime_s(timecstring, sizeof(timecstring), &current);
		//string timestring(timecstring);
        //timestring = timestring.substr(0, timestring.size() - 1);
        //os << timestring;
		//ostringstream os;
		chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds> (chrono::system_clock::now().time_since_epoch());
		while (inbuffer.size() >= 1)
		{
			if (ms.count() <= inbuffer.front().timestamp + latency)
				break;
			string bufferinfo = inbuffer.front().info;
			inbuffer.pop();
			pong.readInput(bufferinfo);
		}
		
		pong.updateBallPositions();	
		bufferMessage buffer;
		buffer.info = pong.generateStateStr();
		buffer.timestamp = ms.count();
		outbuffer.push(buffer);
    }
}

int main(int argc, char *argv[]){
    int port = 8000;
	
	started = false;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
}
