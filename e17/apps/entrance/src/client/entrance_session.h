#ifndef _ENTRANCE_SESSION_H
#define _ENTRANCE_SESSION_H

#include<Edje.h>
#include<Evas.h>
#include<Ecore.h>
#include<Ecore_X.h>
#include<Ecore_Evas.h>
#include<stdio.h>
#include<limits.h>
#include<string.h>
#include<unistd.h>
#include<syslog.h>
/**
 * @file entrance_session.h
 * @brief Struct Definitions and shared function declarations
 */
#include "entrance_auth.h"
#include "entrance_config.h"
#include "entrance_user.h"
#include "entrance_x_session.h"

/**
 * This is the handle to all of the data we've allocated in entrance
 */
struct _Entrance_Session
{
   char *display;               /* The display the session is running on */
   char *session;               /* the current session in context */
   Ecore_Evas *ee;              /* the ecore_evas */
   Evas_Object *edje;           /* the main theme edje */
   Entrance_Auth *auth;         /* encapsulated auth info */
   Entrance_Config *config;     /* configuration options */

   int authed;                  /* whether or not the user has authenticated
                                   * or not */
};

typedef struct _Entrance_Session Entrance_Session;

Entrance_Session *entrance_session_new(const char *config, char *display);
void entrance_session_ecore_evas_set(Entrance_Session * e, Ecore_Evas * ee);
void entrance_session_free(Entrance_Session * e);
void entrance_session_run(Entrance_Session * e);
int entrance_session_auth_user(Entrance_Session * e);
void entrance_session_user_reset(Entrance_Session * e);
void entrance_session_user_set(Entrance_Session * e, Entrance_User * user);
void entrance_session_user_session_default_set(Entrance_Session * e);
void entrance_session_setup_user_session(Entrance_Session * e);
void entrance_session_start_user_session(Entrance_Session * e);
void entrance_session_edje_object_set(Entrance_Session * e,
                                      Evas_Object * obj);
void entrance_session_list_add(Entrance_Session * e);
void entrance_session_user_list_add(Entrance_Session * e);
Entrance_X_Session *entrance_session_x_session_default_get(Entrance_Session *
                                                           e);
void entrance_session_x_session_set(Entrance_Session * e,
                                    Entrance_X_Session * exs);

#endif
