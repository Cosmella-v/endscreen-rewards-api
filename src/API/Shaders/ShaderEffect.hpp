#pragma once

#include <functional>
#include <string>

#include <Geode/Geode.hpp>
#include "Shader.hpp" 

using namespace geode::prelude;
/*
A helper used to Set up shaders in cocos2d-x
*/
class ShaderEffect {
public:
    /*
    Uses a Vertex shader and a Fragment shader
    */
    bool init(std::string const& vertSrc, std::string const& fragSrc);

    /*
    used to render a shader on a node (used in ::Visit())
    */
    bool applyShader(
        CCNode* node,
        std::function<void(CCNode*)> onChildren = nullptr,
        std::function<void(GLuint program)> const& setUniforms = nullptr
    );

    bool isReady() const { return m_ready; }
    /*unload the shader*/
    void unload();

    ~ShaderEffect();

private:
    // internal function to change when the size changes!
    bool calculateSize(CCSize const& size);

    Shader m_shader;
    bool m_ready = false;

    GLuint m_fbo = 0;
    GLuint m_tex = 0;
    CCSize m_texSize;

    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    CCSize m_quadSize;

    GLint m_uMvpLoc = -1;
    GLint m_uTextureLoc = -1;
};
