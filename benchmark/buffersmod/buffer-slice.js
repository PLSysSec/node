'use strict';
var common = require('../common.js');
var SlowBuffer = require('buffer').SlowBuffer;
const v8 = require('v8');

var bench = common.createBenchmark(main, {
  type: ['fast', 'slow'],
  n: [1024]
});

var buf = Buffer.allocUnsafe(1024);
var slowBuf = new SlowBuffer(1024);

function main(conf) {
  var n = +conf.n;
  var b = conf.type === 'fast' ? buf : slowBuf;

  v8.setFlagsFromString('--allow_natives_syntax');
  b.slice(10, 256);
  b.slice(10, 256);
  eval('%OptimizeFunctionOnNextCall(b.slice)');
  b.slice(10, 256); 


  bench.start();
  for (var i = 0; i < n * 1024; i++) {
    b.slice(10, 256);
  }
  bench.end(n);
}
