#include "Shader.hpp"

using namespace geode::prelude;

Shader::~Shader() {
    cleanup();
}

Shader::Shader(Shader&& other) noexcept
    : vertex(other.vertex), fragment(other.fragment), program(other.program) {
    other.vertex = 0;
    other.fragment = 0;
    other.program = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        cleanup();
        vertex = other.vertex;
        fragment = other.fragment;
        program = other.program;
        other.vertex = 0;
        other.fragment = 0;
        other.program = 0;
    }
    return *this;
}

std::string Shader::getShaderLog(GLuint id) {
    GLint length = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    if (length <= 0) return "";

    auto buf = std::make_unique<char[]>(length + 1);
    GLsizei written = 0;
    glGetShaderInfoLog(id, length, &written, buf.get());
    return std::string(buf.get());
}

std::string Shader::getProgramLog(GLuint id) {
    GLint length = 0;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
    if (length <= 0) return "";

    auto buf = std::make_unique<char[]>(length + 1);
    GLsizei written = 0;
    glGetProgramInfoLog(id, length, &written, buf.get());
    return std::string(buf.get());
}

Result<> Shader::compile(std::string const& vertexSource, std::string const& fragmentSource) {
    GLint status = 0;
    std::string header = "";
    #ifdef GEODE_IS_WINDOWS
        header += "#version 120\n";
    #endif
    #ifdef GEODE_IS_MOBILE
        header += "precision highp float;\n";
    #endif

    std::string const finalVertexSource = header + vertexSource;
    std::string const finalFragmentSource = header + fragmentSource;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    {
        const char* src = finalVertexSource.c_str();
        glShaderSource(vertex, 1, &src, nullptr);
    }
    glCompileShader(vertex);

    auto vertexLog = string::trim(getShaderLog(vertex));
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &status);
    if (!status) {
        glDeleteShader(vertex);
        vertex = 0;
        return Err("vertex shader compilation failed:\n{}", vertexLog);
    }
    if (!vertexLog.empty()) {
        log::debug("vertex shader compiled with warnings:\n{}", vertexLog);
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    {
        const char* src = finalFragmentSource.c_str();
        glShaderSource(fragment, 1, &src, nullptr);
    }
    glCompileShader(fragment);

    auto fragmentLog = string::trim(getShaderLog(fragment));
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &status);
    if (!status) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        vertex = 0;
        fragment = 0;
        return Err("fragment shader compilation failed:\n{}", fragmentLog);
    }
    if (!fragmentLog.empty()) {
        log::debug("fragment shader compiled with warnings:\n{}", fragmentLog);
    }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    return Ok();
}

Result<> Shader::link() {
    if (!vertex) return Err("vertex shader not compiled");
    if (!fragment) return Err("fragment shader not compiled");
    if (!program) return Err("program not created");

    glLinkProgram(program);
    auto programLog = string::trim(getProgramLog(program));

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    vertex = 0;
    fragment = 0;

    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        glDeleteProgram(program);
        program = 0;
        return Err("shader link failed:\n{}", programLog);
    }

    if (!programLog.empty()) {
        log::debug("shader linked with warnings:\n{}", programLog);
    }

    return Ok();
}

void Shader::cleanup() {
    if (vertex) {
        glDeleteShader(vertex);
        vertex = 0;
    }
    if (fragment) {
        glDeleteShader(fragment);
        fragment = 0;
    }
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
}