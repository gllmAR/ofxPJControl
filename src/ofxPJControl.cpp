/*
 *  pjControl.h
 *
 *  Created by Noah Shibley on 8/9/10.
 *  Updated by Martial GALLORINI on 19/03/2015
 *
	Video projector control class.
 *
 *
 */

#include "ofxPJControl.h"
#include "ofMain.h"

ofxPJControl::ofxPJControl() {
	connected = false;
	projStatus = false;
}

ofxPJControl::~ofxPJControl() {

}

bool ofxPJControl::getProjectorPowerStatus() {
    // idea to get projector status
    if(commMode == NEC_MODE) {
        cout<<"not implemented yet"<<endl;
    }
    else if (commMode == PJLINK_MODE) {
        cout<<"not implemented yet"<<endl;
    //   pjLink_get_power_status();
    }
    else if (commMode == CHRISTIE_MODE) {
        cout<<"not implemented yet"<<endl;
    }
    else if (commMode == SANYO_MODE) {
        cout<<"not implemented yet"<<endl;
    }
    else if (commMode == PJDESIGN_MODE) {
        cout<<"not implemented yet"<<endl;
    }
    
	return projStatus; //ideally returns if projector is turned on or not, but not foolproof yet
}

bool ofxPJControl::getIsTCPClientConnected(){
    return pjClient.isConnected();
}

void ofxPJControl::setProjectorType(int protocol) { //NEC_MODE or PJLINK_MODE
		commMode = protocol;
    
    string projType = "";
    
    if(protocol ==0){
        projType = "PJLINK";
    }else if(protocol==1){
        projType=="NEC";
    }else if(protocol==2){
        projType=="CHRISTIE";
    }else if(protocol==3){
        projType=="SANYO";
    }else if(protocol==4){
        projType=="ProjectionDesign";
    }
    
    ofLogNotice()<<"-----Projector Type set to: " + projType <<endl;
}

void ofxPJControl::setup(string IP_add, int port, int protocol, string password) {
	setProjectorIP(IP_add);
	setProjectorType(protocol);
	setProjectorPort(port);
    setProjectorPassword(password);
    projStatus = false;
}

void ofxPJControl::setProjectorIP(string IP_add) {
	IPAddress = IP_add;
}

void ofxPJControl::setProjectorPort(int port) {
	pjPort = port;
}

void ofxPJControl::setProjectorPassword(string passwd) {
	password = passwd;
}


void ofxPJControl::On() {
	if(commMode == NEC_MODE) {
		nec_On();
	}
	else if (commMode == PJLINK_MODE) {
		pjLink_On();
	}
    else if (commMode == CHRISTIE_MODE) {
		christie_On();
	}
    else if (commMode == SANYO_MODE) {
		sanyo_On();
	}
    else if (commMode == PJDESIGN_MODE) {
        pjDesign_On();
    }
}

void ofxPJControl::Off(){
	if(commMode == NEC_MODE) {
		nec_Off();
	}
	else if (commMode == PJLINK_MODE) {
		pjLink_Off();
	}
	else if (commMode == CHRISTIE_MODE) {
		christie_Off();
	}
    else if (commMode == SANYO_MODE) {
		sanyo_Off();
	}
    else if (commMode == PJDESIGN_MODE) {
        pjDesign_Off();
    }
}


void ofxPJControl::mute_on() {
    if(commMode == NEC_MODE) {
        cout<<"not implemented yet"<<endl;
        //nec_mute_on();
    }
    else if (commMode == PJLINK_MODE) {
        pjLink_mute_on();
    }
    else if (commMode == CHRISTIE_MODE) {
        cout<<"not implemented yet"<<endl;
        //christie_mute_on();
    }
    else if (commMode == SANYO_MODE) {
        cout<<"not implemented yet"<<endl;
        //sanyo_mute_on();
    }
    else if (commMode == PJDESIGN_MODE) {
        cout<<"not implemented yet"<<endl;
        //pjDesign_mute_on();
    }
}

