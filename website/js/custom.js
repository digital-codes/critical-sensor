
"use strict";

// slide shows
var wiesenIndex = 0;
var sensorIndex = 0;

const reloadIntervall = 10*60*1000
//const reloadIntervall = 10*1000
var latestDate = ""

// ---------------

/* ------- repeat ------ 
    load object e.g. via json like so
{
"wiesen":[
    {"image":"20200818"},
    {"image":"20200919"}
]
}

    object name (wiesen) corresponds to repeat name
    and object array item (image) corresponds to template name
    display is inserted into parent select


*/

function wiesenBilder(img) {
  console.log(img)
  w3.displayObject("wcaption", img);
  w3.displayObject("wfigure", img);
  wiesenShow(8000)
}

// -------------------



// init function
function init() {
    console.log("Init")
    //testScript("#chart")

    //carousel("wiese",wiesenIndex);
    carousel("sensor",sensorIndex);

    /* doesn't CALL the php in basic mode.... */
    /* seems to be a problem only when not using webserver: php and http-server
    ** don't execute the php as a script 
    else use xmlhttp ...
    */
    w3.getHttpObject("data/wiesen.json", wiesenBilder);

    w3.getHttpObject("srv.php", processData);

    setTimeout(reloadPage,reloadIntervall);

}

function reloadPage() {
    location.reload()
    setTimeout(reloadPage,reloadIntervall);
}


function processData(rawData) {
    //console.log("Start processing:",rawData)
    console.log("Start processing")
    latestDate = rawData[rawData.length - 1].date
    console.log("Latest: ",latestDate)
    w3.displayObject("latest", {"date":"Update: " + latestDate});
    //var charts = ["c1","c2","c3","c4","c5","c6","c7","c8","c9"]
    var charts = ["c1","c2","c3","c4","c5","c8","c9"]
    var lbls = ["CO2 [ppm]","Temp [°C]","Hum [%]","Pres [mb]","Light [%]","RSSI [dB]","Bat [%]"] // key must be lower case
    charts.forEach((function(k,i) { // not working with . atrribute
        console.log("Chart for ",k)
        displayData(rawData,"#"+k,lbls[i])
    }))

}



// test script, bind to selector sel
function displayData(rawData,sel,lbl) {

  var dataKey = lbl.split(" ")[0].toLowerCase() // split when using units

    console.log("Testing on ", sel," for ",dataKey)

    // multiple X values work wit h column data only
    // convert :::

    // sort by id
    var data = d3.nest()
            .key(function(d) { return d["id"]; })
            .object(rawData)
    // get keys
    var devKeys =  Object.keys(data)
    console.log("Decives: ",devKeys)

    // create columns !
    var colData = Array()
    var colXs = {}
    var colTypes = {}
    devKeys.forEach(function(item) {
        var xLbl = "x"+item
        var dLbl = "Sensor"+item
        var x = data[item].map(x => x.date)
        x.unshift(xLbl)
        colData.push(x) // push x columns
        var d = data[item].map(x => x[dataKey])
        d.unshift(dLbl)
        colData.push(d) // push d columns
        colXs[dLbl] = xLbl  // set descriptor
        colTypes[dLbl] = "step"  // set type
        //console.log(item," -> ",x)
        //console.log(d)
    })
    console.log("Desc: ",colXs)
    console.log("Types: ",colTypes)
    //console.log("Cols: ",colData)

    // maybe check https://stackoverflow.com/questions/55023252/c3js-multiple-xy-line-chart-with-json-data


  var chart = c3.generate({
    bindto: sel,
    data: {
        xFormat: '%Y-%m-%d %H:%M:%S',
        xs: colXs,
        columns: colData,
        types: colTypes
    },
    axis: {
      x: {
        type: 'timeseries',
        tick: {
          count: 4,
          format: "%Y-%m-%d %H"
        },
        label: {
          text: "Date",
          position: 'outer-center'
        }
      },
       
      y: {
        label: {
          text: lbl,
          position: 'middle'
        }
      }
    },
    legend: {
      position: "inset"
    },
    line: {
      step: {
        type: 'step'
      }
    },

  });

  setTimeout(chart.resize,1000);

}


function carousel(base,slideIndex) {
  var i;
  var x = document.getElementsByClassName(base);
  //console.log(base," items: ",x.length)
  for (i = 0; i < x.length; i++) {
    x[i].style.display = "none";
  }
  slideIndex++;
  if (slideIndex > x.length) {slideIndex = 1}
  x[slideIndex-1].style.display = "block";
  //setTimeout(carousel, 2000); // Change image every 2 seconds
  setTimeout(carousel.bind(null,base,slideIndex), 8000); // Change image every 2 seconds
}


// copy slideshow fpr wiese
var wiesenShow = function (ms) {
  var i, ss
  var x = w3.getElements(".wcaption"), l = x.length;
  var y = w3.getElements(".wimage"), k = y.length;
  if (l != k) {
    console.log("Wiesenshow: length mismatch")
    return;
  }
  console.log("Wiesensshow on ",x,y)
  ss = {};
  ss.current = 1;
  ss.x = x;
  ss.y = y;
  ss.ondisplaychange = null;
  if (!isNaN(ms) || ms == 0) {
    ss.milliseconds = ms;
  } else {
    ss.milliseconds = 1000;
  }
  ss.start = function() {
    ss.display(ss.current)
    if (ss.ondisplaychange) {ss.ondisplaychange();}
    if (ss.milliseconds > 0) {
      window.clearTimeout(ss.timeout);
      ss.timeout = window.setTimeout(ss.next, ss.milliseconds);
    }
  };
  ss.next = function() {
    ss.current += 1;
    if (ss.current > ss.x.length) {ss.current = 1;}
    ss.start();
  };
  ss.previous = function() {
    ss.current -= 1;
    if (ss.current < 1) {ss.current = ss.x.length;}
    ss.start();
  };
  ss.display = function (n) {
    w3.styleElements(ss.x, "display", "none");
    w3.removeClass(ss.x[n-1],'w3-hide')
    w3.styleElement(ss.x[n - 1], "display", "block");
    w3.styleElements(ss.y, "display", "none");
    w3.removeClass(ss.y[n-1],'w3-hide')
    w3.styleElement(ss.y[n - 1], "display", "block");
  }
  ss.start();
  return ss;
};


function w3_open() {
  document.getElementById("menu").style.width = "100%";
  document.getElementById("menu").style.display = "block";
}

function w3_close() {
  document.getElementById("menu").style.display = "none";
}

function toGerman() {
    w3.addClass('.lang-en','w3-hide')
    w3.removeClass('.lang-de','w3-hide')
/*
 <button onclick="w3.addClass('.lang-en','w3-hide')">Add Class</button> 
*/
}

function toEnglish() {
    w3.addClass('.lang-de','w3-hide')
    w3.removeClass('.lang-en','w3-hide')
}



