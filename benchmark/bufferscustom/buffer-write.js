'use strict';
var common = require('../common.js');
var bench = common.createBenchmark(main, {
  noAssert: ['true'],
  buffer: ['slow'],
  type: ['Int16LE',
         ],
  millions: [1]
});
const v8 = require('v8');

const INT8 = 0x7f;
const INT16 = 0x7fff;
const INT32 = 0x7fffffff;
const UINT8 = (INT8 * 2) + 1;
const UINT16 = (INT16 * 2) + 1;
const UINT32 = INT32;

var mod = {
  writeInt8: INT8,
  writeInt16BE: INT16,
  writeInt16LE: INT16,
  writeInt32BE: INT32,
  writeInt32LE: INT32,
  writeUInt8: UINT8,
  writeUInt16BE: UINT16,
  writeUInt16LE: UINT16,
  writeUInt32BE: UINT32,
  writeUInt32LE: UINT32
};

function main(conf) {
  var noAssert = conf.noAssert === 'true';
  var len = +conf.millions * 1e6;
  var clazz = conf.buf === 'fast' ? Buffer : require('buffer').SlowBuffer;
  var buff = new clazz(8);
  var fn = 'write' + conf.type;

  if (fn.match(/Int/))
    benchInt(buff, fn, len, noAssert);
  else
    benchFloat(buff, fn, len, noAssert);
}

function benchInt(buff, fn, len, noAssert) {
  var m = mod[fn];
  var testFunction = new Function('buff', [
    'for (var i = 0; i !== ' + len + '; i++) {',
    '  buff.' + fn + '(i & ' + m + ', 0, ' + JSON.stringify(noAssert) + ');',
    '}'
  ].join('\n'));
  bench.start();
  testFunction(buff);
  bench.end(len / 1e6);
}

function benchFloat(buff, fn, len, noAssert) {
  var testFunction = new Function('buff', [
    'for (var i = 0; i !== ' + len + '; i++) {',
    '  buff.' + fn + '(i, 0, ' + JSON.stringify(noAssert) + ');',
    '}'
  ].join('\n'));


  // Force optimization before starting the benchmark
  testFunction();
  testFunction();
  v8.setFlagsFromString('--allow_natives_syntax');
  eval('%OptimizeFunctionOnNextCall(testFunction)');
  testFunction();

  bench.start();
  testFunction(buff);
  bench.end(len / 1e6);
}
