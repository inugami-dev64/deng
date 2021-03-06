// DENG: dynamic engine - small but powerful 3D game engine
// licence: Apache, see LICENCE file
// file: OpenGLRenderer.cpp - OpenGL renderer class implementation
// author: Karl-Mihkel Ott

#define OPENGL_RENDERER_CPP
#include <OpenGLRenderer.h>


namespace DENG {

#ifdef _DEBUG
    void lglErrorCheck(const std::string &_func, const std::string &_file, const uint32_t _line) {
        GLenum error;
        while((error = glGetError()) != GL_NO_ERROR) {
            switch(error) {
                case GL_INVALID_ENUM:
                    std::cerr << _func << "(); " << _file << ", " << _line << ": GL_INVALID_ENUM " << std::endl;
                    break;

                case GL_INVALID_VALUE:
                    std::cerr << _func << "(); " << _file << ", " << _line << " GL_INVALID_VALUE " << std::endl;
                    break;

                case GL_INVALID_OPERATION:
                    std::cerr << _func << "(); " << _file << ", " << _line << " GL_INVALID_OPERATION " << std::endl;
                    break;

                case GL_STACK_OVERFLOW:
                    std::cerr << _func << "(); " << _file << ", " << _line << " GL_STACK_OVERFLOW " << std::endl;
                    break;

                case GL_STACK_UNDERFLOW:
                    std::cerr << _func << "(); " << _file << ", " << _line << " GL_STACK_UNDERFLOW " << std::endl;
                    break;

                case GL_OUT_OF_MEMORY:
                    std::cerr << _func << "(); " << _file << ", " << _line << " GL_OUT_OF_MEMORY " << std::endl;
                    break;

                default:
                    break;
            }
            std::abort();
        }
    }
#endif

    namespace OpenGL {
        void Initialise(Window &_win) {
            RenderState *rs = RenderState::GetInstance();
            if(rs->GetPrimary() == RENDERER_TYPE_UNKNOWN)
                rs->SetPrimary(RENDERER_TYPE_OPENGL);

            _win.glMakeCurrent();

            // load OpenGL functions
            int status = deng_LoadGL();
            if(!status) {
                std::cerr << "Unexpected error occured when loading OpenGL functions" << std::endl;
                std::exit(-1);
            }
        }
    }


