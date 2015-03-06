/*******
 * xcbinterface.h
 *******
 * isaac.true@gmail.com
 *
 * xcb interface header
 */ 

#ifndef __XCBINTERFACE_H__
#define __XCBINTERFACE_H__

#include <xcb/xcb.h>

extern void initialise_xkb(xcb_connection_t * connection);
extern void register_events(xcb_connection_t * connection);
extern char * get_keymap_name(xcb_connection_t * connection);
extern xcb_connection_t * initialise_xcb();
extern xcb_window_t get_root_window(xcb_connection_t * connection);
#endif
