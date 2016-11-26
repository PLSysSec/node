'use strict';
const FDs       = [];
const VERSION   = '0.0.1';
const fs        = require('fs');
const process   = require('process');

// we may have already messed with module, use the unmodified __func__ if it's there
const openSync  = fs.openSync.__func__  || fs.openSync;
const closeSync = fs.closeSync.__func__ || fs.closeSync;
const writeSync = fs.writeSync.__func__ || fs.writeSync;
const processOn = process.on.__func__   || process.on;

// processOn.call(process, 'beforeExit', () => {
//   FDs.forEach(fd => {
//     try {
//       closeSync(fd);
//     } catch (e) {}
//   });
// });
//
// replace every function in module `mod` with one that logs the types to
// `fileName`
function replace(mod, modName, fileName, omit) {
  omit = omit || [];
  const funcs = Object.keys(mod).
    filter(k => typeof mod[k] === 'function').
    filter(k => omit.indexOf(k) === -1).
    filter(k => /^[a-z]/.test(k));
  const logger = mkLogger(modName, fileName);
  funcs.forEach(funcName => {
    mod[funcName] = wrapFunction(mod[funcName], funcName, logger);
  });
}
module.exports = replace;

function mkLogger(modName, fileName) {
  const fd  = openSync(fileName, 'a+');
  FDs.push(fd);
  const log = (obj) => {
    const result = ('OK' in obj.result) ? 
      typeOf(obj.result.OK) : `throws ${typeOf(obj.result.FAIL)}`;
    let stack = new Error().stack;
    try {
      // trim out error, first line (log) and trailing white space if we can:
      stack = stack.split('\n').slice(2).map(s => s.replace(/^\s*/, '')).join('\n');
    } catch (e) { 
      stack = '<BAD STACK>';
    }

    const json = {
      name: obj.name,
      args: typeOf(obj.arguments),
      result: result,
      stack: stack,
      version: VERSION,
      date: new Date()
    };
    writeSync(fd, `${JSON.stringify(json, 2)}\n`);
  };
  return log;
}


// return a function that wraps `_func` with the sanem name, but additionally
// calls `log` after the function is called with `funcName` and the receiver,
// arguments, and result/exception tuple
function wrapFunction(_func, funcName, log) {
  const o = {
    [funcName]() {
      try {
        const res = _func.apply(this, arguments);
        log({ name: funcName,
              receiver:  this,
              arguments: Array.prototype.slice.call(arguments),
              result: {
                OK: res
              }
        });
        return res;
      } catch (err) {
        log({ name: funcName,
              receiver:  this,
              arguments: Array.prototype.slice.call(arguments),
              result: {
                FAIL: err
              }
        });
        throw err;
      }
    }
  };
  // save original function as __func__
  o[funcName].__func__ = _func;
  return o[funcName];
}

// return the type of the argument, expandinga arrays out
function typeOf(arg) {
  switch(typeof arg) {
    case 'object':
      if (arg === null) {
        return 'null';
      }
      if (Array.isArray(arg)) {
        return `[${Array.prototype.map.call(arg, typeOf).join(', ')}]`;
      }
      return 'object';
    default:
      return typeof arg;
  }
}
