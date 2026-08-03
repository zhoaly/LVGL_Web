/**
 * @file App_UiPages.c
 * @brief 页面注册中心。
 */

#include "App_UiPages.h"

#include <stddef.h>

#include "../debug/App_UiPageDebug.h"
#include "../hid_hub/App_UiPageHidHub.h"
#include "../home/App_UiPageHome.h"
#include "../network/App_UiPageNetwork.h"
#include "../settings/App_UiPageSettings.h"
#include "../text/App_UiPageText.h"

const app_ui_page_t *App_UiPages_Get(app_ui_page_id_t page_id)
{
    const app_ui_page_t *pages[] = {
        App_UiPageHome_Get(),
        App_UiPageText_Get(),
        App_UiPageNetwork_Get(),
        App_UiPageHidHub_Get(),
        App_UiPageSettings_Get(),
        App_UiPageDebug_Get(),
    };
    uint32_t index;

    for(index = 0u;
        index < (uint32_t)(sizeof(pages) / sizeof(pages[0]));
        index++) {
        if(pages[index] != NULL && pages[index]->id == page_id) {
            return pages[index];
        }
    }
    return NULL;
}
