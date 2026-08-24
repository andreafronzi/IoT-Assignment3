package esiot.backend;

import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpServerResponse;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import io.vertx.ext.web.handler.StaticHandler;

import java.util.LinkedList;

public class DataService extends AbstractVerticle {

	private int port;
	private static final int MAX_SIZE = 50;
	private LinkedList<DataPoint> values;
	private CusLogic cusLogic;
	
	public DataService(int port) {
		values = new LinkedList<>();		
		this.port = port;
	}
	
	public void setCusLogic(CusLogic logic) {
	    this.cusLogic = logic;
	}

	@Override
	public void start() {		
		Router router = Router.router(vertx);
		
		// Serve static files (HTML, CSS, JS) from "webroot" directory
		router.route("/*").handler(StaticHandler.create("webroot"));
		
		router.route().handler(BodyHandler.create());
		
		// API Endpoints
		router.get("/api/data").handler(this::handleGetData);		
		router.get("/api/state").handler(this::handleGetState);		
		router.post("/api/mode").handler(this::handleSetMode);
		router.post("/api/valve").handler(this::handleSetValve);
		
		vertx
			.createHttpServer()
			.requestHandler(router)
			.listen(port);

		log("Service ready on port: " + port);
	}
	
	public synchronized void addDataPoint(double value) {
	    values.addLast(new DataPoint(value, System.currentTimeMillis(), "tank"));
	    if (values.size() > MAX_SIZE) {
	        values.removeFirst();
	    }
	}
	
	private synchronized void handleGetData(RoutingContext routingContext) {
		JsonArray arr = new JsonArray();
		for (DataPoint p: values) {
			JsonObject data = new JsonObject();
			data.put("time", p.getTime());
			data.put("value", p.getValue());
			arr.add(data);
		}
		routingContext.response()
			.putHeader("content-type", "application/json")
			.end(arr.encode());
	}
	
	private void handleGetState(RoutingContext routingContext) {
	    JsonObject state = new JsonObject();
	    if (cusLogic != null) {
	        state.put("mode", cusLogic.getCurrentMode());
	        state.put("valveOpening", cusLogic.getValveOpening());
	    } else {
	        state.put("mode", "NOT AVAILABLE");
	        state.put("valveOpening", 0);
	    }
	    routingContext.response()
			.putHeader("content-type", "application/json")
			.end(state.encode());
	}
	
	private void handleSetMode(RoutingContext routingContext) {
	    JsonObject body = routingContext.getBodyAsJson();
	    if (body != null && body.containsKey("mode") && cusLogic != null) {
	        String mode = body.getString("mode");
	        cusLogic.setModeFromDashboard(mode);
	        routingContext.response().setStatusCode(200).end();
	    } else {
	        routingContext.response().setStatusCode(400).end();
	    }
	}
	
	private void handleSetValve(RoutingContext routingContext) {
	    JsonObject body = routingContext.getBodyAsJson();
	    if (body != null && body.containsKey("valve") && cusLogic != null) {
	        int valve = body.getInteger("valve");
	        cusLogic.setValveFromDashboard(valve);
	        routingContext.response().setStatusCode(200).end();
	    } else {
	        routingContext.response().setStatusCode(400).end();
	    }
	}

	private void log(String msg) {
		System.out.println("[DATA SERVICE] "+msg);
	}
}
