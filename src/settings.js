Pebble.addEventListener('ready', function() {
  console.log('Pebblekit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
  var url = 'http://alexemashev.github.io/Cyberage'; // Past configuration URL here
  
  console.log('Showing config page: %s', url);
  
  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  
  console.log('Config page returned: %s' + JSON.stringify(configData));
  
  if(Object.keys(configData).length > 0) {
    Pebble.sendAppMessage({
      'KEY_MILITARY_TIME': configData.militaryTime,
      'KEY_TEMPC': configData.temperatureCelsius,
      'KEY_DATEDDMM': configData.dateDDMM
    }, function() {
      console.log('Settings send successful to watch!');
    }, function() {
      console.log('Settings send failed to watch!');
    });
  }
});
