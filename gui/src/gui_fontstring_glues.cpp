#include "lxgui/gui_fontstring.hpp"
#include "lxgui/gui_layeredregion.hpp"
#include "lxgui/gui_out.hpp"

#include <lxgui/luapp_function.hpp>
#include <lxgui/utils_string.hpp>

namespace gui
{
void font_string::register_glue(utils::wptr<lua::state> pLua)
{
    pLua->reg<lua_font_string>();
}

lua_font_string::lua_font_string(lua_State* pLua) : lua_layered_region(pLua)
{
    pFontStringParent_ = dynamic_cast<font_string*>(pParent_);
    if (pParent_ && !pFontStringParent_)
        throw exception("lua_font_string", "Dynamic cast failed !");
}

int lua_font_string::_get_font(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:get_font", pLua, 1);

    mFunc.push(pFontStringParent_->get_font_name());

    return mFunc.on_return();
}

int lua_font_string::_get_justify_h(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:get_justify_h", pLua, 1);

    text::alignment mAlignment = pFontStringParent_->get_justify_h();
    std::string sAligment;

    switch (mAlignment)
    {
        case text::ALIGN_LEFT :   sAligment = "LEFT"; break;
        case text::ALIGN_CENTER : sAligment = "CENTER"; break;
        case text::ALIGN_RIGHT :  sAligment = "RIGHT"; break;
    }

    mFunc.push(sAligment);

    return mFunc.on_return();
}

int lua_font_string::_get_justify_v(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:get_justify_v", pLua, 1);

    text::vertical_alignment mAlignment = pFontStringParent_->get_justify_v();
    std::string sAligment;

    switch (mAlignment)
    {
        case text::ALIGN_TOP : sAligment = "TOP"; break;
        case text::ALIGN_MIDDLE : sAligment = "MIDDLE"; break;
        case text::ALIGN_BOTTOM : sAligment = "BOTTOM"; break;
    }

    mFunc.push(sAligment);

    return mFunc.on_return();
}

int lua_font_string::_get_shadow_color(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:get_shadow_color", pLua, 4);

    const color& mShadowColor = pFontStringParent_->get_shadow_color();

    mFunc.push(mShadowColor.r);
    mFunc.push(mShadowColor.g);
    mFunc.push(mShadowColor.b);
    mFunc.push(mShadowColor.a);

    return mFunc.on_return();
}

int lua_font_string::_get_shadow_offset(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:get_shadow_offset", pLua, 2);

    mFunc.push(pFontStringParent_->get_shadow_x_offset());
    mFunc.push(pFontStringParent_->get_shadow_y_offset());

    return mFunc.on_return();
}

int lua_font_string::_get_spacing(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:get_spacing", pLua, 1);

    mFunc.push(pFontStringParent_->get_spacing());

    return mFunc.on_return();
}

int lua_font_string::_get_text_color(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:get_text_color", pLua, 4);

    const color& mTextColor = pFontStringParent_->get_text_color();

    mFunc.push(mTextColor.r);
    mFunc.push(mTextColor.g);
    mFunc.push(mTextColor.b);
    mFunc.push(mTextColor.a);

    return mFunc.on_return();
}

int lua_font_string::_set_font(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:set_font", pLua);
    mFunc.add(0, "file", lua::TYPE_STRING);
    mFunc.add(1, "height", lua::TYPE_NUMBER);
    mFunc.add(3, "flags", lua::TYPE_STRING, true);

    if (mFunc.check())
    {
        pFontStringParent_->set_font(mFunc.get(0)->get_string(), mFunc.get(1)->get_number());
        if (mFunc.is_provided(2))
        {
            std::string sFlags = mFunc.get(2)->get_string();
            if (sFlags.find("OUTLINE") || sFlags.find("THICKOUTLINE"))
                pFontStringParent_->set_outlined(true);
            else if (sFlags.empty())
                pFontStringParent_->set_outlined(false);
            else
            {
                gui::out << gui::warning << mFunc.get_name() << " : "
                    << "Unknown flag list : \"" << sFlags <<"\"." << std::endl;
            }
        }
        else
            pFontStringParent_->set_outlined(false);
    }

    return mFunc.on_return();
}

int lua_font_string::_set_justify_h(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:set_justify_h", pLua);
    mFunc.add(0, "justify horizontal", lua::TYPE_STRING);

    if (mFunc.check())
    {
        std::string sJustifyH = mFunc.get(0)->get_string();
        if (sJustifyH == "LEFT")
            pFontStringParent_->set_justify_h(text::ALIGN_LEFT);
        else if (sJustifyH == "CENTER")
            pFontStringParent_->set_justify_h(text::ALIGN_CENTER);
        else if (sJustifyH == "RIGHT")
            pFontStringParent_->set_justify_h(text::ALIGN_RIGHT);
        else
        {
            gui::out << gui::warning << mFunc.get_name() << " : "
                << "Unknown justify behavior : \"" << sJustifyH << "\"." << std::endl;
        }
    }

    return mFunc.on_return();
}

int lua_font_string::_set_justify_v(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:set_justify_v", pLua);
    mFunc.add(0, "justify vertical", lua::TYPE_STRING);

    if (mFunc.check())
    {
        std::string sJustifyV = mFunc.get(0)->get_string();
        if (sJustifyV == "TOP")
            pFontStringParent_->set_justify_v(text::ALIGN_TOP);
        else if (sJustifyV == "MIDDLE")
            pFontStringParent_->set_justify_v(text::ALIGN_MIDDLE);
        else if (sJustifyV == "BOTTOM")
            pFontStringParent_->set_justify_v(text::ALIGN_BOTTOM);
        else
        {
            gui::out << gui::warning << mFunc.get_name() << " : "
                << "Unknown justify behavior : \"" << sJustifyV << "\"." << std::endl;
        }
    }

    return mFunc.on_return();
}

int lua_font_string::_set_shadow_color(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:set_shadow_color", pLua);
    mFunc.add(0, "red", lua::TYPE_NUMBER);
    mFunc.add(1, "green", lua::TYPE_NUMBER);
    mFunc.add(2, "blue", lua::TYPE_NUMBER);
    mFunc.add(3, "alpha", lua::TYPE_NUMBER, true);
    mFunc.new_param_set();
    mFunc.add(0, "color", lua::TYPE_STRING);

    if (mFunc.check())
    {
        color mColor;
        if (mFunc.get_param_set_rank() == 0)
        {
            if (mFunc.is_provided(3))
            {
                mColor = color(
                    mFunc.get(0)->get_number(),
                    mFunc.get(1)->get_number(),
                    mFunc.get(2)->get_number(),
                    mFunc.get(3)->get_number()
                );
            }
            else
            {
                mColor = color(
                    mFunc.get(0)->get_number(),
                    mFunc.get(1)->get_number(),
                    mFunc.get(2)->get_number()
                );
            }
        }
        else
            mColor = color(mFunc.get(0)->get_string());

        pFontStringParent_->set_shadow_color(mColor);
    }

    return mFunc.on_return();
}

int lua_font_string::_set_shadow_offset(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:set_shadow_offset", pLua);
    mFunc.add(0, "x offset", lua::TYPE_NUMBER);
    mFunc.add(1, "y offset", lua::TYPE_NUMBER);

    if (mFunc.check())
    {
        pFontStringParent_->set_shadow_offsets(
            int(mFunc.get(0)->get_number()),
            int(mFunc.get(1)->get_number())
        );
    }

    return mFunc.on_return();
}

int lua_font_string::_set_spacing(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:set_spacing", pLua);
    mFunc.add(0, "spacing", lua::TYPE_NUMBER);

    if (mFunc.check())
    {
        pFontStringParent_->set_spacing(mFunc.get(0)->get_number());
    }

    return mFunc.on_return();
}

int lua_font_string::_set_text_color(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:set_text_color", pLua);
    mFunc.add(0, "red", lua::TYPE_NUMBER);
    mFunc.add(1, "green", lua::TYPE_NUMBER);
    mFunc.add(2, "blue", lua::TYPE_NUMBER);
    mFunc.add(3, "alpha", lua::TYPE_NUMBER, true);
    mFunc.new_param_set();
    mFunc.add(0, "color", lua::TYPE_STRING);

    if (mFunc.check())
    {
        color mColor;
        if (mFunc.get_param_set_rank() == 0)
        {
            if (mFunc.is_provided(3))
            {
                mColor = color(
                    mFunc.get(0)->get_number(),
                    mFunc.get(1)->get_number(),
                    mFunc.get(2)->get_number(),
                    mFunc.get(3)->get_number()
                );
            }
            else
            {
                mColor = color(
                    mFunc.get(0)->get_number(),
                    mFunc.get(1)->get_number(),
                    mFunc.get(2)->get_number()
                );
            }
        }
        else
            mColor = color(mFunc.get(0)->get_string());

        pFontStringParent_->set_text_color(mColor);
    }

    return mFunc.on_return();
}

int lua_font_string::_can_non_space_wrap(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:can_non_space_wrap", pLua, 1);

    mFunc.push(pFontStringParent_->can_non_space_wrap());

    return mFunc.on_return();
}

int lua_font_string::_can_word_wrap(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:can_word_wrap", pLua, 1);

    mFunc.push(pFontStringParent_->can_word_wrap());

    return mFunc.on_return();
}

int lua_font_string::_enable_formatting(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:enable_formatting", pLua);
    mFunc.add(0, "formatting", lua::TYPE_BOOLEAN);

    if (mFunc.check())
    {
        pFontStringParent_->enable_formatting(mFunc.get(0)->get_bool());
    }

    return mFunc.on_return();
}

int lua_font_string::_get_string_height(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:get_string_height", pLua, 1);

    mFunc.push(pFontStringParent_->get_string_height());

    return mFunc.on_return();
}

int lua_font_string::_get_string_width(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:get_string_width", pLua, 1);

    mFunc.push(pFontStringParent_->get_string_width());

    return mFunc.on_return();
}

int lua_font_string::_get_text(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:get_text", pLua, 1);

    mFunc.push(pFontStringParent_->get_text());

    return mFunc.on_return();
}

int lua_font_string::_is_formatting_enabled(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:is_formatting_enabled", pLua, 1);

    mFunc.push(pFontStringParent_->is_formatting_enabled());

    return mFunc.on_return();
}

int lua_font_string::_set_non_space_wrap(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:set_non_space_wrap", pLua);
    mFunc.add(0, "can non space wrap", lua::TYPE_BOOLEAN);

    if (mFunc.check())
    {
        pFontStringParent_->set_non_space_wrap(mFunc.get(0)->get_bool());
    }

    return mFunc.on_return();
}

int lua_font_string::_set_word_wrap(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:set_word_wrap", pLua);
    mFunc.add(0, "can word wrap", lua::TYPE_BOOLEAN);
    mFunc.add(1, "add ellipsis", lua::TYPE_BOOLEAN, true);

    if (mFunc.check())
    {
        bool bEllipsis = true;
        if (mFunc.is_provided(1))
            bEllipsis = mFunc.get(1)->get_bool();

        pFontStringParent_->set_word_wrap(mFunc.get(0)->get_bool(), bEllipsis);
    }

    return mFunc.on_return();
}

int lua_font_string::_set_text(lua_State* pLua)
{
    if (!check_parent_())
        return 0;

    lua::function mFunc("FontString:set_text", pLua);
    mFunc.add(0, "text", lua::TYPE_STRING);
    mFunc.add(0, "number", lua::TYPE_NUMBER);
    mFunc.add(0, "bool", lua::TYPE_BOOLEAN);

    if (mFunc.check())
    {
        std::string sText;
        if (mFunc.get(0)->get_type() == lua::TYPE_STRING)
            sText = mFunc.get(0)->get_string();
        else if (mFunc.get(0)->get_type() == lua::TYPE_NUMBER)
            sText = utils::to_string(mFunc.get(0)->get_number());
        else if (mFunc.get(0)->get_type() == lua::TYPE_BOOLEAN)
            sText = utils::to_string(mFunc.get(0)->get_bool());

        pFontStringParent_->set_text(sText);
    }

    return mFunc.on_return();
}
}
