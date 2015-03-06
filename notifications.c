/*******
 * notifications.c
 *******
 * isaac.true@gmail.com
 *
 * libnotify interface
 */ 

#include "notifications.h"
#include <libnotify/notify.h> 
#include <stdio.h>

void initialise_notifications() {
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
	    notify_notification_set_urgency(notification, NOTIFY_URGENCY_LOW);
	    notify_notification_set_timeout(notification, NOTIFY_TIMEOUT);
    }
    /* Close old notification and reuse */
    notify_notification_close(notification, NULL);
    notify_notification_update(notification, message, "Keyboard layout updated",NULL);
    GError * err = NULL;
    gboolean show_result = notify_notification_show(notification, &err);
    if (!show_result) {
        fprintf(stderr, "ERROR: %s\n", err->message);
    }
}

void deinitialise_notifications() {
    if (notify_is_initted()) {
        notify_uninit();
    }
}
