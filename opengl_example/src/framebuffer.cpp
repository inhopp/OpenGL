#include "framebuffer.h"

FramebufferUPtr Framebuffer::Create(const TexturePtr colorAttachment) {
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->InitWithColorAttachment(colorAttachment))
        return nullptr;
    return std::move(framebuffer);
}

Framebuffer::~Framebuffer() {
    if (m_depthStencilBuffer) {
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    }
    if (m_framebuffer) {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void Framebuffer::BindToDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);   
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachment(const TexturePtr colorAttachment) {
    m_colorAttachment = colorAttachment;
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer); // 화면이 아닌 frame buffer에 그리겠다

    glFramebufferTexture2D(GL_FRAMEBUFFER,              // frame buffer에 texture 붙이기
        GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,            // 바인딩 해놓은 framebuffer에 color texture를 붙일거야
        colorAttachment->Get(), 0);                     

    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    glRenderbufferStorage(                                          // render buffer의 포맷 지정
        GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,       
        colorAttachment->GetWidth(), colorAttachment->GetHeight());
    glBindRenderbuffer(GL_RENDERBUFFER, 0); // 특정 buffer가 아닌 디폴트

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER, m_depthStencilBuffer);

    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }

    BindToDefault();

    return true;
}