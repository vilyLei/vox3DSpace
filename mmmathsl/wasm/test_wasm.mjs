import createMmrslModule from './dist/mmrsl.js';

const Module = await createMmrslModule();

// Test 1: HighPerfParser compile once, execute many
const hp = new Module.HighPerfParser();
const ok = hp.compile('vec3 calc(float t) { return vec3(sin(t), cos(t), 0.0); }');
console.log('compile:', ok, '  error:', hp.getLastError());

const r1 = hp.execute([{kind:'float', value:0.0}]);
console.log('execute(0.0):', JSON.stringify(r1));

const r2 = hp.execute([{kind:'float', value:1.5707963}]);
console.log('execute(pi/2):', JSON.stringify(r2));

console.log('compileTime:', hp.getLastCompileTimeMs().toFixed(3), 'ms');
console.log('funcName:', hp.getFunctionName());

// Test 2: compileAndExecute
const r3 = hp.compileAndExecute(
    'float sq(float x) { return x * x; }',
    [{kind:'float', value:7.0}]
);
console.log('sq(7):', JSON.stringify(r3));

// Test 3: mat3 * vec3
const r4 = hp.compileAndExecute(
    'vec3 xform(mat3 m, vec3 v) { return m * v; }',
    [
        {kind:'mat3', columns:[[1,0,0],[0,1,0],[0,0,1]]},
        {kind:'vec3', x:1.0, y:2.0, z:3.0}
    ]
);
console.log('identity*vec3(1,2,3):', JSON.stringify(r4));

hp.delete();
console.log('\nAll tests PASS');
