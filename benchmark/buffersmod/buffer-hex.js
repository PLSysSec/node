'use strict';

const common = require('../common.js');
const v8 = require('v8');

const bench = common.createBenchmark(main, {
  len: [0, 1, 64, 1024],
  n: [1e7]
});

function main(conf) {
  const len = conf.len | 0;
  const n = conf.n | 0;
  const buf = Buffer.alloc(len);

  for (let i = 0; i < buf.length; i++)
    buf[i] = i & 0xff;

  const hex = buf.toString('hex');

  v8.setFlagsFromString('--allow_natives_syntax');
  Buffer.from(hex, 'hex');
  Buffer.from(hex, 'hex');
  eval('%OptimizeFunctionOnNextCall(Buffer.from)');
  Buffer.from(hex, 'hex'); 

  bench.start();

  for (let i = 0; i < n; i += 1)
    Buffer.from(hex, 'hex');

  bench.end(n);
}
