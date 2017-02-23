'use strict';

const common = require('../common.js');
const v8 = require('v8');

const bench = common.createBenchmark(main, {
  arg: ['true', 'false'],
  len: [0, 1, 64, 1024],
  n: [1e7]
});

function main(conf) {
  const arg = conf.arg === 'true';
  const len = conf.len | 0;
  const n = conf.n | 0;
  const buf = Buffer.alloc(len, 42);


  v8.setFlagsFromString('--allow_natives_syntax');
  buf.toString('utf8');
  buf.toString('utf8');
  eval('%OptimizeFunctionOnNextCall(buf.toString)');
  buf.toString('utf8'); 

  var i;
  bench.start();
  if (arg) {
    for (i = 0; i < n; i += 1)
      buf.toString('utf8');
  } else {
    for (i = 0; i < n; i += 1)
      buf.toString();
  }
  bench.end(n);
}
