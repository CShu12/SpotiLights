/**
 * This is an example of a basic node.js script that performs
 * the Authorization Code oAuth2 flow to authenticate against
 * the Spotify Accounts.
 *
 * For more information, read
 * https://developer.spotify.com/web-api/authorization-guide/#authorization_code_flow
 */

var express = require('express'); // Express web server framework
var request = require('request'); // "Request" library
var cors = require('cors');
var querystring = require('querystring');
var cookieParser = require('cookie-parser');

var client_id = '665f1d1045ff46adb54d997d68eb57b1'; // Your client id
var client_secret = 'a6f038ba1915487a927d23e0f97afdcd'; // Your secret
var redirect_uri = 'http://localhost:8888/callback'; // Your redirect uri

var danceability = -1;
var energy = -1;
var valence = -1;
var tempo = -1;
var durations = [];

/**
 * Generates a random string containing numbers and letters
 * @param  {number} length The length of the string
 * @return {string} The generated string
 */
var generateRandomString = function(length) {
  var text = '';
  var possible = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';

  for (var i = 0; i < length; i++) {
    text += possible.charAt(Math.floor(Math.random() * possible.length));
  }
  return text;
};

var stateKey = 'spotify_auth_state';

var app = express();

app.use(express.static(__dirname + '/public'))
   .use(cors())
   .use(cookieParser());

app.get('/login', function(req, res) {

  var state = generateRandomString(16);
  res.cookie(stateKey, state);

  // your application requests authorization
  var scope = 'user-read-currently-playing';
  res.redirect('https://accounts.spotify.com/authorize?' +
    querystring.stringify({
      response_type: 'code',
      client_id: client_id,
      scope: scope,
      redirect_uri: redirect_uri,
      state: state
    }));
});

app.get('/callback', function(req, res) {

  // your application requests refresh and access tokens
  // after checking the state parameter

  var code = req.query.code || null;
  var state = req.query.state || null;
  var storedState = req.cookies ? req.cookies[stateKey] : null;

  if (state === null || state !== storedState) {
    res.redirect('/#' +
      querystring.stringify({
        error: 'state_mismatch'
      }));
  } else {
    res.clearCookie(stateKey);
    var authOptions = {
      url: 'https://accounts.spotify.com/api/token',
      form: {
        code: code,
        redirect_uri: redirect_uri,
        grant_type: 'authorization_code'
      },
      headers: {
        'Authorization': 'Basic ' + (new Buffer(client_id + ':' + client_secret).toString('base64'))
      },
      json: true
    };

    request.post(authOptions, function(error, response, body) {
      if (!error && response.statusCode === 200) {

        var access_token = body.access_token,
        refresh_token = body.refresh_token;

        var options = {
          url: 'https://api.spotify.com/v1/me',
          headers: { 'Authorization': 'Bearer ' + access_token },
          json: true
        };

        // use the access token to access the Spotify Web API
        request.get(options, function(error, response, body) {
          console.log(body);
        });

        // we can also pass the token to the browser to make requests from there
        res.redirect('/#' +
          querystring.stringify({
            access_token: access_token,
            refresh_token: refresh_token
          }));
      } else {
        res.redirect('/#' +
          querystring.stringify({
            error: 'invalid_token'
          }));
      }
    });
  }
});

app.get('/refresh_token', function(req, res) {

  // requesting access token from refresh token
  var refresh_token = req.query.refresh_token;
  var authOptions = {
    url: 'https://accounts.spotify.com/api/token',
    headers: { 'Authorization': 'Basic ' + (new Buffer(client_id + ':' + client_secret).toString('base64')) },
    form: {
      grant_type: 'refresh_token',
      refresh_token: refresh_token
    },
    json: true
  };

  request.post(authOptions, function(error, response, body) {
    if (!error && response.statusCode === 200) {
      var access_token = body.access_token;
      res.send({
        'access_token': access_token
      });
    }
  });
});

app.get('/get_ID', function(req, res) {
    var access_token = req.query.access_token;
    var options = {
        url: 'https://api.spotify.com/v1/me/player/currently-playing',
        headers: { 'Accept': 'application/json', 'Content-Type': 'application/json', 'Authorization': 'Bearer ' + access_token },
        json: true
    };
    var track_ID = -1;
    request.get(options, function(error, response, body) {
        code = response.statusCode;
        if (!error) {
            if (code === 200) {
                res.redirect('/get_features?' + querystring.stringify({
                    access_token: access_token,
                    id: body.item.id
                }));
            }
            else if (code === 204) {
                res.write('No track playing');
                res.end();
            }
            else {
                res.write("ID Status Code: " + code.toString());
                res.end();
            }
        }
    });
});

app.get('/get_features', function(req, res) {
    var access_token = req.query.access_token;
    var track_ID = req.query.id;
    options = {
        url: 'https://api.spotify.com/v1/audio-features/' + track_ID,
        headers: { 'Accept': 'application/json', 'Content-Type': 'application/json', 'Authorization': 'Bearer ' + access_token },
        json: true
    };
    request.get(options, function(error, response, body) {
        code = response.statusCode;
        if (!error) {
            if (code === 200) {
                danceability = body.danceability;
                energy = body.energy;
                valence = body.valence;
                tempo = body.tempo;
                res.redirect('/get_beats?' + querystring.stringify({
                    access_token: access_token,
                    id: track_ID,
                    danceability: body.danceability,
                    energy: body.energy,
                    valence: body.valence,
                    tempo: body.tempo
                }));
            }
            else {
                res.write("Features Status Code: " + code.toString());
                res.end();
            }
        }
    });
});

app.get('/get_beats', function(req, res) {
    var access_token = req.query.access_token;
    var track_ID = req.query.id;
    options = {
        url: 'https://api.spotify.com/v1/audio-analysis/' + track_ID,
        headers: { 'Accept': 'application/json', 'Content-Type': 'application/json', 'Authorization': 'Bearer ' + access_token },
        json: true
    };
    request.get(options, function(error, response, body) {
        code = response.statusCode;
        if (!error) {
            if (code === 200) {
                durations = body.beats;
                for (i in durations) {
                    durations[i] = durations[i].duration;
                }
                res.write(JSON.stringify({
                    'danceability' : req.query.danceability,
                    'energy' : req.query.energy,
                    'valence' : req.query.valence,
                    'tempo' : req.query.tempo,
                    'beats' : durations
                }));
                res.end();
            }
            else {
                res.write("Features Status Code: " + code.toString());
                res.end();
            }
        }
    });
});

app.get('/get_data', function(req, res) {
    res.writeHead(200,{ 'Content-Type': 'application/json'});
    res.write(JSON.stringify({
        'danceability' : danceability,
        'energy' : energy,
        'valence' : valence,
        'tempo' : tempo,
        'beats' : durations
    }));
    res.end();
});

console.log('Listening on 8888');
app.listen(8888);
