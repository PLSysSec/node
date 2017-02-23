'use strict';

const common = require('../common.js');
const v8 = require('v8');

const bench = common.createBenchmark(main, {
  n: [1024],
  type: ['buffer', 'string']
});

const zeroBuffer = Buffer.alloc(0);
const zeroString = '';

function main(conf) {
  var n = +conf.n;

  v8.setFlagsFromString('--allow_natives_syntax');
  Buffer.from(zeroBuffer);
  Buffer.from(zeroBuffer);
  eval('%OptimizeFunctionOnNextCall(Buffer.from)');
  Buffer.from(zeroBuffer); 
  
  bench.start();

  if (conf.type === 'buffer')
    for (let i = 0; i < n * 1024; i++) Buffer.from(zeroBuffer);
  else if (conf.type === 'string')
    for (let i = 0; i < n * 1024; i++) Buffer.from(zeroString);

  bench.end(n);
}
