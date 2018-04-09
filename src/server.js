const express = require('express');
const webpack = require('webpack');
const middleware = require('webpack-dev-middleware'); //webpack hot reloading middleware

var spawn = require('child_process').spawn;

const cfg = require('../webpack.config.js');
const compiler = webpack(cfg);

const app = express();

app.get('/api/sand', function (req, res) {
  //spawn('cat', ['/dev/urandom']).stdout.pipe(res);
  //res.send('hi');
  setInterval(function() {
    res.write('1234567890');
  }, 250);
});

app.use(middleware(compiler, {
  // webpack-dev-middleware options
}));

app.listen(3000, () => console.log('Example app listening on port 3000!'));