void ofxPJControl::mute_off(){
    if(commMode == NEC_MODE) {
        cout<<"not implemented yet"<<endl;
        //nec_mute_off();
    }
    else if (commMode == PJLINK_MODE) {
        pjLink_mute_off();
    }
    else if (commMode == CHRISTIE_MODE) {
        cout<<"not implemented yet"<<endl;
        //christie_mute_off();
    }
    else if (commMode == SANYO_MODE) {
        cout<<"not implemented yet"<<endl;
        //sanyo_mute_off();
    }
    else if (commMode == PJDESIGN_MODE) {
        cout<<"not implemented yet"<<endl;
        //pjDesign_mute_off();
    }
}


bool ofxPJControl::pingIPAddress(string _IP){
    string pingStr = (string)"ping -c 1 -t 1 " + IPAddress ;
    
    bool pingSuccess = false;
    int flag = system( pingStr.c_str());
    //cout<< "______________PING FLAG: " << flag <<endl;
    
    if(flag == 0){
        cout<<"Successfully Pinged: " + IPAddress <<endl;
        pingSuccess = true;
    }else{
        cout<<ofGetTimestampString() + " Unable to Ping:  "<<IPAddress<<endl;
        pingSuccess = false;
    }
    return pingSuccess;
}

string ofxPJControl::attemptConnection(){
    string msgRx="";
    
    if(!pjClient.isConnected()) {
        
        
        if(pingIPAddress(IPAddress)){
            //only try to establish tcp connection if ping is successful - otherwise, skip it because threading will lock up the app while waiting on a connection
            connected = pjClient.setup(IPAddress, pjPort,true);
        }else{
            cout<<"Ping Unsuccessful - TCP Connection not attempted"<<endl;
        }
        
        if(connected){
            ofLogNotice() << "Connection Established: " << IPAddress << ":" << pjPort << endl;
            string response = "";
            while (msgRx.length() < 8) {
                msgRx = pjClient.receiveRaw();
            }
            ofLogNotice() << "Received Response: " << msgRx << endl;
        } else {
            ofLogError() << "-------Failed to connect."<<endl;
            //we should try to reconnect in this case...or send a message of current status
        }
    }
    
    return msgRx;
}

void ofxPJControl::closeConnection(){
    ofLogNotice("closeConnection")<<"____Closing TCP Connection____"<<endl;
    pjClient.close();
    connected = false;
    
}



void ofxPJControl::sendPJLinkCommand(string command, bool sendOn) {
    string msgRx="";

    msgRx = attemptConnection();
    
    if(connected){
        string authToken = "";

        //eg. PJLINK 1 604cc14d
        if(msgRx[7] == '1') {
            ofLogNotice() << "with authentication" << endl;
            MD5Engine md5;
            md5.reset();
            string hash = msgRx.substr(9,8);
            ofLogNotice() << hash << endl;
            md5.update(hash + password);
            authToken = DigestEngine::digestToHex(md5.digest());
        }
		ofLogNotice() << "Attempting to send command with auth: " << authToken+command << endl;
		pjClient.sendRaw(authToken+command);
		msgRx = "";
		while (msgRx.length() < 8) {
			msgRx = pjClient.receiveRaw();
		}
        //G debug
        cout<<"MESSAGE RX : " <<msgRx<<endl;
        
        ofLogNotice() << "Received response: " << msgRx << endl;
        //Really we should check if we get a valid response here...
        if(sendOn){
            projStatus = true;
        }else{
            projStatus = false;
        }
        closeConnection();
		//connected = false;
    } else {
        ofLogError()<< "Wasn't connected - closing connection. Still not connected."<<endl;
        closeConnection();

    }
}

