/*
 * ******************************************************************************
 * Copyright (c) 2018 Robert Bosch GmbH and others.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/index.php
 *
 *  Contributors:
 *      Robert Bosch GmbH - initial API and functionality
 * *****************************************************************************
 */
#include "vssMapper.hpp"
#include "obd.hpp"
#include <string.h>
#include <iostream>
#include <json.hpp>

using namespace std;
using namespace jsoncons;

typedef unsigned char UInt8 ; 
typedef unsigned short int UInt16 ;
typedef unsigned long int UInt32 ;
typedef signed char Int8 ;
typedef short int Int16 ;
typedef long int Int32 ;
typedef float Float ;
typedef double Double ;
typedef bool Boolean ;

void tokenizeResponse(char** tokens , string response) {
  char* cString = strdup(response.c_str());
  char *tok = strtok(cString, " ");
  int i = 0;
  while (tok != NULL) {
    tokens[i++] = strdup(tok);
    tok = strtok(NULL, " ");
  }
  free(cString);
}

json setRequest(string path) {

  json req;
  req["requestId"] = rand() % 99999;
  req["action"]= "set";
  req["path"] = string(path);
  return req;
}



string setRPM() {

  string readBuf = readMode1Data("01 0C\r");
  int pos = readBuf.find("41 0C", 0);

  if( pos == -1) {
      cout << "Response not valid for RPM" << endl;
      return "Error";
  }

  string response = readBuf.substr (pos, 12);

  if (response.empty()) {
      cout << "Data is NULL form vehicle!" <<endl;
      return "Error";
  }
  
  char* tokens[4];
  tokenizeResponse(tokens , response);

  if(string(tokens[1]) != "0C"){
     cout<< "PID not matching for RPM!" <<endl;
     return "Error";
  }

  int A = stoi (string(tokens[2]),nullptr,16);
  int B = stoi (string(tokens[3]),nullptr,16);
  
  UInt16 value = (A * 256 + B) / 4;
 
  cout << "RPMread from the vehicle = "<< value << endl;

  json req = setRequest("Signal.OBD.EngineSpeed"); 
  req["value"] = value;
  stringstream ss; 
  ss << pretty_print(req);
  string resp = ss.str();
  cout << resp << endl;
  return resp;
}

string setVehicleSpeed() {

  string readBuf = readMode1Data("01 0D\r");
  int pos = readBuf.find("41 0D", 0);

  if( pos == -1) {
      cout << "Response not valid for Vehicle Speed" << endl;
      return "Error";
  }

  string response = readBuf.substr (pos, 9);

  if (response.empty()) {
      cout << "Data is NULL form vehicle!" <<endl;
      return "Error";
  }
  char* tokens[3];
  tokenizeResponse(tokens , response);

  if(string(tokens[1]) != "0D"){
     cout<< "PID not matching for vehicle speed!" <<endl;
     return "Error";
  }

  int A = stoi (string(tokens[2]),nullptr,16);
 
  Int32 value = A ;
 
  cout << "Vehicle speed read from the vehicle = "<< value << endl;
  json req = setRequest("Signal.OBD.Speed"); 
  req["value"] = value;
  stringstream ss; 
  ss << pretty_print(req);
  string resp = ss.str();
  cout << resp << endl;
  return resp;
}
