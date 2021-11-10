const char MAIN_page1[] PROGMEM = R"=====(
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
      <style>
        body {
          background: linear-gradient(45deg, #EECFBA, #C5DDE8);
          /* background: linear-gradient(to top left, powderblue, pink); */
        }
        p {
          margin: 0;
          text-align:center;
          font: 20px monospace;
        }
        .form {
          background-color: rgba(255, 255, 255, 0.5);
          padding: 10px 20px 10px 20px;
          position: fixed; top: 50%; left: 50%;
          -webkit-transform: translate(-50%, -50%);
          -ms-transform: translate(-50%, -50%);
          transform: translate(-50%, -50%);
          border-radius:10px;
          box-shadow: 0px 0px 0px 1px #38577a, 0px 0px 10px #7e9bbd;
        }
        @media (max-width:768px) {
          .form {
            width: 80%
          }
        }

        .select {
          width: 100%;
          height: 40px;
          margin: 10px 0 10px 0;
          -moz-appearance: none;
          -webkit-appearance: none;
          appearance: none;
          border: 1px solid black;
          border-radius: 10px;
          padding: 0 35px 0 15px;
          outline: none;
          cursor: pointer;
          font: 15px monospace;
        }
        .select:focus {
          outline: none;
          box-shadow: 0px 0px 0px 1px #38577a, 0px 0px 10px #7e9bbd;
        }
        .select-wrapper {
          position: relative;
        }
        .select-wrapper::before {
          content: "";
          position: absolute;
          top: 50%;
          right: 15px;
          transform: translateY(-50%);

          display: block;
          width: 0;
          height: 0;
          border-style: solid;
          border-width: 10.4px 6px 0 6px;
          border-color: #38577a transparent transparent transparent;

          pointer-events: none;
        }

        .input {
          width: 100%;
          height: 40px;
          margin: 10px 0 10px 0;
          -moz-appearance: none;
          -webkit-appearance: none;
          appearance: none;
          border: 1px solid black;
          border-radius: 10px;
          padding: 0 15px 0 15px;
          outline: none;
          font: 15px monospace;
        }
        .input:focus {
          outline: none;
          box-shadow: 0px 0px 0px 1px #38577a, 0px 0px 10px #7e9bbd;
        }
        #password_input {
          height: 0;
          margin: 0;
          padding: 0;
          opacity: 0;

          transition: all .3s ease-out;
        }
        #password_input.active {
          height: 40px;
          margin: 10px 0 10px 0;
          padding: 0 15px 0 15px;
          opacity: 1;

          transition: all .3s ease-out;
        }

        .button {
          width: 100%;
          height: 40px;
          margin: 10px 0 10px 0;
          background-color: #ffefe5;
          -moz-appearance: none;
          -webkit-appearance: none;
          appearance: none;
          border: 1px solid black;
          border-radius: 10px;
          outline: none;
          font: 20px monospace;
          transition: .2s;
        }
        .button:hover {
          transition: .3s;
          box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);
        }
        .button:focus {
          outline: none;
          box-shadow: 0px 0px 0px 1px #38577a, 0px 0px 10px #7e9bbd;
        }
      </style>

      <script>
        document.addEventListener("DOMContentLoaded", function() {
          // Display password input
          let select = document.querySelector('.select');
          let password_input = document.querySelector('#password_input');

          function pswdFieldVisability() {
            if (select.options[select.selectedIndex].classList.contains('pswd')) {
              password_input.value = '';
              password_input.classList.add("active");
              password_input.required = true;
              password_input.removeAttribute('disabled');
            } else {
              password_input.value = '';
              password_input.classList.remove("active");
              password_input.required = false;
              password_input.setAttribute('disabled', 'disabled');
            }
          }

          pswdFieldVisability();
          select.addEventListener('change', pswdFieldVisability);

          // Mask for server address input
          let address_input = document.querySelector('#address_input');

          function moveCaret(input, s) {
            let val = input.value;
            while (!/\d/g.test(val[s-1]) && s > 0) {
              s--;
            }
            if (val[s] == '.' || val[s] == ':') {
              let prevVal = val.substring(s-3, s).split('.');
              prevVal = prevVal[prevVal.length - 1];
              if ((prevVal.length == 1 && parseInt(prevVal) == 0) ||
                  (prevVal.length == 2 && parseInt(prevVal) > 25)  ||
                  (prevVal.length == 3)) {
                s++;
              }

            }
            if (s < 0) s = 0;
            input.setSelectionRange(s, s);
          }

          // Mouse over and out
          address_input.addEventListener('mouseover', function() { this.placeholder = '_._._._:_'; });
          address_input.addEventListener("mouseout", function() { this.placeholder = 'address'; });
          // Move caret on click
          address_input.addEventListener('click', function() { moveCaret(this, this.selectionStart); });
          // Focus and blur
          address_input.addEventListener('blur', function() { if (this.value == "_._._._:_") this.value = ""; });
          address_input.addEventListener('focus', function() {
            if (this.value == "") {
              this.value = "_._._._:_";
              moveCaret(this, this.selectionStart);
            }
          });
          // Cancel paste
          address_input.addEventListener('paste', function() { event.preventDefault(); });
          // Input data
          address_input.addEventListener('input', function() {
            let inputAddress = this.value.replace(/[^\.:\d]/g, ""),
                octets = inputAddress.split(/\.|:/g),
                selectionStart = this.selectionStart,
                formattedInputValue = '';

            // Input octets validation first 0 or octet > 255
            for (let i = 0; i < 4; i++) {
              let octet = octets[i];
              if (octet) {
                if (octet[0] == '0' && octet.length != 1) {
                  if (this.value[selectionStart] == 0) {
                    this.value = this.value.substring(0, selectionStart) + this.value.substring(selectionStart + 1);
                  } else {
                    this.value = this.value.substring(0, selectionStart - 1) + this.value.substring(selectionStart);
                  }
                  // moveCaret(this, selectionStart);
                  moveCaret(this, selectionStart - 1);
                  return;
                }
                if (parseInt(octet) > 255) {
                    this.value = this.value.substring(0, selectionStart - 1) + this.value.substring(selectionStart);
                    moveCaret(this, selectionStart - 1);
                    return;
                }
                // Add a formatted octet
                formattedInputValue += octet + ((i !=3 ) ? '.' : ':');
              } else {
                formattedInputValue += '_' + ((i !=3 ) ? '.' : ':');
              }
            }

            // Input port validatuin (< 65 353)
            let port = octets[4];
            if (port) {
              if (port[0] == '0' && port.length != 1) {
                if (this.value[selectionStart] == 0) {
                  this.value = this.value.substring(0, selectionStart) + this.value.substring(selectionStart + 1);
                } else {
                  this.value = this.value.substring(0, selectionStart - 1) + this.value.substring(selectionStart);
                }
                moveCaret(this, selectionStart - 1);
                return;
              }
              if (parseInt(port) > 65535) {
                  this.value = this.value.substring(0, selectionStart - 1) + this.value.substring(selectionStart);
                  moveCaret(this, selectionStart - 1);
                  return;
              }
              formattedInputValue += port;
            } else formattedInputValue += '_';

            this.value = formattedInputValue;
            moveCaret(this, selectionStart);
          });
          // Handling Backspace and Delete
          address_input.addEventListener('keydown', function(e) {
            let val = this.value;
            let s = this.selectionStart;
            let key = e.which || e.keyCode || e.charCode;

            // Handling Backspace
            if (key === 8 && /[\.:]/g.test(val[s-1])) {
              event.preventDefault();
              this.setSelectionRange(s-1, s-1);
            }
            // Handling Delete
            if (key === 46) {
              if (/[\.:]/g.test(val[s])) {
                event.preventDefault();
                this.setSelectionRange(s+1, s+1);
              }
              else if (/[\.:]/g.test(val[s-1])) {
                event.preventDefault();
                if (val[s] != '_') {
                  this.value = this.value.substring(0, s) +
                               (( (/[\.:]/g.test(val[s+1]) ) || !val[s+1] ) ? '_' : '') +
                               this.value.substring(s + 1);
                  this.setSelectionRange(s, s);
                }

              }
            }
            // Handling Space
            if (key === 32) {
              event.preventDefault();
              if (/[\.:]/g.test(val[s])) this.setSelectionRange(s+1, s+1);
            }

            // Handling Android keyboard
            if (key === 229) {
              if (/[\.:]/g.test(val[s])) setTimeout(() => this.setSelectionRange(s+1, s+1), 0);
            }
          });

        });
      </script>

  </head>
  <body>
    <form class="form" name="wifi_save" method="POST" action="wifi_save">

    <p>Wi-Fi</p>

    <div class="select-wrapper">
      <select class="select" required name="ssid">

)=====";
// Insert list of wifi devices
const char MAIN_page2[] PROGMEM = R"=====(
      </select>
    </div>

        <input id="password_input" class="input active" name="pass" type="password" placeholder="password">

        <p>Timer</p>
        <div class="select-wrapper">
          <select class="select" required name="timer">
)=====";
// Insert timer list
const char MAIN_page3[] PROGMEM = R"=====(
          
          </select>
        </div>


        <p>Server</p>
        <input id="address_input" class="input" name="address" type="tel" required='on'
                pattern="((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?):(\d{1,5})"
                 placeholder="address" autocomplete="off"
)=====";
// Insert host value
const char MAIN_page4[] PROGMEM = R"=====(   
                 >


        <input class="button" type="submit" value="Save">
        </form>
      </body>
    </html>
)=====";


