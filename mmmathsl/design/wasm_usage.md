# mmrsl WASM Usage Guide

## Overview

mmrsl compiles to a WebAssembly module that exposes `BytecodeParser` to JavaScript/TypeScript. The module is built with Emscripten and outputs an ES6 module.

**Output files** (in `wasm/dist/`):

| File | Compressed (gzip) | Description |
|------|-------------------|-------------|
| `mmrsl.js` | ~14 KB | ES6 loader + Embind glue |
| `mmrsl.wasm` | ~108 KB | Bytecode VM binary |
| `mmrsl.d.ts` | — | TypeScript type declarations |

---

## Loading the Module

### Browser (ES Module)

```html
<script type="module">
import createMmrslModule from './mmrsl.js';

const Module = await createMmrslModule();
// Module is ready
</script>
```

### Node.js

Requires a `package.json` with `"type": "module"` in the same directory as `mmrsl.js`, or use a `.mjs` file.

```js
import createMmrslModule from './dist/mmrsl.js';

const Module = await createMmrslModule();
```

### Vite / Webpack / Bundler

```js
import createMmrslModule from './wasm/dist/mmrsl.js?url'; // or adjust path
// ...
```

> **Note**: Some bundlers need special handling for `.wasm` files. Configure the bundler to serve `mmrsl.wasm` as a static asset alongside `mmrsl.js`.

---

## Core Pattern: Compile Once, Execute Many

The intended usage is:
1. **Compile** the script once (parsing + bytecode generation, ~1–5 ms)
2. **Execute** the compiled bytecode repeatedly per frame (~microseconds)

```js
const Module = await createMmrslModule();

// Create a parser instance (one per script)
const parser = new Module.BytecodeParser();

// Compile once
const ok = parser.compile(`
    vec3 orbit(float t) {
        return vec3(cos(t), 0.0, sin(t));
    }
`);

if (!ok) {
    console.error('Compile error:', parser.getLastError());
}

// Execute many times (e.g., per frame)
function update(t) {
    const result = parser.execute([{ kind: 'float', value: t }]);
    // result: { kind: 'vec3', x: ..., y: ..., z: ... }
    return result;
}

// Always delete when done to free C++ heap memory
parser.delete();
```

---

## API Reference

### `new Module.BytecodeParser()`

Creates a new parser instance. Each instance holds its own compiled bytecode state.

---

### `compile(source: string): boolean`

Compiles GLSL-style source code to bytecode. Returns `true` on success.

```js
const ok = parser.compile(`
    float smoothstep_custom(float t) {
        return t * t * (3.0 - 2.0 * t);
    }
`);
if (!ok) console.error(parser.getLastError());
```

---

### `execute(args: ValueObject[]): ValueObject`

Executes the compiled bytecode with the given arguments. Returns the function result as a `ValueObject`.

```js
const result = parser.execute([{ kind: 'float', value: 0.5 }]);
// { kind: 'float', value: 0.375 }
```

---

### `compileAndExecute(source: string, args: ValueObject[]): ValueObject`

Convenience method: compiles and immediately executes. Useful for one-shot calculations.

```js
const result = parser.compileAndExecute(
    'float sq(float x) { return x * x; }',
    [{ kind: 'float', value: 7.0 }]
);
// { kind: 'float', value: 49 }
```

---

### `getLastError(): string`

Returns the last compilation error message. Empty string if no error.

---

### `isCompiled(): boolean`

Returns `true` if bytecode is ready to execute.

---

### `getLastCompileTimeMs(): number`

Returns the duration of the last `compile()` call in milliseconds.

---

### `getLastExecuteTimeMs(): number`

Returns the duration of the last `execute()` call in milliseconds.

---

### `getBytecodeDisassembly(): string`

Returns a human-readable disassembly of the compiled bytecode. Useful for debugging.

```js
console.log(parser.getBytecodeDisassembly());
// LOAD_CONST r0, 3.0
// LOAD_PARAM r1, 0
// MUL_FLOAT  r2, r0, r1
// ...
```

---

