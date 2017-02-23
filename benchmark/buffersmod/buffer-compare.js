'use strict';
var common = require('../common.js');
const v8 = require('v8');

var bench = common.createBenchmark(main, {
  size: [16, 512, 1024, 4096, 16386],
  millions: [1]
});

function main(conf) {
  const iter = (conf.millions >>> 0) * 1e6;
  const size = (conf.size >>> 0);
  const b0 = Buffer.alloc(size, 'a');
  const b1 = Buffer.alloc(size, 'a');

  b1[size - 1] = 'b'.charCodeAt(0);


  v8.setFlagsFromString('--allow_natives_syntax');
  Buffer.compare(b0, b1);
  Buffer.compare(b0, b1);
  eval('%OptimizeFunctionOnNextCall(Buffer.compare)');
  Buffer.compare(b0, b1);  

  bench.start();
  for (var i = 0; i < iter; i++) {
    Buffer.compare(b0, b1);
  }
  bench.end(iter / 1e6);
}
