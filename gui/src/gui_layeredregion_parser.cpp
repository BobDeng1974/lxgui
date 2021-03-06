#include "lxgui/gui_layeredregion.hpp"
#include "lxgui/gui_frame.hpp"
#include "lxgui/gui_out.hpp"

#include <lxgui/xml_document.hpp>
#include <lxgui/utils_string.hpp>

namespace gui
{
void layered_region::parse_block(xml::block* pBlock)
{
    parse_attributes_(pBlock);

    parse_size_block_(pBlock);
    parse_anchor_block_(pBlock);
}

void layered_region::parse_attributes_(xml::block* pBlock)
{
    std::string sName = pBlock->get_attribute("name");
    if (!pManager_->check_uiobject_name(sName))
    {
        throw exception(pBlock->get_location(),
            "Cannot create an uiobject with an incorrect name. Skipped."
        );
    }

    bool bVirtual = utils::string_to_bool(pBlock->get_attribute("virtual"));
    frame* pFrameParent = dynamic_cast<frame*>(pParent_);
    if (!utils::has_no_content(sName))
    {
        if (bVirtual || (pFrameParent && pFrameParent->is_virtual()))
            set_virtual();

        set_name(sName);
    }
    else
    {
        throw exception(pBlock->get_location(),
            "Cannot create an uiobject with a blank name. Skipped."
        );
    }

    if (pManager_->get_uiobject_by_name(sName_))
    {
        throw exception(pBlock->get_location(),
            std::string(bVirtual ? "A virtual" : "An")+" object with the name "+
            sName_+" already exists. Skipped."
        );
    }

    pManager_->add_uiobject(this);

    if (!bVirtual_)
        create_glue();

    if (pFrameParent)
        pFrameParent->add_region(this);

    std::string sInheritance = pBlock->get_attribute("inherits");
    if (!utils::has_no_content(sInheritance))
    {
        std::vector<std::string> lObjects = utils::cut(sInheritance, ",");
        std::vector<std::string>::iterator iter;
        foreach (iter, lObjects)
        {
            utils::trim(*iter, ' ');
            uiobject* pObj = pManager_->get_uiobject_by_name(*iter, true);
            if (pObj)
            {
                if (is_object_type(pObj->get_object_type()))
                {
                    // Inherit from the other region
                    copy_from(pObj);
                }
                else
                {
                    gui::out << gui::warning << pBlock->get_location() << " : "
                        << "\"" << sName_ << "\" (" << "gui::" << lType_.back() << ") cannot inherit "
                        << "from \"" << *iter << "\" (" << pObj->get_object_type() << "). "
                        << "Inheritance skipped." << std::endl;
                }
            }
            else
            {
                gui::out << gui::warning <<  pBlock->get_location() << " : "
                    << "Cannot find inherited object \"" << *iter << "\". Inheritance skipped." << std::endl;
            }
        }
    }

    if ((pBlock->is_provided("hidden") || !bInherits_) &&
        (utils::string_to_bool(pBlock->get_attribute("hidden"))))
        hide();

    if ((pBlock->is_provided("setAllPoints") || !bInherits_) &&
        (utils::string_to_bool(pBlock->get_attribute("setAllPoints"))))
        set_all_points("$parent");
}
}
