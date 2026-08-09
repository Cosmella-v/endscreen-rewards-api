#include "ShaderEffect.hpp"

using namespace geode::prelude;

void ShaderEffect::unload() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
    if (m_tex) glDeleteTextures(1, &m_tex);
    if (m_shader.program) m_shader.cleanup();
    m_ready = false;
};

bool ShaderEffect::init(std::string const& vertSrc, std::string const& fragSrc) {
    if (m_ready) return true;

    auto res = m_shader.compile(vertSrc, fragSrc);
    if (!res) {
        log::error("{}", res.unwrapErr());
        return false;
    }

    glBindAttribLocation(m_shader.program, 0, "aPosition");
    glBindAttribLocation(m_shader.program, 1, "aTexCoords");

    res = m_shader.link();
    if (!res) {
        log::error("{}", res.unwrapErr());
        return false;
    }

    m_uMvpLoc = glGetUniformLocation(m_shader.program, "u_MVPMatrix");
    m_uTextureLoc = glGetUniformLocation(m_shader.program, "u_texture");

    m_ready = true;
    return true;
}

bool ShaderEffect::calculateSize(CCSize const& size) {
    if (m_fbo == 0 || m_texSize != size) {
        if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
        if (m_tex) glDeleteTextures(1, &m_tex);

        float const scale = CC_CONTENT_SCALE_FACTOR();
        GLsizei const texW = (GLsizei)(size.width * scale);
        GLsizei const texH = (GLsizei)(size.height * scale);

        glGenFramebuffers(1, &m_fbo);
        glGenTextures(1, &m_tex);

        glBindTexture(GL_TEXTURE_2D, m_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH,
                    0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            log::error("ShaderEffect fbo incomplete");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_texSize = size;
    }
    if (m_vao == 0 || m_quadSize != size) {
        if (m_vao) glDeleteVertexArrays(1, &m_vao);
        if (m_vbo) glDeleteBuffers(1, &m_vbo);

        float const w = size.width;
        float const h = size.height;

        GLfloat const verts[] = {
            0.f, 0.f,   0.f, 0.f,
            0.f, h,     0.f, 1.f,
            w,   0.f,   1.f, 0.f,

            w,   0.f,   1.f, 0.f,
            0.f, h,     0.f, 1.f,
            w,   h,     1.f, 1.f,
        };

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_quadSize = size;
    }

    return true;
}

bool ShaderEffect::applyShader(
    CCNode* node,
    std::function<void(CCNode*)> onChildren,
    std::function<void(GLuint program)> const& setUniforms
) {
    if (!node) return false;
    auto size = node->getContentSize();
    if (!m_ready || size.width <= 0 || size.height <= 0 || !calculateSize(size)) {
        return false;
    }

    float const scale = CC_CONTENT_SCALE_FACTOR();
    GLsizei const texW = (GLsizei)(size.width * scale);
    GLsizei const texH = (GLsizei)(size.height * scale);

    GLint prevFbo = 0;
    GLint prevViewport[4];
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFbo);
    glGetIntegerv(GL_VIEWPORT, prevViewport);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, texW, texH);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    kmGLMatrixMode(KM_GL_PROJECTION);
    kmGLPushMatrix();
    kmMat4 ortho;
    kmMat4OrthographicProjection(&ortho, 0, size.width, 0, size.height, -1024, 1024);
    kmGLLoadMatrix(&ortho);

    kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLPushMatrix();
    kmGLLoadIdentity();

    if (onChildren) {
        node->sortAllChildren();
        if (auto children = node->getChildren()) {
            for (auto* child : CCArrayExt<CCNode*>(children)) {
                onChildren(child);
            }
        }
    }

    kmGLPopMatrix();
    kmGLMatrixMode(KM_GL_PROJECTION);
    kmGLPopMatrix();
    kmGLMatrixMode(KM_GL_MODELVIEW);

    glBindFramebuffer(GL_FRAMEBUFFER, prevFbo);
    glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);

    kmGLPushMatrix();
    node->transform();

    kmMat4 modelview, projection, mvp;
    kmGLGetMatrix(KM_GL_MODELVIEW, &modelview);
    kmGLGetMatrix(KM_GL_PROJECTION, &projection);
    kmMat4Multiply(&mvp, &projection, &modelview);

    glEnable(GL_BLEND);
    ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ccGLUseProgram(m_shader.program);
    glUniformMatrix4fv(m_uMvpLoc, 1, GL_FALSE, mvp.mat);
    glUniform1i(m_uTextureLoc, 0);

    if (setUniforms) setUniforms(m_shader.program);

    ccGLBindTexture2D(m_tex);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    kmGLPopMatrix();

    ccGLBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);

    return true;
}

ShaderEffect::~ShaderEffect() {
    unload();
}