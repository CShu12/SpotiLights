# SpotiLights
Visualize the song you're currently listening to on Spotify! Watch as the 
colors and rhythm of the LEDs change with the song! This project is a 
submission for ESE 111 Fall 2020 at the University of Pennsylvania. See 
[this Devpost][] for more information.

## Configuration
Set the `WIFI_SSID`, `WIFI_PASS`, and `IP_ADDRESS` constants of 
*`LED_control.ino`* in a *`config.h`* file placed in the *`LED_control`* 
directory. Run `npm i` in the *`server`* directory to install the web server's 
dependencies. Set the `CLIENT_ID`, `CLIENT_SECRET`, and `REDIRECT_URI`
environment variables in a .env file placed in the *`server`* directory.

## Running the server
Run `node app.js` in the *`server`* directory to make the webpage available on 
port 8888 of the machine.

[this Devpost]: https://devpost.com/software/spotilights/
