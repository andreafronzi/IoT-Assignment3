package backend;

import java.util.LinkedList;
import java.util.Objects;

import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpServerResponse;
import io.vertx.core.json.Json;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import mqtt.EspData;
import serialcommunication.ArduinoData;

public class DataService extends AbstractVerticle {

    private int port;
    private static final int MAX_SIZE = 10;
    private EspData espData;
    private ArduinoData arduinoData;

    public DataService(final int port, final EspData espData, final ArduinoData arduinoData) {
        this.port = port;
        this.espData = espData;
        this.arduinoData = arduinoData;
    }

    @Override
    public void start() {
        Router router = Router.router(vertx);
        router.route().handler(BodyHandler.create());
        router.post("/api/data").handler(this::handleAddNewData);
        router.get("/api/data").handler(this::handleGetData);
        vertx
                .createHttpServer()
                .requestHandler(router)
                .listen(port);

        log("Service ready on port: " + port);
    }

    private void handleAddNewData(final RoutingContext routingContext) {
        final HttpServerResponse response = routingContext.response();
        // log("new msg "+routingContext.getBodyAsString());
        final JsonObject res = routingContext.getBodyAsJson();
        if (Objects.isNull(res)) {
            sendError(400, response);
        } else {

            // Estrazione sicura dei valori dal JSON ricevuto dal client
            if (res.containsKey("waterLevel")) {
                this.espData.setWaterLevel(res.getDouble("waterLevel"));
            }
            if (res.containsKey("mode")) {
                try {
                    this.arduinoData.setState(CusState.valueOf(res.getString("mode")));
                } catch (IllegalArgumentException e) {
                    System.err.println("Invalid mode received: " + res.getString("mode"));
                }
            }
            if (res.containsKey("valve")) {
                this.arduinoData.setValve(res.getDouble("valve"));
            }
            
            // Chiude la richiesta HTTP dicendo al client che è andata a buon fine
            response.setStatusCode(200).end();
        }
    }

    private void handleGetData(final RoutingContext routingContext) {
        final JsonObject data = new JsonObject()
        .put("waterLevel", this.espData.getWaterLevel())
        .put("mode", this.arduinoData.getState().toString())
        .put("valve", this.arduinoData.getValve());

        routingContext.response()
                .end(Json.encode(data));
    }

    private void sendError(int statusCode, HttpServerResponse response) {
        response.setStatusCode(statusCode).end();
    }

    private void log(String msg) {
        System.out.println("[DATA SERVICE] " + msg);
    }

}
