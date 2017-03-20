const binding = process.binding('buffer');
const v8 = require('v8');

function main() {
  var len = 200 * 1e6;

  var testFunction = binding['testNoOpOriginalCorrected'];

  v8.setFlagsFromString('--allow_natives_syntax');
  binding.testNoOpOriginalCorrected(23);
  binding.testNoOpOriginalCorrected(23);
  eval('%OptimizeFunctionOnNextCall(binding.testNoOpOriginalCorrected)');
  binding.testNoOpOriginalCorrected(23);

  {
    var hrstart = process.hrtime();
    for (var i = 0; i !== len; i++) {
      binding.testNoOpOriginalCorrected(23);
    }
    var hrend = process.hrtime(hrstart);
    console.info("testNoOpOriginalCorrected Execution time (hr): %ds %dms", hrend[0], hrend[1]/1000000);
  }
}


main();
