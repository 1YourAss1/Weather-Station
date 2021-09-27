import java.io.IOException;
import java.io.OutputStream;
import java.io.File;
import java.io.FileWriter;
import java.net.InetSocketAddress;
import java.util.Date;

import com.sun.net.httpserver.HttpServer;

class Rest {

   public static void main(String[] args) throws IOException {
       int serverPort = 80;
       HttpServer server = HttpServer.create(new InetSocketAddress(serverPort), 0);
       server.createContext("/weather", (exchange -> {
            if ("PUT".equals(exchange.getRequestMethod())) { // PUT request
             String reqText = exchange.getRequestURI().getRawQuery(); // String data from request
             System.out.println(reqText);

             // Write data from request to CSV file
             float[] data = getDataFromStringRequest(reqText);
             writeData(data[0], data[1]);

             // Response
             String response = "Success!";
             exchange.sendResponseHeaders(200, response.getBytes().length);
             OutputStream output = exchange.getResponseBody();
             output.write(response.getBytes());
             output.flush();
            } else if ("GET".equals(exchange.getRequestMethod())) {
              // Response
              String response = "GET";
              System.out.println(response);
              exchange.sendResponseHeaders(200, response.getBytes().length);
              OutputStream output = exchange.getResponseBody();
              output.write(response.getBytes());
              output.flush();
            } else {
              exchange.sendResponseHeaders(405, -1);// 405 Method Not Allowed
            }
            exchange.close();
      }));
       server.setExecutor(null);
       server.start();
   }

   public static float[] getDataFromStringRequest(String query) {
    String[] sData = query.split("&");
    float[] fData = new float[2];
    fData[0] = Float.parseFloat(sData[0].split("=")[1]);
    fData[1] = Float.parseFloat(sData[1].split("=")[1]);
    return fData;
   }

   public static void writeData(float temp, float humidity) {
      try {
          File dataFile = new File("data.csv");
          FileWriter writer;
          if (!dataFile.exists()) {
            writer = new FileWriter(dataFile, false);
            writer.write("Date;Temperature;Humidity\n");
          } else {
            writer = new FileWriter(dataFile, true);
          }
          writer.write(String.valueOf(new Date().getTime()));
          writer.append(';');
          writer.write(String.valueOf(temp));
          writer.append(';');
          writer.write(String.valueOf(humidity));
          writer.append('\n');
          writer.flush();
          writer.close();
      } catch (Exception e) {
          e.printStackTrace();
      }
   }
}
