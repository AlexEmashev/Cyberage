(function() {
  loadOptions();
  submitHandler();
})(); // Kind of a constructor for the page.

function submitHandler() {
  var $submitButton = $('#submitButton'); // Using slate.js with syntactic sugar, like jQuery. Actually it's Zepto.js

  // 
  $submitButton.on('click', function() {
    // URL for passing pebble kit is 'pebblejs://close#'
    var return_to = getQueryParam('return_to', 'pebblejs://close#');
    // Redirect vebview to pebblejs://close# url and pass the data
    document.location = return_to + encodeURIComponent(JSON.stringify(getAndStoreConfigData()));
  });  
}


// Get data from config page
function getAndStoreConfigData() {
  var $timeFormat = $('#timeFormat');
  
  var options = {
    militaryTime: $timeFormat[0].checked
  };
  
  // Store values to local storage, to load saved config
  localStorage.militaryTime = options.militaryTime;
  console.log('Options are:' + JSON.stringify(options));
  return options;
}

// Decode URI to get 
function getQueryParam(variable, defaultValue) {
  var query = location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i++) {
    var pair = vars[i].split('=');
    if(pair[0] === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return defaultValue || false;
}

// Load stored options to the config page
function loadOptions() {
  var $timeFormat = $('#timeFormat');
  
  if(localStorage.militaryTime) {
    $timeFormat[0].checked = localStorage.militaryTime === 'true';
  }
}
