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
       server.createContext("/api/weather", (exchange -> {
            if ("PUT".equals(exchange.getRequestMethod())) { // PUT request
             String reqText = exchange.getRequestURI().getRawQuery(); // String data from request

             // Write data from request to CSV file
             int[] data = getDataFromStringRequest(reqText);
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

   public static int[] getDataFromStringRequest(String query) {
    String[] sData = query.split("&");
    int[] iData = new int[2];
    iData[0] = Integer.parseInt(sData[0].split("=")[1]);
    iData[1] = Integer.parseInt(sData[1].split("=")[1]);
    return iData;
   }

   public static void writeData(int temp, int humidity) {
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
      } catch (Exception e) {
          e.printStackTrace();
      }
   }
}
