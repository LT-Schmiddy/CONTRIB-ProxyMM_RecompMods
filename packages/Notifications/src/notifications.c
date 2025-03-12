#include "recomp_api.h"
#include "recompui.h"
#include "z64extern.h"
#include "libc64/sprintf.h"

RecompuiContext context;
RecompuiResource root;
RecompuiResource container;
RecompuiColor notifBg = { 0, 0, 0, 0.7f * 255 };
RecompuiColor prefixTextColor = { 185, 125, 242, 255 };
RecompuiColor msgTextColor = { 255, 255, 255, 255 };
RecompuiColor suffixTextColor = { 23, 214, 232, 255 };

#define MAX_NOTIFS 15
int head = 0;
int count = 0;

typedef struct Notification {
    RecompuiResource element;
    int duration;
} Notification;

Notification notifications[MAX_NOTIFS];

RECOMP_CALLBACK("*", recomp_on_init) 
void OnRecompInit() {
    const float body_padding = 32.0f;
    const float modal_height = RECOMPUI_TOTAL_HEIGHT - (2 * body_padding);

    context = recompui_create_context();
    recompui_open_context(context);
    recompui_set_context_captures_input(context, 0);
    recompui_set_context_captures_mouse(context, 0);

    root = recompui_context_root(context);
    recompui_set_position(root, POSITION_ABSOLUTE);
    recompui_set_top(root, 0, UNIT_DP);
    recompui_set_right(root, 0, UNIT_DP);
    recompui_set_bottom(root, 0, UNIT_DP);
    recompui_set_left(root, 0, UNIT_DP);
    recompui_set_width_auto(root);
    recompui_set_height_auto(root);
    recompui_set_padding(root, body_padding, UNIT_DP);
    recompui_set_flex_direction(root, FLEX_DIRECTION_COLUMN);
    recompui_set_justify_content(root, JUSTIFY_CONTENT_CENTER);

    container = recompui_create_element(context, root);
    recompui_set_height(container, 100.0f, UNIT_PERCENT);
    recompui_set_flex_grow(container, 1.0f);
    recompui_set_display(container, DISPLAY_FLEX);
    recompui_set_justify_content(container, JUSTIFY_CONTENT_FLEX_END);
    recompui_set_flex_direction(container, FLEX_DIRECTION_COLUMN);
    recompui_set_gap(container, 16.0f, UNIT_DP);
    recompui_set_align_items(container, ALIGN_ITEMS_FLEX_END);

    recompui_close_context(context);
    recompui_show_context(context);
}

RECOMP_EXPORT void Notifications_Emit(const char* prefix, const char* msg, const char* suffix) {
    recompui_open_context(context);

    if (count == MAX_NOTIFS) {
        recompui_destroy_element(container, notifications[head].element);
        notifications[head].element = 0;
        head = (head + 1) % MAX_NOTIFS;
        count--;
    }

    int index = (head + count) % MAX_NOTIFS;

    notifications[index].element = recompui_create_element(context, container);
    recompui_set_padding(notifications[index].element, 16.0f, UNIT_DP);
    recompui_set_border_radius(notifications[index].element, 16.0f, UNIT_DP);
    recompui_set_background_color(notifications[index].element, &notifBg);
    recompui_set_display(notifications[index].element, DISPLAY_FLEX);
    recompui_set_gap(notifications[index].element, 8.0f, UNIT_DP);

    if (prefix[0] != '\0') {
        RecompuiResource prefixEl = recompui_create_label(context, notifications[index].element, prefix, LABELSTYLE_SMALL);
        recompui_set_color(prefixEl, &prefixTextColor);
    }
    if (msg[0] != '\0') {
        RecompuiResource msgEl = recompui_create_label(context, notifications[index].element, msg, LABELSTYLE_SMALL);
        recompui_set_color(msgEl, &msgTextColor);
    }
    if (suffix[0] != '\0') {
        RecompuiResource suffixEl = recompui_create_label(context, notifications[index].element, suffix, LABELSTYLE_SMALL);
        recompui_set_color(suffixEl, &suffixTextColor);
    }

    recompui_close_context(context);

    notifications[index].duration = 20 * 5;
    count++;
}

RECOMP_HOOK("GameState_Update")
void OnFrame(GameState* gameState) {
    for (int i = 0; i < count; i++) {
        int index = (head + i) % MAX_NOTIFS;
        if (notifications[index].duration > 0) {
            notifications[index].duration--;
            if (notifications[index].duration == 0) {
                recompui_open_context(context);
                recompui_destroy_element(container, notifications[index].element);
                recompui_close_context(context);

                // Shift head forward if this was the oldest notification
                if (index == head) {
                    head = (head + 1) % MAX_NOTIFS;
                    count--;
                }
            } else if (notifications[index].duration < 20 * 2) {
                recompui_open_context(context);
                // Lower opacity as the notification fades out
                recompui_set_opacity(notifications[index].element, (float)notifications[index].duration / (20 * 2));
                recompui_close_context(context);
            }
        }
    }
}
