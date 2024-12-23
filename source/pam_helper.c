#include <security/pam_appl.h>
#include "../include/passwduser.h"
#include <stdio.h>

#define	PASSWD_SERVICE	"passwd"
#define PAM_OLDPASS 0
#define PAM_NEWPASS 1
#define PAM_SKIPASS 2

static inline int getstate(const char *msg) {
    /* Interpret possible PAM messages (not including errors) */
    if (!strcmp(msg, "Current Password: "))
        return PAM_OLDPASS;

    if (!strcmp(msg, "New Password: "))
        return PAM_NEWPASS;
    if (!strcmp(msg, "Reenter new Password: "))
        return PAM_NEWPASS;

    return PAM_SKIPASS;
}

int
non_interactive_conv (int                        num_msg,
                      const struct pam_message **msgm,
                      struct pam_response      **response,
                      void                      *appdata_ptr)
{
    PasswdUser *user = (PasswdUser *) appdata_ptr;
    g_assert (user != NULL);

    struct pam_response *resp = NULL;
    const struct pam_message *message;
    const gchar *answ = NULL;
    guint size_answ;

    resp = malloc(sizeof(struct pam_response) * num_msg);
    if (resp == NULL) {
        return PAM_CONV_ERR;
    }

    for (int i = 0; i < num_msg; i++) {
        answ = NULL;
        message = msgm[i];

        switch (message->msg_style) {
            case PAM_TEXT_INFO:
            case PAM_ERROR_MSG:
                g_printerr("%s\n", message->msg);
                break;
            case PAM_PROMPT_ECHO_ON:
            case PAM_PROMPT_ECHO_OFF:
                switch (getstate(message->msg)) {
                    case PAM_OLDPASS:
                        answ = g_strdup (user->old_passwd);
                        break;
                    case PAM_NEWPASS:
                        answ = g_strdup (user->new_passwd);
                        break;
                    case PAM_SKIPASS:
                        answ = NULL;
                        break;
                    default:
                        // TODO: добавить лог о неизвестном сообщении
                        break;
                }
                if ((answ != NULL) && (answ[0] != '\0')) {
                    resp[i].resp = g_strdup (answ);

                    if (resp[i].resp == NULL) {
                        resp[i].resp_retcode = PAM_BUF_ERR;
                    }
                    else {
                        resp[i].resp_retcode = PAM_SUCCESS;
                    }
                }
                else {
                    resp[i].resp_retcode = PAM_CONV_ERR;
                    return PAM_CONV_ERR;
                }
                break;
            default:
                break;
        }
    }

    *response = resp;
    return PAM_SUCCESS;
}

int
setup_pam (PasswdUser *user)
{
    g_assert (user != NULL);

    pam_handle_t *pamh = NULL;
    struct pam_conv conv = { non_interactive_conv, user };
    int retval;

    retval = pam_start (PASSWD_SERVICE, user->user_name, &conv, &pamh);
    if (retval != PAM_SUCCESS) {
        g_printerr("%s\n", pam_strerror(pamh, retval));
        pam_end(pamh, retval);
        return retval;
    }

    retval = pam_chauthtok (pamh, 0);
    if (retval != PAM_SUCCESS) {
        //fprintf(stderr, "%s\n", pam_strerror(pamh, retval));
        pam_end(pamh, retval);
        return retval;
    }

    retval = pam_end (pamh, PAM_SUCCESS);
    if (retval != PAM_SUCCESS) {
        g_printerr("%s\n", pam_strerror(pamh, retval));
        pam_end(pamh, retval);
        return retval;
    }

    pam_end(pamh, retval);

    return PAM_SUCCESS;
}

int main (int argc, char *argv[]) {
    PasswdUser *user = NULL;
    int res;

    if (argc != 4) {
        g_printerr("%s\n", "Not enough arguments");
        return 1;
    }

    user = passwd_user_new (argv[1], argv[2], argv[3]);
    res = setup_pam (user);

    g_object_unref (user);
    return 0;
}