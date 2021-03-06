

char configMsg[MAX_CONFIG_LEN];



void waitForParams(void) {
  boolean done = false;

  // Loop until a valid config message is received
  while (! done) {

    // Loop until a line starting with "config:" is received
    uint8_t configMsgLen = 0;
    boolean msgComplete = false;
    while (! msgComplete) {
      if (Serial.available()) {
        char ch = Serial.read();
        if (ch == '\n' || ch == '\r') {
          msgComplete = true;
          configMsg[configMsgLen] = 0;  // Zero terminate
        } else {
          configMsg[configMsgLen] = ch;
          configMsgLen++;
          if (configMsgLen > MAX_CONFIG_LEN) {
            Serial.println(MSG_HEADER " ERR: Config too long");
            configMsgLen = 0; // discard all we have got so far
          }
          if (memcmp(configMsg, CONFIG_MSG_START, min(uint8_t(strlen(CONFIG_MSG_START)), configMsgLen)) != 0 ) {
            configMsgLen = 0; // discard all we have got so far
          }
        }
      }
      yield();
    }

    // Analyze the message
    int i;
    if (strlen(configMsg) >= strlen(CONFIG_MSG_START)) {
      // Split message into parameters
      char* p = configMsg + sizeof(CONFIG_MSG_START) - 1; // start after the "config:"
      for (i = 0; i < NBPARAMS && *p; i++) {
        params[i].valueStr = p;
        p = strchr(p, ',');
        if (p) {
          *p = 0;
          p++;
        } else {
          break;
        }
      }
      if (i == NBPARAMS - 1) {
        // Correct number of parameters, done
        done = true;
        Serial.println(MSG_HEADER " OK");
      } else {
        Serial.println(MSG_HEADER " ERR: Not enough parameters");
        configMsgLen = 0;
      }
    } else {
      Serial.println(MSG_HEADER " ERR: Expected \"" CONFIG_MSG_START "...\"");
    }

    delay(100);
    flushInput();
  }

}



void printParams(void) {
  int i;
  Serial.println(MSG_HEADER " Configuration parameters:");
  for (i = 0; i < NBPARAMS; i++) {
    Serial.print(MSG_HEADER "    ");
    Serial.print(params[i].name);
    Serial.print(" = \"");
    Serial.print(params[i].valueStr);
    Serial.println("\"");
  }
}

void str2IpAddr(const char* str, IPAddress* ip) {
  int i;
  for (i = 0; i < 4; i++) {
    (*ip)[i] = atoi(str);
    str = strchr(str, '.');
    if (str)
      str++;
    else
      break;
  }
}
