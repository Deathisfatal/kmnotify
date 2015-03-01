#include "xcbinterface.h"
#include <xkbcommon/xkbcommon-x11.h>
#include <xcb/xkb.h>
#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_xcb_error(xcb_generic_error_t * err) {
    fprintf(stderr, "Error Code: %d\nMajor: %d\nMinor: %d\n", err->error_code,
     err->major_code, err->minor_code);
}

void initialise_xkb(xcb_connection_t * connection) {
    xcb_xkb_use_extension_cookie_t cookie = xcb_xkb_use_extension(connection,
     XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION);
    xcb_generic_error_t * err           = NULL;
    xcb_xkb_use_extension_reply_t * reply = xcb_xkb_use_extension_reply(connection, cookie, &err);
     if (err) {
        print_xcb_error(err);
        fprintf(stderr, "Failed to initialise XKB extension!\n");
        return;
     } else if (!reply->supported) {
        fprintf(stderr, "Unsupported XKB version (We want %d %d, but X server has %d %d)",
                 XCB_XKB_MAJOR_VERSION, XCB_XKB_MINOR_VERSION,
                 reply->serverMajor, reply->serverMinor);
    }
    free(reply);
}

void register_events(xcb_connection_t * connection, xcb_window_t * root_window) {
    uint32_t mask               = XCB_CW_EVENT_MASK;
    uint32_t values[1]          = { XCB_EVENT_MASK_KEYMAP_STATE };
    xcb_void_cookie_t cookie    = xcb_change_window_attributes_checked(connection, 
            *root_window, mask, values);
    xcb_generic_error_t * err   = xcb_request_check(connection, cookie);
    if (err != NULL) {
        print_xcb_error(err);
        free(err);
    } else {
        printf("Successfully registered for events.\n");
    }
    xcb_flush(connection); /* Necessary? */
}

int16_t get_keyboard_id(xcb_connection_t * connection) {
    xcb_xkb_get_device_info_cookie_t cookie = xcb_xkb_get_device_info(connection, 
        XCB_XKB_ID_USE_CORE_KBD, 0, 0, 0, 0, 0, 0);
    xcb_generic_error_t * err           = NULL;
    xcb_xkb_get_device_info_reply_t * reply = xcb_xkb_get_device_info_reply(connection, cookie, &err);
    if (!reply) {
        printf("XXX");
    }
    if (err) {
        print_xcb_error(err);
        fprintf(stderr, "Failed to get core keyboard ID.\n");
        free(err);
        return -1;
    } else {
        uint8_t device_id = reply->deviceID;
        free(reply);
        return device_id;
    }
}

void get_keymap_name(xcb_connection_t * connection, char ** keymap_name) {
    xcb_query_keymap_cookie_t cookie    = xcb_query_keymap(connection);
    xcb_generic_error_t * err           = NULL;
    xcb_query_keymap_reply_t * reply    = xcb_query_keymap_reply(connection, 
            cookie, &err);
    int16_t keyboard_id = get_keyboard_id(connection);
    printf("%d\n", keyboard_id);
    if (err) {
        fprintf(stderr, "Failed to query keymap state.\n");
    }  else  {
        free(reply);
    }
    *keymap_name = "x";
}

xcb_connection_t * initialise_xcb( ) {
    return xcb_connect( NULL, NULL );
}

xcb_window_t get_root_window(xcb_connection_t * connection) {
    xcb_screen_t * screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    return screen->root;
}
