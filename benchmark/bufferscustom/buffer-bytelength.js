'use strict';
var common = require('../common');
const v8 = require('v8');
const buffer = process.binding('buffer');

var bench = common.createBenchmark(main, {
  encoding: ['utf8'],
  len: [64], // x16
  n: [5e6]
});

// 16 chars each
var chars = [
  'hello brendan!!!'//, // 1 byte
  // 'ΰαβγδεζηθικλμνξο'//, // 2 bytes
  // '挰挱挲挳挴挵挶挷挸挹挺挻挼挽挾挿'//, // 3 bytes
  // '𠜎𠜱𠝹𠱓𠱸𠲖𠳏𠳕𠴕𠵼𠵿𠸎𠸏𠹷𠺝𠺢' // 4 bytes
];

function main(conf) {
  var n = conf.n | 0;
  var encoding = conf.encoding;

  var str = buildString('hello brendan!!!', 64);

  v8.setFlagsFromString('--allow_natives_syntax');
  buffer.byteLengthUtf8(str, encoding);
  buffer.byteLengthUtf8(str, encoding);
  eval('%OptimizeFunctionOnNextCall(buffer.byteLengthUtf8)');
  buffer.byteLengthUtf8(str, encoding); 

  bench.start();
  for (var i = 0; i < n; i++) {
    var r = buffer.byteLengthUtf8(str, encoding);
  }
  bench.end(n);
}

function buildString(str, times) {
  if (times === 1) return str;

  return str + buildString(str, times - 1);
}