void ofxPJControl::sendCommand(string command,bool sendOn){
//        if(!pjClient.isConnected()) {
//			pjClient.setVerbose(true);
//			ofLogNotice() << "Connecting to : " << IPAddress << ":" << pjPort << endl;
//			connected = pjClient.setup(IPAddress, pjPort, true);
//			ofLogNotice() << "Connection state : " << connected;
//		}
    
    attemptConnection();
    
    if(connected){
        ofLogNotice() << "Attempting to Send Command : " << command << endl;
        pjClient.sendRaw(command);
        ofLogNotice() << "Response length (Bytes) : " << pjClient.getNumReceivedBytes() << endl;
        msgRx = "";
        msgRx = pjClient.receiveRaw();
        ofLogNotice() << "Received response : " << msgRx << endl;
        //Really we should check if we get a valid response here...
        if(sendOn){
            projStatus = true;
        }else{
            projStatus = false;
        }
        closeConnection();
    }
}



void ofxPJControl::pjLink_On() {
	string command = "%1POWR 1\r";
    sendPJLinkCommand(command, true);
    ofLogNotice()<<"PJ LINK ON SENT"<<endl;

}

void ofxPJControl::pjLink_Off() {
	string command = "%1POWR 0\r";
	sendPJLinkCommand(command, false);
    ofLogNotice()<<"PJ LINK OFF SENT"<<endl;
}

void ofxPJControl::pjLink_mute_on() {
    string command = "%1AVMT 11\r";
    sendPJLinkCommand(command, true);
    ofLogNotice()<<"PJ LINK MUTE ON SENT"<<endl;
    
}


void ofxPJControl::pjLink_mute_off() {
    string command = "%1AVMT 10\r";
    sendPJLinkCommand(command, false);
    ofLogNotice()<<"PJ LINK MUTE OFF SENT"<<endl;
}

void ofxPJControl::pjLink_get_power_status() {
    
//    ofLogNotice()<<"PJ get power "<<endl;
//    string msgRx = "";
//    string response = "";
//
//    ofLogNotice() << "received response: " << msgRx << endl;
    
    std::string msgRx = "";
    cout <<pjClient.receiveRaw()<<endl;
    attemptConnection();
    std::string command = "%POWR ?\r";
    
    
    if(connected){
        ofLogNotice() << "Attempting to Send Command : " << "POWR?" << endl;
        pjClient.sendRaw(command);
        

        ofLogNotice() << "Received response : " << msgRx << endl;
        //Really we should check if we get a valid response here...
        //        if(sendOn){
        //            projStatus = true;
        //        }else{
        //            projStatus = false;
        //        }
        closeConnection();
    }
    

    
}

void ofxPJControl::sanyo_On() {
	string command = "PWR ON\r";
	sendCommand(command, true);
    ofLogNotice()<<"PJ LINK SANYO ON SENT"<<endl;
}

void ofxPJControl::sanyo_Off() {
	string command = "PWR OFF\r";
	sendCommand(command, false);
    ofLogNotice()<<"PJ LINK SANYO ON SENT"<<endl;
}

void ofxPJControl::christie_On() {
	string command = "(PWR1)";
	sendCommand(command, true);
    ofLogNotice()<<"PJ LINK CHRISTIE ON SENT"<<endl;
}

void ofxPJControl::christie_Off() {
	string command = "(PWR0)";
	sendCommand(command, false);
    ofLogNotice()<<"PJ LINK CHRISTIE OFF SENT"<<endl;
}

void ofxPJControl::pjDesign_On() {
    string command = ":POWR 1\r";
    sendCommand(command, true);
    ofLogNotice()<<"PJ LINK ProjDesign ON SENT"<<endl;
}

void ofxPJControl::pjDesign_Off() {
    string command = ":POWR 0\r";
    sendCommand(command, false);
    ofLogNotice()<<"PJ LINK ProjDesign OFF SENT"<<endl;
}

