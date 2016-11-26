const fs = require('fs');
const entries = fs.readFileSync('./fs.ty', 'utf8').
      split('\n').map(line => JSON.parse(line));
