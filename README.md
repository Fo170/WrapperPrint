# WrapperPrint

[![GitHub](https://img.shields.io/badge/GitHub-Fo170/WrapperPrint-blue?logo=github)](https://github.com/Fo170/WrapperPrint)

Wrapper **multi-plateforme** `Print → WebServer::sendContent()` pour Arduino / ESP8266 / ESP32.

Il permet d'utiliser n'importe quelle librairie acceptant un `Print&` (comme `Graphique_Synchrone` ou `Graphique_Asynchrone`) pour streamer directement vers un client HTTP, sans allocation de `String` massive.

---

## Principe

Les serveurs web embarqués (ESP8266WebServer, WebServer ESP32, etc.) proposent une méthode `sendContent()` pour envoyer des données en mode **chunked** (transfert par morceaux). Cependant, elles n'héritent pas de `Print`.

`ServerPrint` hérite de `Print` et redirige chaque `write()` vers `sendContent()` avec un mini-buffer de 128 octets. Résultat :
- **Zéro-copie** : les données partent directement vers le client.
- **Pas de fragmentation mémoire** : pas de `String` de plusieurs ko.
- **Multi-plateforme** : un seul header pour ESP8266, ESP32, et au-delà.

---

## Installation

### PlatformIO (recommandé)

Ajoutez à `platformio.ini` :

```ini
lib_deps =
    https://github.com/Fo170/WrapperPrint.git@^1.0.0
```

### Arduino IDE

Copiez `WrapperPrint.h` dans le dossier `src/` ou `lib/ServerPrint/` de votre projet, ou téléchargez depuis [GitHub](https://github.com/Fo170/WrapperPrint).

---

## Utilisation

### ESP8266 / ESP32 (détection automatique)

```cpp
#include "WrapperPrint.h"

// ESP8266
ESP8266WebServer server(80);
// ou ESP32
// WebServer server(80);

void handleRoot() {
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");

    ServerPrint out(server);   // alias auto-detecte
    maLibrairie.streamVersPrint(out);

    out.flush();
    server.sendContent("");    // EOF chunked
}
```

### Autre plateforme (template explicite)

```cpp
#include "WrapperPrint.h"

MonWebServer serveur(80);

void handleRoot() {
    serveur.setContentLength(CONTENT_LENGTH_UNKNOWN);
    serveur.send(200, "text/html", "");

    ServerPrintT<MonWebServer> out(serveur);
    maLibrairie.streamVersPrint(out);

    out.flush();
    serveur.sendContent("");
}
```

---

## API

| Classe / Alias | Description |
|----------------|-------------|
| `ServerPrintT<ServerType>` | Template universel. `ServerType` doit posséder `sendContent(const String&)` |
| `ServerPrint` | Alias automatique sur ESP8266 (`ESP8266WebServer`) ou ESP32 (`WebServer`) |
| `write(uint8_t)` | Écrit un octet (bufferisé, flush à 128 octets) |
| `write(const uint8_t*, size_t)` | Écrit un bloc (gros blocs > 128 octets partent directement) |
| `flush()` | Vide le buffer interne vers `sendContent()` |

---

## Compatibilité

| Plateforme | Serveur web | Statut |
|------------|-------------|--------|
| ESP8266 | `ESP8266WebServer` | ✅ Natif |
| ESP32 | `WebServer` | ✅ Natif |
| Autre | Tout objet avec `sendContent(String)` | ✅ Via template explicite |

---

## Exemple complet avec Graphique_Asynchrone

```cpp
#include <ESP8266WebServer.h>
#include <Graphique_Asynchrone.h>
#include "WrapperPrint.h"

ESP8266WebServer server(80);
GraphiqueAsync gph(2, 60);

void handleGraph() {
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html; charset=utf-8", "");

    ServerPrint out(server);
    gph.streamTemplate(out);   // zero-copy

    out.flush();
    server.sendContent("");      // fin du chunked
}

void handleData() {
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "application/json", "");

    ServerPrint out(server);
    gph.streamDataJSON(out);     // zero-copy

    out.flush();
    server.sendContent("");
}
```

---

## Remarques

- Le buffer interne est limité à **128 octets** avant flush automatique. Cela garantit une latence faible sans surcharger le serveur web.
- N'oubliez pas d'appeler `out.flush()` puis `server.sendContent("")` à la fin pour fermer proprement le transfert chunked.
- Aucune dépendance externe : seul `Arduino.h` + le header du serveur web de votre plateforme sont requis.

---

## Licence

GPL-3.0 — Olivier FOURNET
