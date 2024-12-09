#include <security/pam_appl.h>
#include "../include/passwdservice.h"

#define	PASSWD_SERVICE	"passwd"

static void
__attribute__ ((__noreturn__))
failure(pam_handle_t * pamh, int retval)
{
	if (pamh)
	{
		g_printerr("passwd: %s.\n", pam_strerror(pamh, retval));
		pam_end(pamh, retval);
	}
    exit(EXIT_FAILURE);
}

int
non_interactive_conv (int                        num_msg,
                      const struct pam_message **msgm,
                      struct pam_response      **response,
                      void                      *appdata_ptr)
{
    PasswdService *passwdservice = (PasswdService *) appdata_ptr;
    g_assert (passwdservice != NULL);

    struct pam_response *resp = NULL;
    const struct pam_message *message;
    gchar **passwd_service_fileds = passwd_service_get_fields (passwdservice);
    const gchar *answ = NULL;
    guint size_answ, code;

    for (int i = 0; i < num_msg; i++) {
        message = &((*msgm)[i]);
        message = msgm[i];

        switch (message->msg_style) {
            case PAM_TEXT_INFO:
            case PAM_ERROR_MSG:
                break;
            case PAM_PROMPT_ECHO_ON:
            case PAM_PROMPT_ECHO_OFF:
                if (message->msg_style == PAM_PROMPT_ECHO_ON) {
                    answ = passwdservice->user_name;
                }
                else {
                    answ = passwdservice->old_passwd;
                }
                if ((answ != NULL) && (answ[0] != '\0')) {
                    size_answ = strlen (answ);
                    resp[i].resp = malloc(size_answ + 1);
                    if (resp[i].resp == NULL) {
                        resp[i].resp_retcode = PAM_BUF_ERR;
                    }
                    else {
                        memcpy(resp[i].resp, answ, size_answ);
                        resp[i].resp[size_answ] = '\0';
                        resp[i].resp_retcode = PAM_SUCCESS;
                    }
                }
                else {
                    resp[i].resp_retcode = PAM_CONV_ERR;
                    code = PAM_CONV_ERR;
                }
                break;
            default:
                break;
        }
    }

    for (int i = 0; passwd_service_fileds[i] != NULL; i++) {
        g_free (passwd_service_fileds[i]);
    }
    g_free (passwd_service_fileds);

    *response = resp;
    return code;
}

int
setup_pam (PasswdService *passwdservice)
{
    g_assert (passwdservice != NULL);

    pam_handle_t *pamh = NULL;
    struct pam_conv conv = { non_interactive_conv, passwdservice };
    int retval;

    retval = pam_start (PASSWD_SERVICE, passwdservice->user_name, &conv, &pamh);
    if (retval != PAM_SUCCESS) {
         failure (pamh, retval);
    }

    retval = pam_chauthtok (pamh, 0);
    if (retval != PAM_SUCCESS) {
        failure (pamh, retval);
    }

    retval = pam_end (pamh, PAM_SUCCESS);
    if (retval != PAM_SUCCESS) {
        failure (pamh, retval);
    }

    return PAM_SUCCESS;
}