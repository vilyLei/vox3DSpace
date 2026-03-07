// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
interface WasmModule {
}

type EmbindString = ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string;
export interface ClassHandle {
  isAliasOf(other: ClassHandle): boolean;
  delete(): void;
  deleteLater(): this;
  isDeleted(): boolean;
  // @ts-ignore - If targeting lower than ESNext, this symbol might not exist.
  [Symbol.dispose](): void;
  clone(): this;
}
export interface BytecodeParser extends ClassHandle {
  compile(_0: EmbindString): boolean;
  execute(_0: any): any;
  compileAndExecute(_0: EmbindString, _1: any): any;
  getLastError(): string;
  isCompiled(): boolean;
  getFunctionName(): string;
  getLastCompileTimeMs(): number;
  getLastExecuteTimeMs(): number;
  getBytecodeDisassembly(): string;
}

interface EmbindModule {
  BytecodeParser: {
    new(): BytecodeParser;
  };
}

export type MainModule = WasmModule & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
