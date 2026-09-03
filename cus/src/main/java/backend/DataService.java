package backend;

import java.util.Objects;
import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpServerResponse;
import io.vertx.core.json.Json;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import io.vertx.ext.web.handler.StaticHandler;
import mqtt.EspData;
import serialcommunication.ArduinoData;

public class DataService extends AbstractVerticle {

    private final int port;
    private final EspData espData;
    private final ArduinoData arduinoData;

    public DataService(final int port, final EspData espData, final ArduinoData arduinoData) {
        this.port = port;
        this.espData = espData;
        this.arduinoData = arduinoData;
    }

@Override
    public void start() {
        Router router = Router.router(vertx);
        router.route().handler(BodyHandler.create());
        
        router.route("/*").handler(StaticHandler.create("../dbs").setCachingEnabled(false));
        
        router.post("/api/data").handler(this::handleAddNewData);
        router.get("/api/data").handler(this::handleGetData);
        
        vertx.createHttpServer().requestHandler(router).listen(port);
        log("Service ready on port: " + port);
    }

    private void handleAddNewData(final RoutingContext routingContext) {
        final HttpServerResponse response = routingContext.response();
        final JsonObject res = routingContext.getBodyAsJson();

        if (Objects.isNull(res)) {
            sendError(400, response);
            return;
        }

        // Il Frontend sottomette un Intento di cambio modalità
        if (res.containsKey("mode")) {
            try {
                this.arduinoData.setIntentExplicitMode(CusState.valueOf(res.getString("mode")));
            } catch (final IllegalArgumentException e) {
                System.err.println("Invalid mode received: " + res.getString("mode"));
            }
        }

        // Il Frontend sottomette un Intento di posizionamento valvola
        if (res.containsKey("valve")) {
            this.arduinoData.setIntentValve(res.getDouble("valve"));
        }

        response.setStatusCode(200).end();
    }

    private void handleGetData(final RoutingContext routingContext) {
        // Il GET restituisce la Telemetria ufficiale (lo stato fisico reale)
        final JsonObject data = new JsonObject()
                .put("waterLevel", this.espData.getWaterLevel())
                .put("mode", this.arduinoData.getTelemetryMode().toString())
                .put("valve", this.arduinoData.getTelemetryValve());

        routingContext.response().end(Json.encode(data));
    }

    private void sendError(int statusCode, HttpServerResponse response) {
        response.setStatusCode(statusCode).end();
    }

    private void log(String msg) {
        System.out.println("[DATA SERVICE] " + msg);
    }
}