const char MAIN_page[] PROGMEM = R"=====(
<HTML>
	<HEAD>
			<!-- <TITLE>Weather Station Settings</TITLE> -->
	</HEAD>
<BODY>
	<CENTER>
			<B>Wifi settings</B>
            <form name="wifi_save" method="POST" action="/wifi_save">
                <p><b>SSID:</b><br>
                 <input name="ssid" type="text" size="40">
                </p>

                <p><b>Password:</b><br>
                    <input name="pass" type="password" size="40">
                </p>

                <p><b>Server (IP address):</b><br>
                    <input name="host" type="text" size="40">
                </p>

                <p><input type="submit" value="Save and reboot"></p>
            </form>

               </body>
	</CENTER>	
</BODY>
</HTML>
)=====";
