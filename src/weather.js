// JavaScript component of watchface to send and recieve messages with Pebble app on the phone.

// Request wrapper
var xhrRequest = function(url, type, callback){
  var xhr = new XMLHttpRequest();
  xhr.onload = function(){
    callback(this.responseText);
  };
  xhr.onerror = function() {
    sendAppMessage({'KEY_ERROR': 'req. err'});
  };
  xhr.ontimeout = function() {
    sendAppMessage({'KEY_ERROR': 'no net'});
  };
  xhr.open(type, url);
  xhr.send();
};

// On success location, make http request for weather
function locationSuccess(pos){
	var weatherApiKey = ''; // Please, get your key on openweathermap.org it's free
  // Request for weather
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + 
      '&appid=' + weatherApiKey;
  
  // Send a request to OpenWeatherMap.org
  xhrRequest(url, 'GET', 
            function(responseText){
              console.log(responseText);
              try{
                var json = JSON.parse(responseText);
                var temperature = Math.round(json.main.temp - 273.15);              
                var conditions = json.weather[0].main;
                
                var dictionary = {
                  'KEY_TEMPERATURE' : temperature,
                  'KEY_CONDITIONS' : conditions
                };
                
                sendAppMessage(dictionary);
              } catch (e) {
                sendAppMessage({'KEY_ERROR': 'parse err'});
              }
            });
}

// On error location
function locationError(err){
  console.log('Error requesting location!');
  sendAppMessage({'KEY_ERROR' : 'no loc.'});
}

// Make request for weather
function getWeather(){
  // First get location
  navigator.geolocation.getCurrentPosition(
  locationSuccess,
  locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Sends message to Pebble
function sendAppMessage(message){
  Pebble.sendAppMessage(message,
                        function(e){
                          console.log('Weather info sent to Pebble succesfully!');
                        },
                        function(e){
                          console.log('Error sending weather info to Pebble!');
                        });
}

Pebble.addEventListener('ready', 
                        function(e){
                          console.log('PebbleKit JS ready!');
                          
                          // Get the initial weather
                          getWeather();
                        });

Pebble.addEventListener('appmessage',
                       function(e){
                         console.log('AppMessage received!');
                       });

Pebble.addEventListener('appmessage', function(e){
  console.log('AppMessage received!');
  getWeather();
});