#include "App_UiPages.h"

#include <stddef.h>

#include "../home/App_UiPageHome.h"

const app_ui_page_t *App_UiPages_Get(app_ui_page_id_t page_id)
{
    const app_ui_page_t *pages[] = {
        App_UiPageHome_Get(),
    };
    uint32_t i;

    for(i = 0; i < (uint32_t)(sizeof(pages) / sizeof(pages[0])); i++) {
        if(pages[i] != NULL && pages[i]->id == page_id) {
            return pages[i];
        }
    }
    return NULL;
}
