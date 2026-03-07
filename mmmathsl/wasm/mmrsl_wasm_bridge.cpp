// mmrsl WASM bridge — Embind bindings for JS/TS consumers
// Exposes: BytecodeParser only (SimpleParser excluded to minimize WASM size)

#include <emscripten/bind.h>
#include "mmrsl/mmrsl.hpp"

using namespace emscripten;
using namespace mmrsl;

// ============================================================================
// Value helpers: convert between C++ Value and JS plain objects
// JS format: { kind: "vec3", x:..., y:..., z:... } etc.
// ============================================================================

static val valueToJs(const Value& v) {
    val obj = val::object();
    switch (v.kind()) {
        case TypeKind::Int:
            obj.set("kind", std::string("int"));
            obj.set("value", v.asInt());
            break;
        case TypeKind::Float:
            obj.set("kind", std::string("float"));
            obj.set("value", v.asFloat());
            break;
        case TypeKind::Bool:
            obj.set("kind", std::string("bool"));
            obj.set("value", v.asBool());
            break;
        case TypeKind::Vec2: {
            Vec2 u = v.asVec2();
            obj.set("kind", std::string("vec2"));
            obj.set("x", u.x); obj.set("y", u.y);
            break;
        }
        case TypeKind::Vec3: {
            Vec3 u = v.asVec3();
            obj.set("kind", std::string("vec3"));
            obj.set("x", u.x); obj.set("y", u.y); obj.set("z", u.z);
            break;
        }
        case TypeKind::Vec4: {
            Vec4 u = v.asVec4();
            obj.set("kind", std::string("vec4"));
            obj.set("x", u.x); obj.set("y", u.y); obj.set("z", u.z); obj.set("w", u.w);
            break;
        }
        case TypeKind::Mat2: {
            Mat2 m = v.asMat2();
            obj.set("kind", std::string("mat2"));
            val cols = val::array();
            for (int c = 0; c < 2; ++c) {
                val col = val::array();
                for (int r = 0; r < 2; ++r) col.call<void>("push", m[c][r]);
                cols.call<void>("push", col);
            }
            obj.set("columns", cols);
            break;
        }
        case TypeKind::Mat3: {
            Mat3 m = v.asMat3();
            obj.set("kind", std::string("mat3"));
            val cols = val::array();
            for (int c = 0; c < 3; ++c) {
                val col = val::array();
                for (int r = 0; r < 3; ++r) col.call<void>("push", m[c][r]);
                cols.call<void>("push", col);
            }
            obj.set("columns", cols);
            break;
        }
        case TypeKind::Mat4: {
            Mat4 m = v.asMat4();
            obj.set("kind", std::string("mat4"));
            val cols = val::array();
            for (int c = 0; c < 4; ++c) {
                val col = val::array();
                for (int r = 0; r < 4; ++r) col.call<void>("push", m[c][r]);
                cols.call<void>("push", col);
            }
            obj.set("columns", cols);
            break;
        }
        default:
            obj.set("kind", std::string("void"));
            break;
    }
    return obj;
}

static Value jsToValue(const val& obj) {
    std::string kind = obj["kind"].as<std::string>();
    if (kind == "int")   return Value(obj["value"].as<int>());
    if (kind == "float") return Value(obj["value"].as<float>());
    if (kind == "bool")  return Value(obj["value"].as<bool>());
    if (kind == "vec2")  return Value(Vec2(obj["x"].as<float>(), obj["y"].as<float>()));
    if (kind == "vec3")  return Value(Vec3(obj["x"].as<float>(), obj["y"].as<float>(), obj["z"].as<float>()));
    if (kind == "vec4")  return Value(Vec4(obj["x"].as<float>(), obj["y"].as<float>(), obj["z"].as<float>(), obj["w"].as<float>()));
    if (kind == "mat2") {
        val cols = obj["columns"];
        Mat2 m;
        for (int c = 0; c < 2; ++c)
            for (int r = 0; r < 2; ++r)
                m[c][r] = cols[c][r].as<float>();
        return Value(m);
    }
    if (kind == "mat3") {
        val cols = obj["columns"];
        Mat3 m;
        for (int c = 0; c < 3; ++c)
            for (int r = 0; r < 3; ++r)
                m[c][r] = cols[c][r].as<float>();
        return Value(m);
    }
    if (kind == "mat4") {
        val cols = obj["columns"];
        Mat4 m;
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r)
                m[c][r] = cols[c][r].as<float>();
        return Value(m);
    }
    return Value(); // void
}

// ============================================================================
// JS-facing wrapper for BytecodeParser
// ============================================================================

class JsBytecodeParser {
public:
    JsBytecodeParser() = default;

    bool compile(const std::string& source) {
        return parser_.compile(source);
    }

    val execute(const val& jsArgs) {
        std::vector<Value> args;
        int len = jsArgs["length"].as<int>();
        args.reserve(len);
        for (int i = 0; i < len; ++i)
            args.push_back(jsToValue(jsArgs[i]));
        try {
            return valueToJs(parser_.execute(args));
        } catch (const std::exception& e) {
            val err = val::object();
            err.set("kind", std::string("error"));
            err.set("message", std::string(e.what()));
            return err;
        }
    }

    val compileAndExecute(const std::string& source, const val& jsArgs) {
        std::vector<Value> args;
        int len = jsArgs["length"].as<int>();
        args.reserve(len);
        for (int i = 0; i < len; ++i)
            args.push_back(jsToValue(jsArgs[i]));
        try {
            return valueToJs(parser_.compileAndExecute(source, args));
        } catch (const std::exception& e) {
            val err = val::object();
            err.set("kind", std::string("error"));
            err.set("message", std::string(e.what()));
            return err;
        }
    }

    std::string getLastError() const            { return parser_.getLastError(); }
    bool isCompiled() const                     { return parser_.isCompiled(); }
    std::string getFunctionName() const         { return parser_.getFunctionName(); }
    double getLastCompileTimeMs() const         { return parser_.getLastCompileTimeMs(); }
    double getLastExecuteTimeMs() const         { return parser_.getLastExecuteTimeMs(); }
    std::string getBytecodeDisassembly() const  { return parser_.getBytecodeDisassembly(); }

private:
    BytecodeParser parser_;
};

// ============================================================================
// Embind registration
// ============================================================================

EMSCRIPTEN_BINDINGS(mmrsl_module) {
    class_<JsBytecodeParser>("BytecodeParser")
        .constructor<>()
        .function("compile",                &JsBytecodeParser::compile)
        .function("execute",                &JsBytecodeParser::execute)
        .function("compileAndExecute",      &JsBytecodeParser::compileAndExecute)
        .function("getLastError",           &JsBytecodeParser::getLastError)
        .function("isCompiled",             &JsBytecodeParser::isCompiled)
        .function("getFunctionName",        &JsBytecodeParser::getFunctionName)
        .function("getLastCompileTimeMs",   &JsBytecodeParser::getLastCompileTimeMs)
        .function("getLastExecuteTimeMs",   &JsBytecodeParser::getLastExecuteTimeMs)
        .function("getBytecodeDisassembly", &JsBytecodeParser::getBytecodeDisassembly);
}
