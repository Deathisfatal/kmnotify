/*******
 * notifications.h
 *******
 * isaac.true@gmail.com
 *
 * libnotify interface header
 */ 

#ifndef __NOTIFICATIONS_H__
#define __NOTIFICATIONS_H__

#define NOTIFY_TIMEOUT 100

extern void initialise_notifications();
extern void deinitialise_notifications();
extern void show_notification(const char * message);

#endif
