// Author: Ed Dam

// clayjs
var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

// xml http request
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

// talk to OpenWeather
function locationSuccess(pos) {
  var apiKey = '42efb17b4ad3d4a52025106d13376d96';
  var url = 'http://api.openweathermap.org/data/2.5/weather' +
  '?lat=' + pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + apiKey;
  xhrRequest(url, 'GET', function(responseText) {
    var json = JSON.parse(responseText);
    var name = json.name;
    var temp_cel = Math.round(json.main.temp - 273.15);
    var temp_fah = Math.round((json.main.temp - 273.15) * 9 / 5 + 32);
    //console.log('Collected name: ' + name);
    //console.log('Collected icon: ' + icon);
    //console.log('Collected temp_cel: ' + temp_cel);
    //console.log('Collected temp_fah: ' + temp_fah);
    var dictionary = {
      'LOCATION': name,
      'TEMP_CEL': temp_cel,
      'TEMP_FAH': temp_fah
    };
    Pebble.sendAppMessage(dictionary);
  });
}

function locationError(err) {
  console.log('Error: Requesting Location!');
}

// get gps location
function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// listen for when the watchface is opened
Pebble.addEventListener('ready',
  function(e) {
    //console.log('PebbleKit JS ready!');
    getWeather();
  }
);
