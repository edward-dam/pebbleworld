// Author: Ed Dam

// pebblejs
require('pebblejs');

// clayjs
var Clay       = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

// libraries
var UI       = require('pebblejs/ui');
var Vector2  = require('pebblejs/lib/vector2');
var ajax     = require('pebblejs/lib/ajax');
var Settings = require('pebblejs/settings');

// collect api data
var gpsLatitude;
var gpsLongitude;
var apiKey = '42efb17b4ad3d4a52025106d13376d96';
//console.log('Saved apidata: ' + Settings.data('worldapi'));

// collect options
var time = "%H:%M";
var date = "%A %b %d";
var options = JSON.parse(localStorage.getItem('clay-settings'));
if ( options !== null) {
  console.log('Loaded hours_24 option: ' + options.hours_24);
  console.log('Loaded date_order option: ' + options.date_order);
  console.log('Loaded temp_degrees option: ' + options.temp_degrees);
  if ( options.hours_24 === "12hours" ) {
    time = "%I:%M";
  }
  if ( options.date_order === "day-month" ) {
    date = "%A %d %b";
  }
}


// definitions
var window = new UI.Window();
var windowSize = window.size();
var size = new Vector2(windowSize.x, windowSize.y);
var backgroundColor = 'black';
var highlightBackgroundColor = 'white';
var textColor = 'white';
var highlightTextColor = 'black';
var textAlign = 'center';
var fontXLarge = 'leco-42-numbers';
//var fontLarge = 'gothic-28-bold';
//var fontMedium = 'gothic-24-bold';
var fontSmall = 'gothic-18-bold';
var fontXSmall = 'gothic-14-bold';
function position(height){
  return new Vector2(0, windowSize.y / 2 + height);
}

// main screen
collectgpslocation(function() {
  collectweatherdata(function(){
    var apidata = Settings.data('worldapi');
    var location = apidata.name;
    var temperature = Math.round((apidata.main.temp - 273.15) * 9 / 5 + 32) + '°F';
    if ( options !== null) {
      if ( options.temp_degrees === "celsius" ) {
        temperature = Math.round(apidata.main.temp - 273.15) + '°C';
      }
    }
    var mainWind = new UI.Window();
    var mainLocation = new UI.Text({size: size, backgroundColor: backgroundColor, textAlign: textAlign});
    var mainImage = new UI.Image({size: size});
    var mainTime = new UI.TimeText({size: size, textAlign: textAlign,
      color: highlightTextColor, backgroundColor: highlightBackgroundColor
    });
    var mainDate = new UI.TimeText({size: size, backgroundColor: backgroundColor, textAlign: textAlign});
    mainLocation.position(position(-86));
    mainImage.position(position(-111));
    mainTime.position(position(11));
    mainDate.position(position(66));
    mainLocation.font(fontSmall);
    mainTime.font(fontXLarge);
    mainDate.font(fontXSmall);
    //mainLocation.text(apidata.name);
    mainLocation.text(location + ' ' + temperature);
    mainImage.image('images/splash.png');
    mainTime.text(time);
    mainDate.text(date);
    mainWind.add(mainLocation);
    mainWind.add(mainImage);
    mainWind.add(mainTime);
    mainWind.add(mainDate);
    mainWind.show();
  });
});

// functions

function collectgpslocation(callback) {
  navigator.geolocation.getCurrentPosition(function(api) {
    //console.log('Collected gpsLocation: ' + api.coords);
    gpsLatitude = api.coords.latitude;
    gpsLongitude = api.coords.longitude;
    //console.log('Latitude gpsLocation: ' + gpsLatitude);
    //console.log('Longitude gpsLocation: ' + gpsLongitude);
    callback();
  });
}

function collectweatherdata(callback) {
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
  gpsLatitude + '&lon=' + gpsLongitude + '&appid=' + apiKey;
  //console.log('url: ' + url);
  ajax({ url: url, method: 'get', type: 'json' },
    function(api){
      console.log('Collected apidata: ' + api);
      Settings.data('worldapi', api);
      callback();
    }
  );
}