    OpenGLRenderer::OpenGLRenderer(Window &_win, const RendererConfig &_conf) : 
        Renderer(_win, _conf),
        m_buffer_loader()
    {
        RenderState *rs = RenderState::GetInstance();
        m_id = rs->RegisterRenderer(RENDERER_TYPE_OPENGL, time(NULL));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glErrorCheck("glTexParameteri");

        // load missing texture
        int x, y, size;
        const char *tex = GetMissingTexture(x, y, size);
        PushTextureFromMemory(MISSING_TEXTURE_NAME, tex, x, y, 4);

        // create main framebuffer instance
        m_framebuffer_draws.insert({
            MAIN_FRAMEBUFFER_NAME,
            {
                MAIN_FRAMEBUFFER_NAME,
                {}, {},
                {
                    static_cast<uint32_t>(m_window.GetSize().x),
                    static_cast<uint32_t>(m_window.GetSize().y)
                }
            }
        });

        m_framebuffers.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(MAIN_FRAMEBUFFER_NAME),
            std::forward_as_tuple(
                MAIN_FRAMEBUFFER_NAME,
                m_framebuffer_draws,
                m_opengl_textures,
                m_buffer_loader.GetBufferData(),
                0,
                true
            )
        );
    }


    OpenGLRenderer::~OpenGLRenderer() {
        RenderState *rs = RenderState::GetInstance();
        rs->RemoveRenderer(m_id);

        // delete texture objects
        for(auto it = m_opengl_textures.begin(); it != m_opengl_textures.end(); it++)
            glDeleteTextures(1, &it->second);
    }


    void OpenGLRenderer::PushFramebuffer(const FramebufferDrawData &_fb) {
        DENG_ASSERT(m_framebuffer_draws.find(_fb.image_name) == m_framebuffer_draws.end());
        DENG_ASSERT(m_framebuffers.find(_fb.image_name) == m_framebuffers.end());
        DENG_ASSERT(m_opengl_textures.find(_fb.image_name) == m_opengl_textures.end());

        // submit framebuffer draw to hashmap
        m_framebuffer_draws.insert(std::make_pair(_fb.image_name, _fb));

        GLuint texture;
        glGenTextures(1, &texture);
        glErrorCheck("glGenTextures");

        glBindTexture(GL_TEXTURE_2D, texture);
        glErrorCheck("glBindTextures");

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei) _fb.extent.x, (GLsizei) _fb.extent.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glErrorCheck("glTexImage2D");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        m_opengl_textures.insert(std::make_pair(_fb.image_name, texture));
        m_framebuffers.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(_fb.image_name),
            std::forward_as_tuple(
                _fb.image_name,
                m_framebuffer_draws,
                m_opengl_textures,
                m_buffer_loader.GetBufferData(),
                texture,
                false
            )
        );
    }


    void OpenGLRenderer::PushTextureFromFile(const std::string &_name, const std::string &_file_name) {
        Libdas::TextureReader reader(Libdas::Algorithm::RelativePathToAbsolute(_file_name));
        size_t len;
        int x, y;
        const char *buf = reader.GetRawBuffer(x, y, len);
        PushTextureFromMemory(_name, buf, static_cast<uint32_t>(x), static_cast<uint32_t>(x), 4);
    }


    void OpenGLRenderer::PushTextureFromMemory(const std::string &_name, const char *_raw_data, uint32_t _width, uint32_t _height, uint32_t _bit_depth) {
        // check if the named texture already exists in texture map
        if(m_opengl_textures.find(_name) != m_opengl_textures.end()) {
            std::cerr << "Texture with name '" << _name << "' already exists in texture map" << std::endl;
            DENG_ASSERT(false);
            return;
        }

        // check if there are any texture units available
        GLint max_units;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_units);

        if(static_cast<GLint>(m_opengl_textures.size() + 1) >= max_units) {
            std::cerr << "Current texture exceeds the maximum texture capacity\n"\
                         "Requested " << m_opengl_textures.size() + 1 << ", avalilable" << max_units << std::endl;
            std::exit(1);
        }

        GLuint tex;
        glGenTextures(1, &tex);
        glErrorCheck("glGenTextures");

        glBindTexture(GL_TEXTURE_2D, tex);
        glErrorCheck("glBindTexture");

        switch(_bit_depth) {
            case 3:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height), 0, GL_RGB, GL_UNSIGNED_BYTE, _raw_data);
                break;

            case 4:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height), 0, GL_RGBA, GL_UNSIGNED_BYTE, _raw_data);
                break;

            default:
                DENG_ASSERT(false);
                break;
        }

        glErrorCheck("glTexImage2D");

        glGenerateMipmap(GL_TEXTURE_2D);
        glErrorCheck("glGenerateMipmap");
        
        m_opengl_textures[_name] = tex;
    }


    void OpenGLRenderer::RemoveTexture(const std::string &_name) {
        // check if texture does not exist in current hash map
        if(m_opengl_textures.find(_name) == m_opengl_textures.end()) {
            std::cerr << "Requested texture with name '" << _name << "' does not exist for removal" << std::endl;
            DENG_ASSERT(false);
            return;
        }

        glDeleteTextures(1, &m_opengl_textures[_name]);
    }


    std::vector<std::string> OpenGLRenderer::GetTextureNames() {
        std::vector<std::string> names;

        for(auto it = m_opengl_textures.begin(); it != m_opengl_textures.end(); it++)
            names.push_back(it->first);

        return names;
    }


    uint32_t OpenGLRenderer::AlignUniformBufferOffset(uint32_t _req) {
        return AlignData(_req, m_buffer_loader.GetUniformBufferOffsetAlignment());
    }


    void OpenGLRenderer::LoadShaders() {
        for(auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++) {
            it->second.LoadData();
        }
    }


    void OpenGLRenderer::UpdateUniform(const char *_raw_data, uint32_t _size, uint32_t _offset) {
        m_buffer_loader.RequestMemory(_offset + _size, GL_UNIFORM_BUFFER);

        if(_raw_data) {
            void *data = glMapNamedBufferRange(m_buffer_loader.GetBufferData().ubo_buffer, (GLintptr) _offset, (GLsizeiptr) _size, GL_MAP_WRITE_BIT);
            glErrorCheck("glMapBufferRange");
            std::memcpy(data, _raw_data, static_cast<size_t>(_size));
            glUnmapNamedBuffer(m_buffer_loader.GetBufferData().ubo_buffer);
            glErrorCheck("glUnmapNamedBuffer");
        }
    }


    void OpenGLRenderer::UpdateVertexDataBuffer(std::pair<const char*, uint32_t> _raw_data, uint32_t _offset) {
        m_buffer_loader.RequestMemory(_offset + _raw_data.second, GL_ARRAY_BUFFER);
        void *data = glMapNamedBufferRange(m_buffer_loader.GetBufferData().vert_buffer, (GLintptr) _offset, (GLsizeiptr) _raw_data.second, GL_MAP_WRITE_BIT);
        glErrorCheck("glMapNamedBufferRange");
            std::memcpy(data, _raw_data.first, static_cast<size_t>(_raw_data.second));
        glUnmapNamedBuffer(m_buffer_loader.GetBufferData().vert_buffer);
        glErrorCheck("glUnmapeNamedBuffer");
    }


    void OpenGLRenderer::ClearFrame() {
        glViewport(0, 0, static_cast<GLsizei>(m_window.GetSize().x), static_cast<GLsizei>(m_window.GetSize().y));
        glErrorCheck("glViewport");

        m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second.ClearFrame(m_conf.clear_color);
        for(auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++) {
            it->second.ClearFrame(m_conf.clear_color);
        }
    }


    void OpenGLRenderer::RenderFrame() {
        // update all non-default framebuffers
        for(auto it = m_framebuffers.begin(); it != m_framebuffers.end(); it++) {
            if(it->first == MAIN_FRAMEBUFFER_NAME)
                continue;
            it->second.Render();
        }

        m_framebuffer_draws.find(MAIN_FRAMEBUFFER_NAME)->second.extent = {
            static_cast<uint32_t>(m_window.GetSize().x),
            static_cast<uint32_t>(m_window.GetSize().y)
        };

        m_framebuffers.find(MAIN_FRAMEBUFFER_NAME)->second.Render();
    }
}
