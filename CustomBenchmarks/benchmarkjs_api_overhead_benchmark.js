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
on('start', function(){
	process.stdout.write("Running... ");
}).
on('cycle', function(){
	process.stdout.write("... ");
}).
on('complete', function(arg){
	console.info("Done");

	for(var i = 0; i <= 2; i++){
		var str = arg.currentTarget[i].name + "\n"
		 	+ "Mean: " + arg.currentTarget[i].stats.mean + " +- " + arg.currentTarget[i].stats.rme + "%" + "\n"
		 	+ "Deviation: " + arg.currentTarget[i].stats.deviation + "\n"
		;
		console.info(str);
	}
}).
run({
	'async' : true,
	'minSamples': 10
});
