const binding = process.binding('buffer');
const v8 = require('v8');

function main(conf) {
  var n = 5e6;
  var str = buildString("hello brendan!!!", 64);

  v8.setFlagsFromString('--allow_natives_syntax');
  binding.byteLengthUtf8(str);
  binding.byteLengthUtf8(str);
  eval('%OptimizeFunctionOnNextCall(binding.byteLengthUtf8)');
  binding.byteLengthUtf8(str); 

  var hrstart = process.hrtime();
  for (var i = 0; i < n; i++) {
    var r = binding.byteLengthUtf8(str);
  }
  var hrend = process.hrtime(hrstart);

  console.info("Execution time (hr): %ds %dms", hrend[0], hrend[1]/1000000);
}

function buildString(str, times) {
  if (times === 1) return str;

  return str + buildString(str, times - 1);
}

main();