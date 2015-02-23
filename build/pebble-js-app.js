var values = 0;
var config_url = "http://edwinfinch.github.io/config-hi";
var options = JSON.parse(localStorage.getItem('hi_ops'));

Pebble.addEventListener("showConfiguration", function(e) {
    if (localStorage.getItem("hi_ops") !== null) {
        options = JSON.parse(localStorage.hi_ops);
    }
        Pebble.openURL(config_url);
        console.log("Opening " + config_url);
});

Pebble.addEventListener("webviewclosed", function (e) {
    console.log("Configuration closed");
    console.log("Response = " + e.response.length + "   " + e.response);
    if (e.response) { // user clicked Save/Submit, not Cancel/Done
        console.log("User hit save");
        values = JSON.parse(decodeURIComponent(e.response));
        console.log("stringified options: " + JSON.stringify((values)));

        if(values.customLoc){
          localStorage.setItem("customLoc", values.customLoc.replace(" ", ""));
          console.log("Set customLoc to: " + values.customLoc.replace(" ", ""));
        }
        if(values.customLoc === ""){
          localStorage.setItem("customLoc", "noLOC");
          console.log("Reset customLoc");
        }
        
        Pebble.sendAppMessage(
            values
        );
    }
});

var fetched = 0;
var values = 0;

function iconFromWeatherId(weatherId) {
  if (weatherId > 199 && weatherId < 233) {
    return 0; //Thunderstorm
  } 
    else if (weatherId > 299 && weatherId < 322) {
    return 1; //Drizzle
  } 
    else if (weatherId > 499 && weatherId < 523) {
    return 2; //Rain
  } 
    else if (weatherId > 599 && weatherId < 622) {
    return 3; //Snow
  } 
    else if (weatherId > 700 && weatherId < 742) {
    return 4; //Haze or fog
  }
    else if (weatherId == 800){
    return 5; //Clear
  }
    else if (weatherId == 801){
    return 6; //Few clouds
  }
    else if (weatherId > 801){
    return 7; //Scattered, broken, or overcast clouds
  }
}

function fetchWeather(toattach) {
  var response;
  var req = new XMLHttpRequest();
  var address = "http://api.openweathermap.org/data/2.5/weather?" + toattach;
  req.open('GET', address, true);
  console.log("Getting new weather data from: " + address);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log("Response success. Result: \n" + req.responseText);
        response = JSON.parse(req.responseText);
    var temperature, condition;
      var weatherResultList = response.weather[0];
      temperature = response.main.temp;
      condition = iconFromWeatherId(weatherResultList.id);

          Pebble.sendAppMessage({
            "icon":condition,
            "temp":temperature,
      });
      } else {
          console.log("Error: could not connect! (is api.openweathermap.com down?)");
      }
    }
  };
  req.send();
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
    if(fetched === 0){
        console.log("Location success! Fetching weather...");
        fetchWeather(coordinates.latitude, coordinates.longitude);
        fetched = 1;
    }
}

function locationError(err) {
  console.warn('Location error (' + err.code + '): ' + err.message);
    var temperatureError = parseInt(err.code) + 400;
    var workingLatitude = localStorage.getItem("latitude1");
    var workingLongitude = localStorage.getItem("longitude1");

    fetchWeather(workingLatitude, workingLongitude);
    console.log("Fetching previous working temperature from latitude: " + workingLatitude + " and longitude: " + workingLongitude);
    console.log("Because of error: " + temperatureError);
}

var locationOptions = { "timeout": 10000, "maximumAge": 30000 }; 
var fetched = 0;

function formatLatAndLong(lat, lon){
  var toReturn = "lat=" + lat + "&lon=" + lon;
  return toReturn;
}

function formatCustomLoc(customLoc){
  var toReturn = "q=" + customLoc;
  return toReturn;
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  if(fetched === 0){
    fetchWeather(formatLatAndLong(coordinates.latitude, coordinates.longitude));
    fetched = 1;
  }
}

function get_weather(){
    fetched = 0;
    var customLoc = localStorage.getItem("customLoc");
    if(customLoc === "noLOC" || customLoc === null || customLoc === ""){
      locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
    }
    else{
      fetchWeather(formatCustomLoc(customLoc));
    }
}

Pebble.addEventListener("ready",
                        function(e) {
                          console.log("connect!" + e.ready);
                            
                          get_weather();
                            
                          console.log(e.type);
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                            console.log(e.type);
                            console.log("message!");
                            get_weather();
                        });
