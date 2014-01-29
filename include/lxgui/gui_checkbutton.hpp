#ifndef GUI_CHECKBUTTON_HPP
#define GUI_CHECKBUTTON_HPP

#include <lxgui/utils.hpp>
#include "lxgui/gui_button.hpp"

namespace gui
{
    /// A button with two additional states : checked and unchecked
    /** This widget works exactly like a classic Button, but is has two
    *   additional special textures for the check sign.
    */
    class check_button : public button
    {
    public :

        /// Constructor.
        explicit check_button(manager* pManager);

        /// Destructor.
        virtual ~check_button();

        /// Prints all relevant information about this widget in a string.
        /** \param sTab The offset to give to all lines
        *   \return All relevant information about this widget
        */
        virtual std::string serialize(const std::string& sTab) const;

        /// Copies an uiobject's parameters into this CheckButton (inheritance).
        /** \param pObj The uiobject to copy
        */
        virtual void copy_from(uiobject* pObj);

        /// Checks this button.
        virtual void check();

        /// UnChecks this button.
        virtual void uncheck();

        /// Disables this CheckButton.
        /** \note A disabled button doesn't receive any input.
        */
        virtual void disable();

        /// Enables this CheckButton.
        virtual void enable();

        /// Releases this CheckButton.
        /** \note This function only has a visual impact :
        *         the OnClick() handler is not called.
        */
        virtual void release();

        /// Checks if this CheckButton is checked :)
        /** \return ... well you get my point.
        */
        bool is_checked();

        /// Returns this button's checked texture.
        /** \return This button's checked texture
        */
        texture* get_checked_texture();

        /// Returns this button's disabled checked texture.
        /** \return This button's disabled checked texture
        */
        texture* get_disabled_checked_texture();

        /// Sets this button's checked texture.
        /** \param pTexture The new texture
        */
        void set_checked_texture(texture* pTexture);

        /// Sets this button's disabled checked texture.
        /** \param pTexture The new texture
        */
        void set_disabled_checked_texture(texture* pTexture);

        /// Returns this widget's Lua glue.
        virtual void create_glue();

        /// Parses data from an xml::block.
        /** \param pBlock The Checkbutton's xml::block
        */
        virtual void parse_block(xml::block* pBlock);

        /// Registers this widget to the provided lua::state
        static void register_glue(utils::wptr<lua::state> pLua);

        #ifndef NO_CPP11_CONSTEXPR
        static constexpr const char* CLASS_NAME = "CheckButton";
        #else
        static const char* CLASS_NAME;
        #endif

    protected :

        texture* create_checked_texture_();
        texture* create_disabled_checked_texture_();

        bool bChecked_;

        texture* pCheckedTexture_;
        texture* pDisabledCheckedTexture_;

    };

    /** \cond NOT_REMOVE_FROM_DOC
    */

    class lua_check_button : public lua_button
    {
    public :

        explicit lua_check_button(lua_State* pLua);

        // Glues
        int _is_checked(lua_State*);
        int _get_checked_texture(lua_State*);
        int _get_disabled_checked_texture(lua_State*);
        int _set_checked(lua_State*);
        int _set_checked_texture(lua_State*);
        int _set_disabled_checked_texture(lua_State*);

        static const char className[];
        static const char* classList[];
        static Lunar<lua_check_button>::RegType methods[];

    protected :

        check_button* pCheckButtonParent_;
    };

    /** \endcond
    */
}

#endif