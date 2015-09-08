// JavaScript component of watchface to send and recieve messages with Pebble app on the phone.

var xhrRequest = function(url, type, callback){
  var xhr = new XMLHttpRequest();
  xhr.onload = function(){
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos){
  // Request for weather
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude;
  console.log(url);
  
  // Send a request to OpenWeatherMap.org
  xhrRequest(url, 'GET', 
            function(responseText){
              var json = JSON.parse(responseText);
              
              var temperature = Math.round(json.main.temp - 273.15);
              console.log('The temperature is ' + temperature);
              
              var conditions = json.weather[0].main;
              console.log('Conditions are ' + conditions);
              
              var dictionary = {
                'KEY_TEMPERATURE' : temperature,
                'KEY_CONDITIONS' : conditions
              };
              
              Pebble.sendAppMessage(dictionary,
                                   function(e){
                                     console.log('Weather info sent to Pebble succesfully!');
                                   },
                                   function(e){
                                     console.log('Error sending weather info to Pebble!');
                                   });
            });
}

function locationError(err){
  console.log('Error requesting location!');
}

function getWeather(){
  navigator.geolocation.getCurrentPosition(
  locationSuccess,
  locationError,
    {timeout: 15000, maximumAge: 60000}
  );
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