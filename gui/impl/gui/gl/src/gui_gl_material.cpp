#include "lxgui/impl/gui_gl_material.hpp"
#include "lxgui/impl/gui_gl_manager.hpp"
#include <lxgui/gui_out.hpp>

#include <lxgui/utils_string.hpp>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <cmath>

#ifdef MSVC
template<typename T>
T log2(T v)
{
    return log(v)/log(2.0);
}
#endif // MSVC

namespace gui {
namespace gl
{
bool material::ONLY_POWER_OF_TWO = true;
uint material::MAXIMUM_SIZE = 128;

ub32color::ub32color()
{
}

ub32color::ub32color(chanel r, chanel g, chanel b, chanel a) : r(r), g(g), b(b), a(a)
{
}

material::material(uint uiWidth, uint uiHeight, wrap mWrap, filter mFilter, bool bGPUOnly) :
    mType_(TYPE_TEXTURE)
{
    pTexData_ = utils::refptr<texture_data>(new texture_data());
    pTexData_->uiWidth_ = uiWidth;
    pTexData_->uiHeight_ = uiHeight;
    pTexData_->mWrap_ = mWrap;
    pTexData_->mFilter_ = mFilter;

    if (ONLY_POWER_OF_TWO)
    {
        pTexData_->uiRealWidth_ = pow(2.0f, ceil(log2((float)uiWidth)));
        pTexData_->uiRealHeight_ = pow(2.0f, ceil(log2((float)uiHeight)));
    }
    else
    {
        pTexData_->uiRealWidth_ = uiWidth;
        pTexData_->uiRealHeight_ = uiHeight;
    }

    if (pTexData_->uiRealWidth_ > MAXIMUM_SIZE || pTexData_->uiRealHeight_ > MAXIMUM_SIZE)
    {
        throw gui::exception("gui::gl::material", "Texture dimensions not supported by graphics card : ("+
            utils::to_string(pTexData_->uiRealWidth_)+" x "+
            utils::to_string(pTexData_->uiRealHeight_)+")."
        );
    }

    GLint iPreviousID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &iPreviousID);

    glGenTextures(1, &pTexData_->uiTextureHandle_);

    glBindTexture(GL_TEXTURE_2D, pTexData_->uiTextureHandle_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
        pTexData_->uiRealWidth_, pTexData_->uiRealHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );

    switch (mWrap)
    {
    case CLAMP :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        break;
    case REPEAT :
    default :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    }
    switch (mFilter)
    {
    case LINEAR :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    case NONE :
    default :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    }

    glBindTexture(GL_TEXTURE_2D, iPreviousID);

    if (!bGPUOnly)
        pTexData_->pData_.resize(uiWidth*uiHeight);
}

material::material(const color& mColor) : mType_(TYPE_COLOR)
{
    pColData_ = utils::refptr<color_data>(new color_data());
    pColData_->mColor_ = mColor;
}

material::~material()
{
    switch (mType_)
    {
    case TYPE_TEXTURE :
        glDeleteTextures(1, &pTexData_->uiTextureHandle_); break;
    case TYPE_COLOR :
        break;
    }
}

material::type material::get_type() const
{
    return mType_;
}

color material::get_color() const
{
    return pColData_->mColor_;
}

void material::set_wrap(wrap mWrap)
{
    GLint iPreviousID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &iPreviousID);
    glBindTexture(GL_TEXTURE_2D, pTexData_->uiTextureHandle_);

    switch (mWrap)
    {
    case CLAMP :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        break;
    case REPEAT :
    default :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    }

    glBindTexture(GL_TEXTURE_2D, iPreviousID);
}

void material::set_filter(filter mFilter)
{
    GLint iPreviousID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &iPreviousID);
    glBindTexture(GL_TEXTURE_2D, pTexData_->uiTextureHandle_);

    switch (mFilter)
    {
    case LINEAR :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    case NONE :
    default :
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    }

    glBindTexture(GL_TEXTURE_2D, iPreviousID);
}

void material::bind() const
{
    glBindTexture(GL_TEXTURE_2D, pTexData_->uiTextureHandle_);
}

const std::vector<ub32color>& material::get_data() const
{
    return pTexData_->pData_;
}

std::vector<ub32color>& material::get_data()
{
    return pTexData_->pData_;
}

void material::set_pixel(uint x, uint y, const ub32color& mColor)
{
    pTexData_->pData_[x + y*pTexData_->uiWidth_] = mColor;
}

const ub32color& material::get_pixel(uint x, uint y) const
{
    return pTexData_->pData_[x + y*pTexData_->uiWidth_];
}