const char SAVE_page[] PROGMEM = R"=====(
  <html>
  	<head>
  		<meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
        <style>
  				body {
  					background: linear-gradient(45deg, #EECFBA, #C5DDE8);
  					/* background: linear-gradient(to top left, powderblue, pink); */
  				}
  				p {
  					margin: 0;
  					text-align:center;
  					font: 20px monospace;
  				}
  				.form {

  					background-color: rgba(255, 255, 255, 0.5);
  					padding: 10px 20px 10px 20px;
  					position: fixed; top: 50%; left: 50%;
  					-webkit-transform: translate(-50%, -50%);
  					-ms-transform: translate(-50%, -50%);
  					transform: translate(-50%, -50%);
  					border-radius:10px;
  					box-shadow: 0px 0px 0px 1px #38577a, 0px 0px 10px #7e9bbd;
            max-width: 400px;
  				}

  				@media (max-width:768px) {
  				  .form {
  						width: 80%
  					}
  				}


  				.button {
  						width: 100%;
  						height: 40px;
  						margin: 10px 0 10px 0;
  						background-color: #ffefe5;
  						-moz-appearance: none;
  						-webkit-appearance: none;
  						appearance: none;
  						border: 1px solid black;
  						border-radius: 10px;
  						outline: none;
  						font: 20px monospace;
  						transition: .2s;
  					}

  					.button:hover {
  						transition: .3s;
  						box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24), 0 17px 50px 0 rgba(0,0,0,0.19);
  					}

  					.button:focus {
  						outline: none;
  						box-shadow: 0px 0px 0px 1px #38577a, 0px 0px 10px #7e9bbd;
  					}
        </style>

  	</head>
  	<body>
  		<form class="form" name="reboot" method="GET" action="reboot">
    		<p>The data was saved successfully. Reboot the station in hardware (using the reboot button on the device) or by clicking on the button below.</p>
    		<input class="button" type="submit" value="Reboot">
  		</form>
  	</body>
  </html>

)=====";
