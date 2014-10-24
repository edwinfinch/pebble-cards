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
        for(var key in values) {
            localStorage.setItem(key, values[key]);
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

function fetchWeather(latitude, longitude) {
  var response;
  var req = new XMLHttpRequest();
  req.open('GET', "http://api.openweathermap.org/data/2.5/weather?" + "lat=" + latitude + "&lon=" + longitude, true);

  console.log("http://api.openweathermap.org/data/2.5/weather?" + "lat=" + latitude + "&lon=" + longitude);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        console.log("Response: " + response);
        var temperature, icon;
            var weatherResultList = response.weather[0];
            temperature = response.main.temp;
            icon = iconFromWeatherId(weatherResultList.id);
            console.log("It is " + temperature + " degrees");
            console.log("Icon resource loaded: " + icon);
            //Incase of future location error, save the working address, longitude and latitude
            localStorage.setItem("latitude1", latitude);
            localStorage.setItem("longitude1", longitude);
            console.log("Working latitude: " + localStorage.getItem("latitude1"));
            console.log("Working longitude: " + localStorage.getItem("longitude1"));

          Pebble.sendAppMessage({
            "icon":icon,
            "temp":temperature,
            });
        //}
      } else {
            console.log("Error: could not connect! (is api.openweathermap.com down?)");
      }
    }
  };
  req.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
    if(fetched === 0){
        console.log("Location success! Fetching weather...");
        fetchWeather(coordinates.latitude, coordinates.longitude);
        fetched = 1;
    }
    else if(fetched == 1){
        return;
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

var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 
var long_s = 0;
var lat_s = 0;
var fetchtype = 0;
var d = new Date();
var n = d.getDate();

function get_weather(){
    locationWatcher = window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
}

Pebble.addEventListener("ready",
                        function(e) {
                          console.log("connect!" + e.ready);
                            console.log("DATE IS: " + n);
                            long_s = parseFloat(localStorage.getItem("long_s"));
                            lat_s = parseFloat(localStorage.getItem("lat_s"));
                            fetchtype = parseInt(localStorage.getItem("fetchtype"));
                            console.log("Got values: " + long_s + " and " + lat_s + " and " + fetchtype);
                            if(fetchtype === 1){
                                console.log("Fetching weather with specified coordinates: " + long_s + " and " + lat_s);
                                fetchWeather(lat_s, long_s);
                            }
                            else{
                                console.log("Specified coordinates is false...");
                                if(parseInt(localStorage.getItem("day_fetched")) !== n){
                                    console.log("Fetching with GPS location.");
                                    locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);  
                                }
                                else{
                                    console.log("Fetching from previous stored longitude and latitude");
                                    var lat_l = parseInt(localStorage.getItem("latitude1"));
                                    var long_l = parseInt(localStorage.getItem("longitude1"));
                                    fetchWeather(lat_l, long_l);
                                }
                            }
                            localStorage.setItem("day_fetched", n);
                          console.log(e.type);
                          setInterval(function() {
                            getWeather();
                          }, 1800000);
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                            console.log(e.type);
                            console.log(e.payload.temperature);
                            console.log("message!");
                        });
