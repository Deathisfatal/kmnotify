#include <xcb/xcb.h>
#include <xcb/xkb.h>
#include <stdio.h>
#include <stdlib.h>
#include <libnotify/notify.h> 
#include <string.h>
#define NOTIFY_TIMEOUT 1000

void show_notification(const char * message);

void register_events(xcb_connection_t * connection, xcb_window_t * root_window) {
    uint32_t mask               = XCB_CW_EVENT_MASK;
    uint32_t values[1]          = { XCB_EVENT_MASK_KEYMAP_STATE };
    xcb_void_cookie_t cookie    = xcb_change_window_attributes_checked(connection, 
            *root_window, mask, values);
    xcb_generic_error_t * err   = xcb_request_check(connection, cookie);
    if (err != NULL) {
        fprintf(stderr, "X11 error %d\n", err->error_code);
        free(err);
    } else {
        printf("Successfully registered for events.\n");
    }
    xcb_flush(connection);
}

void get_keymap_name(xcb_connection_t * connection, char ** keymap_name) {
    xcb_xkb_get_names_cookie_t cookie = xcb_xkb_get_names(connection, NULL,);
    xcb_query_keymap_cookie_t cookie    = xcb_query_keymap(connection);
    xcb_generic_error_t * err           = NULL;
    xcb_query_keymap_reply_t * reply    = xcb_query_keymap_reply(connection, 
            cookie, &err);
    if (err) {
        fprintf(stderr, "Failed to query keymap state.\n");
    }  else  {
        int i = 0;
        for (i = 0; i < 32; i++) {
            printf("%x ", reply->keys[i]);
        }
        printf("\n");
        free(reply);
    }
    *keymap_name = "x";
}

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
    free(keymap_name);
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

void initialise_libnotify() {
    gboolean init_result = notify_init("kmnotify");
    if (!init_result) {
        fprintf(stderr, "Failed to initialise libnotify!\n");
        return;
    }   
}

void show_notification(const char * message) {
    static NotifyNotification * notification = NULL;
    if (notification == NULL) {
        notification = notify_notification_new(NULL, NULL, NULL);
    }
    /* Close old notification and reuse */
    notify_notification_close(notification, NULL);
    notify_notification_update(notification, message, NULL, NULL);
    notify_notification_set_urgency(notification, NOTIFY_URGENCY_LOW);
    notify_notification_set_timeout(notification, NOTIFY_TIMEOUT);
    GError * err = NULL;
    gboolean show_result = notify_notification_show(notification, &err);
    if (!show_result) {
        fprintf(stderr, "ERROR: %s\n", err->message);
    }
}

void deinitialise_libnotify() {
    if (notify_is_initted()) {
        notify_uninit();
    }
}

int main(int argc, char * argv[]) {
    initialise_libnotify();
    xcb_screen_t        * screen        = NULL;
    xcb_connection_t    * connection    = NULL;
    connection = xcb_connect( NULL, NULL );
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    xcb_window_t root_window = screen->root;
    register_events(connection, &root_window);
    event_loop(connection);
    xcb_disconnect(connection);
    deinitialise_libnotify();
    return 0;
}