### `delete()`

**Must be called** when the parser is no longer needed. Frees the underlying C++ heap memory.

```js
parser.delete();
```

---

## Value Object Format

Arguments and return values are plain JS objects with a `kind` field:

### Scalar types

```js
{ kind: 'float', value: 1.0 }
{ kind: 'int',   value: 42  }
{ kind: 'bool',  value: true }
```

### Vector types

```js
{ kind: 'vec2', x: 1.0, y: 2.0 }
{ kind: 'vec3', x: 1.0, y: 2.0, z: 3.0 }
{ kind: 'vec4', x: 1.0, y: 2.0, z: 3.0, w: 4.0 }
```

### Matrix types (column-major)

```js
// mat3: columns[col][row]
{
    kind: 'mat3',
    columns: [
        [1, 0, 0],   // column 0
        [0, 1, 0],   // column 1
        [0, 0, 1]    // column 2
    ]
}

// mat2, mat4 follow the same pattern
```

### Error return

When `execute()` encounters a runtime error, it returns:

```js
{ kind: 'error', message: 'Division by zero' }
```

---

## Supported GLSL-style Syntax

### Types

`float`, `int`, `bool`, `vec2`, `vec3`, `vec4`, `mat2`, `mat3`, `mat4`

### Control Flow

```glsl
if / else
for (init; condition; update)
break / continue / return
```

### Operators

Arithmetic: `+`, `-`, `*`, `/`  
Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`  
Logical: `&&`, `||`, `!` (short-circuit evaluated)  
Ternary: `condition ? a : b`

### Built-in Functions

| Category | Functions |
|----------|-----------|
| Math | `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan(y,x)` |
| Math | `sqrt`, `pow`, `exp`, `log`, `abs`, `sign`, `floor`, `ceil`, `fract`, `mod` |
| Range | `min`, `max`, `clamp`, `mix`, `step`, `smoothstep` |
| Geometry | `length`, `distance`, `dot`, `cross`, `normalize` |
| Geometry | `reflect`, `refract`, `faceforward` |
| Matrix | `transpose`, `inverse`, `determinant` |
| Vector | `any`, `all`, `not` |

### Swizzles

```glsl
vec3 v = vec3(1.0, 2.0, 3.0);
float x = v.x;     // component access
vec2 xy = v.xy;    // swizzle
vec3 zyx = v.zyx;  // reorder
```

---

## Example: Per-frame Animation (Game Loop)

```js
const Module = await createMmrslModule();
const parser = new Module.BytecodeParser();

parser.compile(`
    vec3 wave(float t, vec3 basePos) {
        float offset = sin(t * 2.0 + basePos.x) * 0.3;
        return vec3(basePos.x, basePos.y + offset, basePos.z);
    }
`);

// Game loop
let t = 0;
function tick(dt) {
    t += dt;
    const pos = parser.execute([
        { kind: 'float', value: t },
        { kind: 'vec3', x: 0.0, y: 1.0, z: 0.0 }
    ]);
    // pos: { kind: 'vec3', x: 0, y: ..., z: 0 }
}
```

---

## Example: Mat3 Transform

```js
const result = parser.compileAndExecute(
    'vec3 xform(mat3 m, vec3 v) { return m * v; }',
    [
        {
            kind: 'mat3',
            columns: [[1,0,0],[0,1,0],[0,0,1]]  // identity
        },
        { kind: 'vec3', x: 1.0, y: 2.0, z: 3.0 }
    ]
);
// result: { kind: 'vec3', x: 1, y: 2, z: 3 }
```

---

## Build from Source

Prerequisites: [emsdk](https://emscripten.org/docs/getting_started/downloads.html) installed and activated.

```bash
# Activate emsdk (PowerShell example)
D:\dev\emsdk\emsdk_env.ps1

# Configure (first time only)
mkdir wasm/build
cd wasm/build
emcmake cmake ../.. -DCMAKE_BUILD_TYPE=Release

# Build
emmake cmake --build . --target mmrsl_wasm
```

Output goes to `wasm/dist/`.
