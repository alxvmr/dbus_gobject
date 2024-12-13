#ifndef PASSWD_PAM_H
#define PASSWD_PAM_H
#include "passwduser.h"

int setup_pam (PasswdUser *passwduser, GError **error);
#endif