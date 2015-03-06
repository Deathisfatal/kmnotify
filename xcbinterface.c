/*******
 * xcbinterface.c
 *******
 * isaac.true@gmail.com
 *
 * xcb interface
 */ 

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

void register_events(xcb_connection_t * connection) {
	xcb_window_t root_window = get_root_window(connection);
    uint32_t mask               = XCB_CW_EVENT_MASK;
    uint32_t values[1]          = { XCB_EVENT_MASK_KEYMAP_STATE };
    xcb_void_cookie_t cookie    = xcb_change_window_attributes_checked(connection, 
            root_window, mask, values);
    xcb_generic_error_t * err   = xcb_request_check(connection, cookie);
    if (err != NULL) {
        print_xcb_error(err);
        free(err);
    } else {
        printf("Successfully registered for events.\n");
    }
    xcb_flush(connection); /* Necessary? */
}

xkb_layout_index_t get_active_layout_index(xcb_connection_t * connection, int32_t keyboard_id, struct xkb_keymap * keymap) {
	struct xkb_state * state = xkb_x11_state_new_from_device(keymap, connection, keyboard_id);
	xkb_layout_index_t num_layouts = xkb_keymap_num_layouts (keymap);
	xkb_layout_index_t index = 0;
	for (xkb_layout_index_t i = 0; i < num_layouts; i++) {
		int is_active = xkb_state_layout_index_is_active(state, i, XKB_STATE_LAYOUT_EFFECTIVE);
		if (is_active == 1) {
			index = i;
			break;
		}
	}
	xkb_state_unref(state);
	return index;
}


char * get_keymap_name(xcb_connection_t * connection) {
	struct xkb_context * context = xkb_context_new(0);
    int32_t keyboard_id = xkb_x11_get_core_keyboard_device_id(connection);
    struct xkb_keymap * keymap = xkb_x11_keymap_new_from_device(context, connection, keyboard_id,0);
	xkb_layout_index_t active_layout = get_active_layout_index(connection, keyboard_id, keymap); 
    const char * keymap_name = xkb_keymap_layout_get_name(keymap, active_layout);
    char * return_name = (char *)calloc(strlen(keymap_name), sizeof(char));
    strcpy(return_name, keymap_name);
    xkb_context_unref(context);
    xkb_keymap_unref(keymap);
    return return_name;
}

xcb_connection_t * initialise_xcb( ) {
    return xcb_connect( NULL, NULL );
}

xcb_window_t get_root_window(xcb_connection_t * connection) {
    xcb_screen_t * screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    return screen->root;
}
