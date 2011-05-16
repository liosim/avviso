//  Copyright (C) 2011 Mike Colagrosso
//  http://colagrosso.net
//
//  This file is part of Avviso: iOS push notifications through Prowl
//
//  Avviso is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as
//  published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version.
//
//  Avviso is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with Avviso. If not, see
//  <http://www.gnu.org/licenses/>.
//

#include "WProgram.h"
#include "Avviso.h"
#include <string.h>
#include <EthernetDHCP.h>
#include <EthernetDNS.h>
#include <HTTPClient.h>

#define PROWL_API_HOST "api.prowlapp.com"
#define PUSH_PATH_URI "/publicapi/add"

#define AVVISO_DEBUG 0

AvvisoClass::AvvisoClass() {
}

void AvvisoClass::begin() {
  
  // // At the time of writing,
  // prowlIpAddr should be {209, 20, 72, 170};
  DNSError err = EthernetDNS.resolveHostName(PROWL_API_HOST, prowlIpAddr);
  
  if (AVVISO_DEBUG) {
    if (DNSSuccess == err) {
      Serial.print("The IP address of ");
      Serial.print(PROWL_API_HOST);
      Serial.print(" is: ");
      static char buf[16];
      sprintf(buf, "%d.%d.%d.%d\0", prowlIpAddr[0], prowlIpAddr[1], prowlIpAddr[2], prowlIpAddr[3]);
      Serial.print(buf);
      Serial.println(".");
    } else if (DNSTimedOut == err) {
      Serial.println("Timed out.");
    } else if (DNSNotFound == err) {
      Serial.println("Does not exist.");
    } else {
      Serial.print("Failed with error code ");
      Serial.print((int)err, DEC);
      Serial.println(".");
    }
  }
}

void AvvisoClass::setApiKey(char *aApiKey) {
  strncpy(apiKey, aApiKey, 41);
}

void AvvisoClass::setApplicationName(char *aApplicationName) {
  strncpy(applicationName, aApplicationName, 257);
}

int AvvisoClass::push(char *eventStr, char *messageStr, int priority) {
  if (AVVISO_DEBUG) {
    Serial.println(apiKey);  
    Serial.println(applicationName);  
    Serial.println(eventStr);  
    Serial.println(messageStr);  
    Serial.println(priority);   
  }
  char priorityStr[5];
  sprintf(priorityStr, "%d", priority);

  HTTPClient client(PROWL_API_HOST, prowlIpAddr);
  http_client_parameter prowl_arguments[] = {
      { "apikey", apiKey }, 
      { "application", applicationName }, 
      { "event", eventStr }, 
      { "description", messageStr }, 
      { "priority", priorityStr }, 
    { NULL ,NULL }
  };
  FILE* result = client.getURI(PUSH_PATH_URI, prowl_arguments);
  int returnCode = client.getLastReturnCode();
  if (result!=NULL) {
    client.closeStream(result);
  } 
  else {
    if (AVVISO_DEBUG) {
      Serial.println("failed to connect");      
    }
  }
  if (returnCode==200) {
    if (AVVISO_DEBUG) {
      Serial.println("data uploaded");
    }
  } 
  else {
    if (AVVISO_DEBUG) {
      Serial.print("ERROR: Server returned ");
      Serial.println(returnCode);
    }
  }
  
  return returnCode;
}

AvvisoClass Avviso;