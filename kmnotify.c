/*******
 * kmnotify.c
 *******
 * isaac@is.having.coffee
 *
 * Source for main sections of code
 */ 

#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "notifications.h"
#include "xcbinterface.h"
#include <signal.h>

static xcb_connection_t * connection = NULL;
static char * last_keymap_name = NULL;


void cleanup() {
    xcb_disconnect(connection);
    deinitialise_notifications();
    exit(0);
}

void handle_sigint(int dummy) {
    cleanup();
}

void handle_keymap_notify_event(xcb_connection_t * connection) {
    char * keymap_name = get_keymap_name(connection);
    /* Check if new keymap name is different to the previous. If it is different,
       replace last_keymap_name with the new keymap_name, free the old one, and show
       the notification. Otherwise, free the new name and do nothing.
    */
    if (last_keymap_name == NULL || strncmp(keymap_name, last_keymap_name, strlen(keymap_name))) {
        if (last_keymap_name != NULL) {
            free(last_keymap_name);
        }
        last_keymap_name = keymap_name;
        show_notification(keymap_name);
    } else {
        free(keymap_name);
    }
}

void event_loop(xcb_connection_t * connection) {
    xcb_generic_event_t * event     = NULL;
    printf("Waiting for events...\n");
    while ((event = xcb_wait_for_event(connection))) {
        uint32_t response_type = event->response_type & ~0x80; /* is this bitmask necessary?*/
        free(event);
        if (response_type == XCB_KEYMAP_NOTIFY) {
            handle_keymap_notify_event(connection);            
        }
    }
}

int main(int argc, char * argv[]) {
    signal(SIGINT, handle_sigint);
    initialise_notifications();
    connection = initialise_xcb();
    initialise_xkb(connection);
    last_keymap_name = get_keymap_name(connection);
    register_events(connection);
    event_loop(connection);
    cleanup();
    return 0;
}

