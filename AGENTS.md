# AGENTS.md

## Repo structure

Single-header Arduino library (`WrapperPrint.h`) — no build system, no tests, no CI, no package manifests.

## Key architecture

- `ServerPrintT<ServerType>` wraps any object with `sendContent(const String&)` into an Arduino `Print` subclass.
- Platform aliases: `ServerPrint` = `ServerPrintT<ESP8266WebServer>` on ESP8266, `ServerPrintT<WebServer>` on ESP32.
- Platform detection is compile-time via `#if defined(ESP8266)` / `#elif defined(ESP32)` — only these two are auto-detected. Other platforms must use `ServerPrintT<YourServer>` explicitly.
- Internal buffer is 128 B — `write()` flushes automatically when full; `write(const uint8_t*, size_t)` sends blobs >= 128 B directly.

## Required usage pattern

```cpp
server.setContentLength(CONTENT_LENGTH_UNKNOWN);
server.send(200, "text/html", "");

ServerPrint out(server);
yourLib.streamToPrint(out);

out.flush();                // drain remaining buffer
server.sendContent("");     // signal EOF (chunked terminator)
```

Forgetting `flush()` or the final `sendContent("")` will produce truncated or malformed chunked responses.

## Languages

README is in French. Header comments are bilingual (French + English). Keep both when editing.

## No tests or CI

The only verification is manual compile-check on target (ESP8266/ESP32). When editing, ensure compatibility with both `ESP8266WebServer` and `WebServer` APIs.
