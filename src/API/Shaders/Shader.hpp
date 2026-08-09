#pragma once

#include <Geode/Geode.hpp>



class Shader {
public:
    GLuint vertex = 0;
    GLuint fragment = 0;
    GLuint program = 0;

    Shader() = default;
    ~Shader();

    // no copying
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    // this compiles the 2 shaders togther
    geode::Result<> compile(std::string const& vertexSource, std::string const& fragmentSource);

    // this links the compiled shader
    geode::Result<> link();

    // this cleans up the shader
    void cleanup();
    // did i already run .link()?
    bool isLinked() const { return program != 0 && vertex == 0 && fragment == 0; }

private:
    static std::string getShaderLog(GLuint id);
    static std::string getProgramLog(GLuint id);
};