void ofxPJControl::nec_On(){
    
    closeConnection(); //close any open connections first
    char* buffer = new char[6]; //02H 00H 00H 00H 00H 02H (the on command in hex)
    buffer[0] = 2;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 0;
    buffer[4] = 0;
    buffer[5] = 2;
    
    pjClient.setVerbose(true);
    if(!pjClient.isConnected()) {
        if(pingIPAddress(IPAddress)){
            //only try to establish tcp connection if ping is successful - otherwise, skip it because threading will lock up the app while waiting on a connection
            connected = pjClient.setup(IPAddress, NEC_PORT);
        }else{
            cout<<"Unable to ping! Did not attempt TCP Connection"<<endl;
        }
        if(connected){
            ofLogNotice() << " NEC connection established: " << IPAddress << ":" << NEC_PORT << endl;
        }
    }
    
    if(connected){
        ofLogNotice() << "Sending NEC Command: ON" << endl;
        
        pjClient.sendRawBytes(buffer, 6);
        
        printf("sent: %x %x %x %x %x %x\n",buffer[0] , buffer[1] , buffer[2] , buffer[3] , buffer[4] , buffer[5] );
        
        char* rxBuffer = new char[6];
        
        if(pjClient.receiveRawBytes(rxBuffer, 6)>0){ //message size should be something other than -1, otherwise, it was a failure
            
            printf("NEC Receive: %x %x %x %x %x %x\n",rxBuffer[0] , rxBuffer[1] , rxBuffer[2] , rxBuffer[3] , rxBuffer[4] , rxBuffer[5] );
            
            //Make sure we mark this ON
            projStatus = true;
            
            delete[] rxBuffer;
            delete[] buffer;
            closeConnection();
        }else{
            ofLogNotice()<<"Didn't receive response from NEC for off command" <<endl;
        }
    }else{
        ofLogNotice()<<"Not Connected for NEC ON Command" <<endl;
    }
}

void ofxPJControl::nec_Off() {
    
    char* buffer = new char[6]; //02H 01H 00H 00H 00H 03H (the off command in hex)
    buffer[0] = 2;
    buffer[1] = 1;
    buffer[2] = 0;
    buffer[3] = 0;
    buffer[4] = 0;
    buffer[5] = 3;
    
    projStatus = true;
    
    pjClient.setVerbose(true);
    
    if(!pjClient.isConnected()) {
        if(pingIPAddress(IPAddress)){
            //only try to establish tcp connection if ping is successful - otherwise, skip it because threading will lock up the app while waiting on a connection
            connected = pjClient.setup(IPAddress, NEC_PORT);
        }else{
            cout<<"Unable to ping! Did not attempt TCP Connection"<<endl;
        }
        if(connected){
            ofLogNotice() << " NEC connection established: " << IPAddress << ":" << NEC_PORT << endl;
        }
    }
    
    if(connected){
        ofLogNotice() << "Sending NEC Command: OFF " << endl;
        
        pjClient.sendRawBytes(buffer, 6);
        printf("NEC Send: %x %x %x %x %x %x\n",buffer[0] , buffer[1] , buffer[2] , buffer[3] , buffer[4] , buffer[5] );
        
        
        char* rxBuffer = new char[6];
        
        
        if(pjClient.receiveRawBytes(rxBuffer, 6)>0){ //message size should be something other than -1, otherwise, it was a failure
        
            printf("NEC Receive: %x %x %x %x %x %x\n",rxBuffer[0] , rxBuffer[1] , rxBuffer[2] , rxBuffer[3] , rxBuffer[4] , rxBuffer[5] );
        
            //Make sure we mark this OFF
            projStatus = false;
        
            delete[] rxBuffer;
            delete[] buffer;
            
            //we'll close the connection for every
            closeConnection();
            
        }else{
            ofLogNotice()<<"Didn't receive response from NEC for off command" <<endl;
        }
    }else{
        ofLogNotice()<<"Not Connected for NEC OFF Command" <<endl;
    }
}


