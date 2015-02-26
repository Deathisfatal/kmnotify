#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "notifications.h"
#include "xcbinterface.h"

void handle_keymap_notify_event(xcb_connection_t * connection, xcb_generic_event_t * event) {
//    xcb_keymap_notify_event_t * km_event    = (xcb_keymap_notify_event_t *) event;
    char * keymap_name = NULL;
    get_keymap_name(connection, &keymap_name);
//    printf("Keymap change detected! New keymap: %s\n", keymap_name);
    char * base_text = "New keyboard layout: ";
    size_t text_length = strlen(keymap_name) + strlen(base_text) + 1;
    char * notification_text = (char *) calloc(text_length, sizeof(char));
    sprintf(notification_text, "%s%s", base_text, keymap_name);
    show_notification("New keyboard layout:");
    //free(keymap_name);
    free(notification_text);
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
    xcb_screen_t        * screen        = NULL;
    xcb_connection_t    * connection    = NULL;
    connection = xcb_connect( NULL, NULL );
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    xcb_window_t root_window = screen->root;
    initialise_xkb(connection);
    register_events(connection, &root_window);
    event_loop(connection);
    xcb_disconnect(connection);
    deinitialise_notifications();
    return 0;
}

