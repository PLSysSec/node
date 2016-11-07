const binding = process.binding('buffer');
const n = 1e8;
var Benchmark = require('benchmark');
var suite = new Benchmark.Suite;

function testNoOpOriginal()
{
	for (var i = 0; i <= n; i++){
	  var ret = binding.testNoOpOriginal(23);
	}
}

function testNoOpOriginalCorrected()
{
	for (var i = 0; i <= n; i++){
	  var ret = binding.testNoOpOriginalCorrected(23);
	}
}

function testNoOpPromise()
{
	for (var i = 0; i <= n; i++){
	  var ret = binding.testNoOpPromise(23);
	}
}

var ret = suite.add("testNoOpOriginal", testNoOpOriginal).
add("testNoOpOriginalCorrected", testNoOpOriginalCorrected).
add("testNoOpPromise", testNoOpPromise).
run({ 'minSamples': 10 });

for(var i = 0; i <= 2; i++){
	var str = ret[i].name + "\n"
	 	+ "Mean: " + ret[i].stats.mean + " +- " + ret[i].stats.rme + "%" + "\n"
	 	+ "Deviation: " + ret[i].stats.deviation + "\n"
	 	+ "Time Period: " + ret[i].times.period + " sec" + "\n"
	;

	console.info(str);
}