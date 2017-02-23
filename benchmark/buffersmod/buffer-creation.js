'use strict';
const SlowBuffer = require('buffer').SlowBuffer;
const v8 = require('v8');

const common = require('../common.js');
const assert = require('assert');
const bench = common.createBenchmark(main, {
  type: [
    'fast-alloc',
    'fast-alloc-fill',
    'fast-allocUnsafe',
    'slow-allocUnsafe',
    'slow',
    'buffer()'],
  len: [10, 1024, 2048, 4096, 8192],
  n: [1024]
});

function main(conf) {
  const len = +conf.len;
  const n = +conf.n;

  v8.setFlagsFromString('--allow_natives_syntax');

  Buffer.alloc(len);
  Buffer.alloc(len);
  eval('%OptimizeFunctionOnNextCall(Buffer.alloc)');
  Buffer.alloc(len); 

  Buffer.allocUnsafe(len);
  Buffer.allocUnsafe(len);
  eval('%OptimizeFunctionOnNextCall(Buffer.allocUnsafe)');
  Buffer.allocUnsafe(len)

  Buffer.allocUnsafeSlow(len);
  Buffer.allocUnsafeSlow(len);
  eval('%OptimizeFunctionOnNextCall(Buffer.allocUnsafeSlow)');
  Buffer.allocUnsafeSlow(len);

  SlowBuffer(len);
  SlowBuffer(len);
  eval('%OptimizeFunctionOnNextCall(SlowBuffer)');
  SlowBuffer(len);

  Buffer(len);
  Buffer(len);
  eval('%OptimizeFunctionOnNextCall(Buffer)');
  Buffer(len); 


  switch (conf.type) {
    case 'fast-alloc':
      bench.start();
      for (let i = 0; i < n * 1024; i++) {
        Buffer.alloc(len);
      }
      bench.end(n);
      break;
    case 'fast-alloc-fill':
      bench.start();
      for (let i = 0; i < n * 1024; i++) {
        Buffer.alloc(len, 0);
      }
      bench.end(n);
      break;
    case 'fast-allocUnsafe':
      bench.start();
      for (let i = 0; i < n * 1024; i++) {
        Buffer.allocUnsafe(len);
      }
      bench.end(n);
      break;
    case 'slow-allocUnsafe':
      bench.start();
      for (let i = 0; i < n * 1024; i++) {
        Buffer.allocUnsafeSlow(len);
      }
      bench.end(n);
      break;
    case 'slow':
      bench.start();
      for (let i = 0; i < n * 1024; i++) {
        SlowBuffer(len);
      }
      bench.end(n);
      break;
    case 'buffer()':
      bench.start();
      for (let i = 0; i < n * 1024; i++) {
        Buffer(len);
      }
      bench.end(n);
      break;
    default:
      assert.fail(null, null, 'Should not get here');
  }
}
