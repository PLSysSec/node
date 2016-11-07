const binding = process.binding('buffer');
const n = 1e8;

function TestOrder1()
{
	const a1 = process.hrtime();
	for (var i = 0; i <= n; i++){
	  binding.testNoOpOriginal(23);
	}
	const b1 = process.hrtime(a1);


	const c1 = process.hrtime();
	for (var i = 0; i <= n; i++){
	  binding.testNoOpOriginalCorrected(23);
	}
	const d1 = process.hrtime(c1);


	const e1 = process.hrtime();
	for (var i = 0; i <= n; i++){
	  binding.testNoOpPromise(23);
	}
	const f1 = process.hrtime(e1);

	console.info("Order = testNoOpOriginal testNoOpOriginalCorrected testNoOpPromise");
	console.info("Execution time testNoOpOriginal (hr): %ds %dns", b1[0], b1[1]);
	console.info("Execution time testNoOpOriginalCorrected (hr): %ds %dns", d1[0], d1[1]);
	console.info("Execution time testNoOpPromise (hr): %ds %dns", f1[0], f1[1]);
	console.info("\n");
}

function TestOrder2()
{
	const c1 = process.hrtime();
	for (var i = 0; i <= n; i++){
	  binding.testNoOpOriginalCorrected(23);
	}
	const d1 = process.hrtime(c1);


	const e1 = process.hrtime();
	for (var i = 0; i <= n; i++){
	  binding.testNoOpPromise(23);
	}
	const f1 = process.hrtime(e1);

	const a1 = process.hrtime();
	for (var i = 0; i <= n; i++){
	  binding.testNoOpOriginal(23);
	}
	const b1 = process.hrtime(a1);

	console.info("Order = testNoOpOriginalCorrected testNoOpPromise testNoOpOriginal");
	console.info("Execution time testNoOpOriginal (hr): %ds %dns", b1[0], b1[1]);
	console.info("Execution time testNoOpOriginalCorrected (hr): %ds %dns", d1[0], d1[1]);
	console.info("Execution time testNoOpPromise (hr): %ds %dns", f1[0], f1[1]);
	console.info("\n");
}

function TestOrder3()
{

	const e1 = process.hrtime();
	for (var i = 0; i <= n; i++){
	  binding.testNoOpPromise(23);
	}
	const f1 = process.hrtime(e1);

	const a1 = process.hrtime();
	for (var i = 0; i <= n; i++){
	  binding.testNoOpOriginal(23);
	}
	const b1 = process.hrtime(a1);


	const c1 = process.hrtime();
	for (var i = 0; i <= n; i++){
	  binding.testNoOpOriginalCorrected(23);
	}
	const d1 = process.hrtime(c1);

	console.info("Order = testNoOpPromise testNoOpOriginal testNoOpOriginalCorrected");
	console.info("Execution time testNoOpOriginal (hr): %ds %dns", b1[0], b1[1]);
	console.info("Execution time testNoOpOriginalCorrected (hr): %ds %dns", d1[0], d1[1]);
	console.info("Execution time testNoOpPromise (hr): %ds %dns", f1[0], f1[1]);
	console.info("\n");
}

console.info("Note - ignore the first run, it always seems to start out a little fast");

const iterationCount = 3;
for (var i = 1; i <= iterationCount; i++) {
	console.info("JS Overhead Run" + i);
	TestOrder1();
	TestOrder2();
	TestOrder3();
}

for (var i = 1; i <= iterationCount; i++) {
	console.info("CPP Overhead Run" + i);
	var ret = binding.cppOverheadTest(23);
	console.info(ret);
}


