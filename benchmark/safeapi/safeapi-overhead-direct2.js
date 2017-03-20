const binding = process.binding('buffer');
const v8 = require('v8');

function main() {
  var len = 200 * 1e6;

  var testFunction = binding['testNoOpPromise'];

  v8.setFlagsFromString('--allow_natives_syntax');
  binding.testNoOpPromise(23);
  binding.testNoOpPromise(23);
  eval('%OptimizeFunctionOnNextCall(binding.testNoOpPromise)');
  binding.testNoOpPromise(23);

  {
    var hrstart = process.hrtime();
    for (var i = 0; i !== len; i++) {
      binding.testNoOpPromise(23);
    }
    var hrend = process.hrtime(hrstart);
    console.info("testNoOpPromise Execution time (hr): %ds %dms", hrend[0], hrend[1]/1000000);
  }
}

main();
