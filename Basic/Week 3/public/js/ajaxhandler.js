function formatParams( params ){
    return "?" + Object
        .keys(params)
        .map(function(key){
            return key+"="+encodeURIComponent(params[key])
        })
        .join("&");
  }

function httpGetAsync(theUrl, params={}, callback, args={})
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200 && callback) {
            callback(xmlHttp.responseText, args);
        }
    }
    xmlHttp.open("GET", theUrl + formatParams(params), true); // true for asynchronous 
    xmlHttp.send(null);
}

function httpPostAsync(theUrl, params={}, callback, args={})
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("POST", theUrl, true); // true for asynchronous 
    // Send the proper header information along with the request
    xmlHttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200 && callback) {
            callback(xmlHttp.responseText, args);
        }
    }
    xmlHttp.send(formatParams(params).substr(1));
}

function httpGetBlocking(theUrl, params={}, callback, args={})
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200 && callback) {
            callback(xmlHttp.responseText, args);
        }
    }
    xmlHttp.open("GET", theUrl + formatParams(params), false); // true for asynchronous 
    xmlHttp.send(null);
}