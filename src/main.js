Pebble.addEventListener('ready', function() {
  console.log('Pebblekit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
  var url = ''; // Past configuration page URL here
  
  console.log('Showing config page: %s', url);
  
  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  
  console.log('Config page returned: %s' + JSON.stringify(configData));
  
  if(configData.timeFormat) {
    // Send message to watchface.
  }
})