ub32color& material::get_pixel(uint x, uint y)
{
    return pTexData_->pData_[x + y*pTexData_->uiWidth_];
}

void material::premultiply_alpha()
{
    uint imax = pTexData_->uiWidth_*pTexData_->uiHeight_;
    for (uint i = 0; i < imax;  ++i)
    {
        ub32color& c = pTexData_->pData_[i];
        float a = c.a/255.0f;
        c.r *= a;
        c.g *= a;
        c.b *= a;
    }
}

float material::get_width() const
{
    switch (mType_)
    {
    case TYPE_TEXTURE :
        return pTexData_->uiWidth_;
    case TYPE_COLOR :
        return 1.0f;
    }

    return 1.0f;
}

float material::get_height() const
{
    switch (mType_)
    {
    case TYPE_TEXTURE :
        return pTexData_->uiHeight_;
    case TYPE_COLOR :
        return 1.0f;
    }

    return 1.0f;
}

float material::get_real_width() const
{
    switch (mType_)
    {
    case TYPE_TEXTURE :
        return pTexData_->uiRealWidth_;
    case TYPE_COLOR :
        return 1.0f;
    }

    return 1.0f;
}

float material::get_real_height() const
{
    switch (mType_)
    {
    case TYPE_TEXTURE :
        return pTexData_->uiRealHeight_;
    case TYPE_COLOR :
        return 1.0f;
    }

    return 1.0f;
}

bool material::set_dimensions(uint uiWidth, uint uiHeight)
{
    if (uiWidth > pTexData_->uiRealWidth_ || uiHeight > pTexData_->uiRealHeight_)
    {

        utils::refptr<texture_data> pOldData = pTexData_;
        pTexData_ = utils::refptr<texture_data>(new texture_data());
        pTexData_->uiWidth_  = uiWidth;
        pTexData_->uiHeight_ = uiHeight;
        pTexData_->mWrap_    = pOldData->mWrap_;
        pTexData_->mFilter_  = pOldData->mFilter_;

        if (ONLY_POWER_OF_TWO)
        {
            pTexData_->uiRealWidth_  = pow(2.0f, ceil(log2((float)uiWidth)));
            pTexData_->uiRealHeight_ = pow(2.0f, ceil(log2((float)uiHeight)));
        }
        else
        {
            pTexData_->uiRealWidth_  = uiWidth;
            pTexData_->uiRealHeight_ = uiHeight;
        }

        if (pTexData_->uiRealWidth_ > MAXIMUM_SIZE || pTexData_->uiRealHeight_ > MAXIMUM_SIZE)
        {
            pTexData_ = pOldData;
            return false;
        }

        glDeleteTextures(1, &pTexData_->uiTextureHandle_);

        GLint iPreviousID;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &iPreviousID);

        glGenTextures(1, &pTexData_->uiTextureHandle_);

        glBindTexture(GL_TEXTURE_2D, pTexData_->uiTextureHandle_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
            pTexData_->uiRealWidth_, pTexData_->uiRealHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );

        switch (pTexData_->mWrap_)
        {
        case CLAMP :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            break;
        case REPEAT :
        default :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        }
        switch (pTexData_->mFilter_)
        {
        case LINEAR :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            break;
        case NONE :
        default :
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            break;
        }

        glBindTexture(GL_TEXTURE_2D, iPreviousID);
        return true;
    }
    else
    {
        pTexData_->uiWidth_  = uiWidth;
        pTexData_->uiHeight_ = uiHeight;
        return false;
    }
}

void material::update_texture()
{
    GLint iPreviousID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &iPreviousID);

    glBindTexture(GL_TEXTURE_2D, pTexData_->uiTextureHandle_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pTexData_->uiWidth_, pTexData_->uiHeight_,
        GL_RGBA, GL_UNSIGNED_BYTE, pTexData_->pData_.data()
    );

    glBindTexture(GL_TEXTURE_2D, iPreviousID);
}

void material::clear_cache_data_()
{
    pTexData_->pData_.clear();
}

uint material::get_handle_()
{
    return pTexData_->uiTextureHandle_;
}

void material::check_availability()
{
    ONLY_POWER_OF_TWO = !manager::is_gl_extension_supported("GL_ARB_texture_non_power_of_two");
    gui::out << "Note : non power of two textures are " << (ONLY_POWER_OF_TWO ? "not " : "") << "supported." << std::endl;
    int max;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
    MAXIMUM_SIZE = max;
    gui::out << "Note : maximum texture size is " << MAXIMUM_SIZE << "." << std::endl;
}
}
}
