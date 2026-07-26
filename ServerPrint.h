/*
  ServerPrint.h - Wrapper Print -> WebServer::sendContent (chunked), multi-plateforme
  Auteur    : Olivier FOURNET
  License   : GPL-3.0

  Compatible ESP8266, ESP32 et toute plateforme dont le serveur Web expose sendContent().

  Utilisation :
    #include "ServerPrint.h"

    // ESP8266 / ESP32 : alias automatique
    ServerPrint out(server);

    // Autre plateforme (template explicite)
    ServerPrintT<MonWebServer> out(server);

    maLibrairie.streamXXX(out);   // zero-copy vers le client HTTP
*/

#ifndef SERVER_PRINT_H
#define SERVER_PRINT_H

#include <Arduino.h>

// ============================================================
// INCLUDES PAR PLATEFORME
// ============================================================
#if defined(ESP8266)
  #include <ESP8266WebServer.h>
#elif defined(ESP32)
  #include <WebServer.h>
#endif

// ============================================================
// TEMPLATE UNIVERSEL
// ============================================================
template <typename ServerType>
class ServerPrintT : public Print {
public:
    explicit ServerPrintT(ServerType& s) : server(s) {}

    size_t write(uint8_t c) override {
        buf += (char)c;
        if (buf.length() >= 128) flush();
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size) override {
        if (size >= 128) {
            flush();
            String s;
            s.reserve(size);
            for (size_t i = 0; i < size; i++) s += (char)buffer[i];
            server.sendContent(s);
            return size;
        }
        for (size_t i = 0; i < size; i++) buf += (char)buffer[i];
        if (buf.length() >= 128) flush();
        return size;
    }

    void flush() {
        if (buf.length() > 0) {
            server.sendContent(buf);
            buf = "";
        }
    }

private:
    ServerType& server;
    String buf;   // buffer local, jamais > ~128 octets
};

// ============================================================
// ALIAS PAR PLATEFORME (compatibilite ascendante)
// ============================================================
#if defined(ESP8266)
  using ServerPrint = ServerPrintT<ESP8266WebServer>;
#elif defined(ESP32)
  using ServerPrint = ServerPrintT<WebServer>;
#endif

#endif // SERVER_PRINT_H
