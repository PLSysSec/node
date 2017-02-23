'use strict';
const assert = require('assert');
const common = require('../common.js');
const v8 = require('v8');

const bench = common.createBenchmark(main, {});

function main(conf) {
  const s = 'abcd'.repeat(8 << 20);
  s.match(/./);  // Flatten string.
  assert.equal(s.length % 4, 0);
  const b = Buffer.allocUnsafe(s.length / 4 * 3);
  b.write(s, 0, s.length, 'base64');

  v8.setFlagsFromString('--allow_natives_syntax');
  b.base64Write(s, 0, s.length);
  b.base64Write(s, 0, s.length);
  eval('%OptimizeFunctionOnNextCall(b.base64Write)');
  b.base64Write(s, 0, s.length);	

  bench.start();
  for (var i = 0; i < 32; i += 1) b.base64Write(s, 0, s.length);
  bench.end(32);
}
