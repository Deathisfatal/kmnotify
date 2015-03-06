/*******
 * kmnotify.c
 *******
 * isaac.true@gmail.com
 *
 * Source for main sections of code
 */ 

#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "notifications.h"
#include "xcbinterface.h"

void handle_keymap_notify_event(xcb_connection_t * connection, xcb_generic_event_t * event) {
//    xcb_keymap_notify_event_t * km_event    = (xcb_keymap_notify_event_t *) event;
    char * keymap_name = get_keymap_name(connection);
    show_notification(keymap_name);
    free(keymap_name);
}

void event_loop(xcb_connection_t * connection) {
    xcb_generic_event_t * event     = NULL;
    printf("Waiting for events...\n");
    while ((event = xcb_wait_for_event(connection))) {
        uint32_t response_type = event->response_type & ~0x80; /* is this bitmask necessary?*/
        if (response_type == XCB_KEYMAP_NOTIFY) {
            handle_keymap_notify_event(connection, event);            
        }
        free(event);
    }
}

int main(int argc, char * argv[]) {
    initialise_notifications();
    xcb_connection_t * connection = initialise_xcb();
    initialise_xkb(connection);
    register_events(connection);
    event_loop(connection);
    xcb_disconnect(connection);
    deinitialise_notifications();
    return 0;
}

