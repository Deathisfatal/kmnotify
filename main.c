#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
//#include <libnotify/notify.h> 
void register_events(xcb_connection_t * connection, xcb_window_t * root_window) {
    uint32_t mask               = XCB_CW_EVENT_MASK;
    uint32_t values[1]          = { XCB_EVENT_MASK_KEYMAP_STATE };
    xcb_generic_error_t *err    = NULL;
    xcb_void_cookie_t ck;
    ck = xcb_change_window_attributes_checked(connection, *root_window, mask, values);
    err = xcb_request_check(connection, ck);
    if (err != NULL) {
        fprintf(stderr, "X11 error %d\n", err->error_code);
        free(err);
    } else {
        printf("Successfully registered for events.\n");
    }
    xcb_flush(connection);
}

void get_keymap_name(xcb_connection_t * connection, char ** keymap_name) {
    xcb_query_keymap_cookie_t cookie;
    xcb_query_keymap_reply_t * reply = NULL;
    cookie = xcb_query_keymap(connection);
    reply = xcb_query_keymap_reply(connection, cookie, NULL);
    if (reply) {
        int i = 0;
        for (i = 0; i < 32; i++) {
            printf("%x\n", reply->keys[i]);
        }
        free(reply);
    }
}

void handle_keymap_notify_event(xcb_connection_t * connection, xcb_generic_event_t * event) {
    xcb_keymap_notify_event_t * km_event    = (xcb_keymap_notify_event_t *) event;
    char * keymap_name = NULL;
    get_keymap_name(connection, &keymap_name);
    printf("Keymap change detected! New keymap: %s\n", keymap_name);
    free(keymap_name);
}

void event_loop(xcb_connection_t * connection) {
    xcb_generic_event_t * event     = NULL;
    printf("Waiting for events...\n");
    while ((event = xcb_wait_for_event(connection))) {
        uint32_t response_type = event->response_type & ~0x80;
        if (response_type == XCB_KEYMAP_NOTIFY) {
            handle_keymap_notify_event(connection, event);            
        }
        free(event);
    }
}

int main(int argc, char * argv[]) {
    xcb_screen_t        * screen        = NULL;
    xcb_connection_t    * connection    = NULL;
    connection = xcb_connect( NULL, NULL );
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    xcb_window_t root_window = screen->root;
    register_events(connection, &root_window);
    event_loop(connection);
    xcb_disconnect (connection);
    return 0;
}

