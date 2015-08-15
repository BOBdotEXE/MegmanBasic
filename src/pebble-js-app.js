
  Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('https://bobdotexe.github.io/watchfaces/megaman_basic_conf.html');
});

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
 
    //Send to Pebble, persist there
    Pebble.sendAppMessage(
      {"speed": configuration.speed},
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!");
      }
    );
  }
);