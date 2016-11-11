var common = require('../common.js');
const binding = process.binding('buffer');

var bench = common.createBenchmark(main, {
  type: ['testNoOpOriginal(23)', 
         'testNoOpOriginalCorrected(23)',
         'testNoOpPromise(23)',
         'testNoOpOriginalCorrected_Get([23])',
         'testNoOpPromise_GetSlow([23])',
         'testNoOpPromise_GetFast([23])'],
  millions: [100]
});

function main(conf) {
  var len = +conf.millions * 1e6;
  var fn = conf.type;

  var testFunction = new Function('', [
    'for (var i = 0; i !== ' + len + '; i++) {',
    '  binding.' + fn + ';',
    '}'
  ].join('\n'));
  bench.start();
  testFunction(safeAPIBench);
  bench.end(len / 1e6);
}
