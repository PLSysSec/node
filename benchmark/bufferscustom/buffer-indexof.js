'use strict';
var common = require('../common.js');
var fs = require('fs');
const path = require('path');
const v8 = require('v8');
var binding = process.binding('buffer');

var bench = common.createBenchmark(main, {
  search: [//'@'
          // , 'SQ', '10x', '--l', 'Alice', 'Gryphon', 'Panther',
          //  'Ou est ma chatte?', 'found it very', 
          'among mad people'
          //, 'neighbouring pool', 'Soo--oop', 
          //'aaaaaaaaaaaaaaaaa'
          //  ,'venture to go near the house till she had brought herself down to',
          //  '</i> to the Caterpillar'
           ],
  encoding: ['undefined', 
            //'utf8' ,
            //'ucs2'
            //, 'binary'
            ],
  type: ['buffer', /*'string'*/],
  iter: [10]
});

function main(conf) {
  var iter = ((conf.iter) * 100000)>>0;
  var aliceBuffer = fs.readFileSync(
    path.resolve(__dirname, '../fixtures/alice.html')
  );
  var search = conf.search;
  var encoding = conf.encoding;

  if (encoding === 'undefined') {
    encoding = undefined;
  }

  if (encoding === 'ucs2') {
    aliceBuffer = Buffer.from(aliceBuffer.toString(), encoding);
  }

  if (conf.type === 'buffer') {
    search = Buffer.from(Buffer.from(search).toString(), encoding);
  }

  v8.setFlagsFromString('--allow_natives_syntax');
  binding.indexOfBuffer(aliceBuffer, search, 0, encoding, true);
  binding.indexOfBuffer(aliceBuffer, search, 0, encoding, true);
  eval('%OptimizeFunctionOnNextCall(binding.indexOfBuffer)');
  binding.indexOfBuffer(aliceBuffer, search, 0, encoding, true);

  bench.start();
  for (var i = 0; i < iter; i++) {
    binding.indexOfBuffer(aliceBuffer, search, 0, encoding, true);
  }
  bench.end(iter);
}
