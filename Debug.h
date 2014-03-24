#define DEBUGLN(msg, ...)  \
      do { if (DEBUG_OUTPUT) Serial.println(msg, ##__VA_ARGS__); } while (0)
#define DEBUG(msg, ...)  \
      do { if (DEBUG_OUTPUT) Serial.print(msg, ##__VA_ARGS__); } while (0)      
