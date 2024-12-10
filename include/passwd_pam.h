#ifndef PASSWD_PAM_H
#define PASSWD_PAM_H

typedef struct _PasswdService PasswdService;

int setup_pam (PasswdService *passwdservice, GError **error);
#endif