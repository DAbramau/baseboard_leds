#include "config.h"

#include <Arduino.h>

#include <ESPmDNS.h>
#include <SPIFFS.h>
#include <Update.h>
#include <WiFi.h>

#include "ArduinoJson.h"
#include "ESPAsyncWebServer.h"

#include "neo_led.h"

/* -------------------------------------------------------------------------- */

#define TASK_WIFI_SERVER_STACK_SIZE         (3000)
#define TASK_WIFI_SERVER_PRIORITY           (configMAX_PRIORITIES - 5)  

static TaskHandle_t _wifi_server_task_handler;
static AsyncWebServer* _server;

/* -------------------------------------------------------------------------- */

static void _json_response(AsyncWebServerRequest* request, const DynamicJsonDocument &jsonDocument, int code = 200)
{
    log_i("[%d] code: %d type: %s", millis(), code, "application/json");
    String output;
    serializeJson(jsonDocument, output);
    log_i("content: %s", output.c_str());
    request->send(code, "application/json", output);
}

/* -------------------------------------------------------------------------- */

static void _request_get_info(AsyncWebServerRequest* request)
{
    DynamicJsonDocument jsonBuffer(2048);
    jsonBuffer["version"] = BUILD_VERSION;
    jsonBuffer["server"] = BUILD_SERVER;
    jsonBuffer["commit_sha"] = BUILD_COMMIT_SHA;
    jsonBuffer["build_date"] = __DATE__;
    jsonBuffer["build_time"] = __TIME__;
    jsonBuffer["wifi_ssid"] = WiFi.SSID();
    _json_response(request, jsonBuffer);
}

/* -------------------------------------------------------------------------- */

static void _request_next_mode(AsyncWebServerRequest* request)
{
    neo_led_next_mode();
    DynamicJsonDocument jsonBuffer(2048);
    jsonBuffer["status"] = "success";
    _json_response(request, jsonBuffer);
}

/* -------------------------------------------------------------------------- */

static void _add_route(const char* uri,
                       ArRequestHandlerFunction fn_get,
                       ArRequestHandlerFunction fn_post = nullptr)
{
    if (fn_get != nullptr)
    {
        _server->on(uri, HTTP_GET, [uri, fn_get](AsyncWebServerRequest* request) {
            log_i("[%d] request GET %s", millis(), uri);
            for (int i = 0; i < request->params(); ++i)
            {
                log_i("  [%s] -> %s", request->getParam(i)->name().c_str(), request->getParam(i)->value().c_str());
            }
            fn_get(request);
        });
    }

    if (fn_post != nullptr)
    {
        _server->on(uri, HTTP_POST, [uri, fn_post](AsyncWebServerRequest* request) {
            log_i("[%d] request POST %s", millis(), uri);
            for (int i = 0; i < request->params(); ++i)
            {
                log_i("  [%s] -> %s", request->getParam(i)->name().c_str(), request->getParam(i)->value().c_str());
            }
            fn_post(request);
        });
    }
}

/* -------------------------------------------------------------------------- */

static void wifi_server_task(void* parameter)
{
    String hostname = "BBL-" + String((uint32_t)(ESP.getEfuseMac()));
    log_i("Name: %s\n", hostname.c_str());
    WiFi.setHostname(hostname.c_str());

    log_i("connecting to SSID %s", CONFIG_WIFI_SSID);

    WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    WiFi.waitForConnectResult();

    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect();
        log_w("connection failed");
    }
    else
    {
        if (!MDNS.begin("bbl"))
        {
            log_w("Error starting mDNS");
        }

        _server = new AsyncWebServer(80);
        log_i("connected with ip: %s", WiFi.localIP().toString().c_str());

        if (SPIFFS.begin(true))
        {
            _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                        { request->redirect("/index.html"); });
            _server->serveStatic("/", SPIFFS, "/");

            _add_route("/info", _request_get_info);
            _add_route("next_mode", _request_next_mode);
            _server->on(
                "/uploadFW", HTTP_POST, [](AsyncWebServerRequest *request)
                {
                    AsyncWebServerResponse *response;
                    if (!Update.hasError())
                    {
                        log_w("update ok");
                        response = request->beginResponse(200, "text/plain", "OK");
                    }
                    else
                    {
                        log_w("update failed");
                        response = request->beginResponse(500, "text/plain", "FAIL");
                    }
                    response->addHeader("Connection", "close");
                    response->addHeader("Access-Control-Allow-Origin", "*");
                    request->send(response);
                    vTaskDelay(200);
                    esp_restart();
                },
                [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
                {
                    if (!index)
                    {
                        if (request->hasParam("fw", true) && (request->getParam("fw", true)->value().equals(filename)))
                        {
                            if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH))
                            {
                                Update.printError(Serial);
                                log_w("firmware update start fail");
                                return request->send(400, "text/plain", "OTA could not begin (fw)");
                            }
                        }
                        else if (request->hasParam("fs", true) && (request->getParam("fs", true)->value().equals(filename)))
                        {
                            if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS))
                            {
                                Update.printError(Serial);
                                log_w("spiffs update start fail");
                                return request->send(400, "text/plain", "OTA could not begin (fs)");
                            }
                        }
                        else
                        {
                            log_w("undefined OTA target");
                            return request->send(400, "text/plain", "OTA could not begin (undefined)");
                        }
                    }

                    if (len)
                    {
                        if (Update.write(data, len) != len)
                        {
                            log_w("update process failed");
                            return request->send(400, "text/plain", "OTA could not process OTA chank");
                        }
                    }

                    if (final)
                    {
                        if (!Update.end(true))
                        {
                            log_w("update finish failed");
                            Update.printError(Serial);
                            return request->send(400, "text/plain", "Could not end OTA");
                        }
                    }
                });
        }
        else
        {
            log_w("An Error has occurred while mounting SPIFFS");
        }
        _server->begin();
    }

    vTaskDelete(NULL);
}

/* -------------------------------------------------------------------------- */

void wifi_server_init(void)
{
    xTaskCreate(wifi_server_task,
                "wifi_server",
                TASK_WIFI_SERVER_STACK_SIZE,
                NULL,
                TASK_WIFI_SERVER_PRIORITY,
                &_wifi_server_task_handler);
}

/* -------------------------------------------------------------------------- */
