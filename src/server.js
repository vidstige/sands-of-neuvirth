const express = require('express');
const webpack = require('webpack');
const middleware = require('webpack-dev-middleware'); //webpack hot reloading middleware

const cfg = require('../webpack.config.js');
const compiler = webpack(cfg);

const app = express();

app.get('/hello', function (req, res) {
  res.send('Hello World!');
});

app.use(middleware(compiler, {
  // webpack-dev-middleware options
}));

app.listen(3000, () => console.log('Example app listening on port 3000!'))