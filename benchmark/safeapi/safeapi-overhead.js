var common = require('../common.js');
const binding = process.binding('buffer');
const v8 = require('v8');

var bench = common.createBenchmark(main, {
  type: [//'testNoOpOriginal',
         'testNoOpOriginalCorrected',
	 'testNoOpPromise'],
  millions: [100]
});

function main(conf) {
  var len = +conf.millions * 1e6;
  var fn = conf.type;

  var testFunction = binding[fn];

  v8.setFlagsFromString('--allow_natives_syntax');
  testFunction(23);
  testFunction(23);
  eval('%OptimizeFunctionOnNextCall(testFunction)');
  //eval('%NeverOptimizeFunction(testFunction)');
  testFunction(23);

  bench.start();
  for (var i = 0; i !== len; i++) {
    testFunction(23);
  }
  bench.end(len / 1e6);
}
