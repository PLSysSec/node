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

function TestGet()
{
	const a1 = process.hrtime();
	for (var i = 0; i <= n; i++){
		var ret = binding.testNoOpOriginal_Get(["test"]);
	}
	const b1 = process.hrtime(a1);
	const a2 = process.hrtime();
	for (var i = 0; i <= n; i++){
		var ret = binding.testNoOpOriginalCorrected_GetSlow(["test"]);
	}
	const b2 = process.hrtime(a2);
	const a3 = process.hrtime();
	for (var i = 0; i <= n; i++){
		var ret = binding.testNoOpOriginalCorrected_GetFast(["test"]);
	}
	const b3 = process.hrtime(a3);
	const a4 = process.hrtime();
	for (var i = 0; i <= n; i++){
		var ret = binding.testNoOpPromise_GetSlow(["test"]);
	}
	const b4 = process.hrtime(a4);
	const a5 = process.hrtime();
	for (var i = 0; i <= n; i++){
		var ret = binding.testNoOpPromise_GetFast(["test"]);
	}
	const b5 = process.hrtime(a5);

	console.info("Execution time testNoOpOriginal_Get (hr): %ds %dns", b1[0], b1[1]);
	console.info("Execution time testNoOpOriginalCorrected_GetSlow (hr): %ds %dns", b2[0], b2[1]);
	console.info("Execution time testNoOpOriginalCorrected_GetFast (hr): %ds %dns", b3[0], b3[1]);
	console.info("Execution time testNoOpPromise_GetSlow (hr): %ds %dns", b4[0], b4[1]);
	console.info("Execution time testNoOpPromise_GetFast (hr): %ds %dns", b5[0], b5[1]);

	console.info("\n");
}

function TestGet1()
{
	const a1 = process.hrtime();
	for (var i = 0; i <= n; i++){
		var ret = binding.testNoOpOriginal_Get(["test"]);
	}
	const b1 = process.hrtime(a1);
	console.info("Execution time testNoOpOriginal_Get (hr): %ds %dns", b1[0], b1[1]);
}
function TestGet2()
{
	const a2 = process.hrtime();
	for (var i = 0; i <= n; i++){
		var ret = binding.testNoOpOriginalCorrected_GetSlow(["test"]);
	}
	const b2 = process.hrtime(a2);
	console.info("Execution time testNoOpOriginalCorrected_GetSlow (hr): %ds %dns", b2[0], b2[1]);
}
function TestGet3()
{
	const a3 = process.hrtime();
	for (var i = 0; i <= n; i++){
		var ret = binding.testNoOpOriginalCorrected_GetFast(["test"]);
	}
	const b3 = process.hrtime(a3);
	console.info("Execution time testNoOpOriginalCorrected_GetFast (hr): %ds %dns", b3[0], b3[1]);
}
function TestGet4()
{
	const a4 = process.hrtime();
	for (var i = 0; i <= n; i++){
		var ret = binding.testNoOpPromise_GetSlow(["test"]);
	}
	const b4 = process.hrtime(a4);
	console.info("Execution time testNoOpPromise_GetSlow (hr): %ds %dns", b4[0], b4[1]);
}
function TestGet5()
{
	const a5 = process.hrtime();
	for (var i = 0; i <= n; i++){
		var ret = binding.testNoOpPromise_GetFast(["test"]);
	}
	const b5 = process.hrtime(a5);
	console.info("Execution time testNoOpPromise_GetFast (hr): %ds %dns", b5[0], b5[1]);
}

const iterationCount = 3;
for (var i = 1; i <= iterationCount; i++) {
	console.info("JS With Overhead Run" + i);
	TestOrder1();
	TestOrder2();
	TestOrder3();
}

for (var i = 1; i <= iterationCount; i++) {
	console.info("CPP Overhead Run" + i);
	var ret = binding.cppOverheadTest(23);
	console.info(ret);
}

for (var i = 1; i <= iterationCount; i++) {
	console.info("JS GetField Overhead Run" + i);
	TestGet1();
	TestGet2();
	TestGet3();
	TestGet4();
	TestGet5();
}