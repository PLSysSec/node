'use strict';
var common = require('../common.js');
const v8 = require('v8');

var bench = common.createBenchmark(main, {});

function main(conf) {
  var N = 64 * 1024 * 1024;
  var b = Buffer.allocUnsafe(N);
  var s = '';
  var i;
  for (i = 0; i < 256; ++i) s += String.fromCharCode(i);
  for (i = 0; i < N; i += 256) b.write(s, i, 256, 'ascii');


  v8.setFlagsFromString('--allow_natives_syntax');
  b.toString('base64');
  b.toString('base64');
  eval('%OptimizeFunctionOnNextCall(b.toString)');
  b.toString('base64');	
  	
  bench.start();
  for (i = 0; i < 32; ++i) b.toString('base64');
  bench.end(64);
}
