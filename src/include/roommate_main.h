#ifndef ROOMMATE_MAIN_H
#define ROOMMATE_MAIN_H

typedef void(*void_func)(void);

void roommate_app_begin(void_func connect_wifi, void_func setup_key_provisioning);

#endif