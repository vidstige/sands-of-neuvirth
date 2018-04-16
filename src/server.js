const express = require('express');
const webpack = require('webpack');
const middleware = require('webpack-dev-middleware'); //webpack hot reloading middleware

var spawn = require('child_process').spawn;

const cfg = require('../webpack.config.js');
const compiler = webpack(cfg);

const app = express();

app.get('/api/sand', function (req, res) {
  res.header("Content-Type", "application/octet-stream");
  res.header('Content-Transfer-Encoding', 'binary');

  const child = spawn('solver/fluid');
  child.stdout.pipe(res);
  child.stderr.pipe(process.stdout);
});

app.use(middleware(compiler, {
  // webpack-dev-middleware options
}));

app.listen(3000, () => console.log('Example app listening on port 3000!'));
