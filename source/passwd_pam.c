#include <security/pam_appl.h>
#include "../include/passwduser.h"

#define	PASSWD_SERVICE	"passwd"

static void
failure(pam_handle_t * pamh, int retval, GError **error)
{
	if (pamh)
	{
        g_set_error_literal (error, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS, pam_strerror(pamh, retval));
		pam_end(pamh, retval);
	}
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
    gchar **passwd_user_fileds = passwd_user_get_fields_for_pam (user);
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
                answ = passwd_user_fileds[i];
                // if (message->msg_style == PAM_PROMPT_ECHO_ON) {
                //     answ = passwdservice->user_name;
                // }
                // else {
                //     answ = passwdservice->old_passwd;
                // }
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

    // for (int i = 0; passwd_service_fileds[i] != NULL; i++) {
    //     g_free (passwd_service_fileds[i]);
    // }
    // g_free (passwd_service_fileds);

    *response = resp;
    return code;
}

int
setup_pam (PasswdUser *user,
           GError     **error)
{
    g_assert (user != NULL);

    pam_handle_t *pamh = NULL;
    struct pam_conv conv = { non_interactive_conv, user };
    int retval;

    retval = pam_start (PASSWD_SERVICE, user->user_name, &conv, &pamh);
    if (retval != PAM_SUCCESS) {
        failure (pamh, retval, error);
        return retval;
    }

    retval = pam_chauthtok (pamh, 0);
    if (retval != PAM_SUCCESS) {
        failure (pamh, retval, error);
        return retval;
    }

    retval = pam_end (pamh, PAM_SUCCESS);
    if (retval != PAM_SUCCESS) {
        failure (pamh, retval, error);
        return retval;
    }

    return PAM_SUCCESS;
}

// int main (int argc, char *argv[]) {
//     GError *error = NULL;

//     if (argc != 4) {
//         g_set_error(&error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT, "Error: Not enough arguments");
//         g_printerr("%s\n", error->message);
//         g_error_free(error);
//         return 1;
//     }

//     // Создать новый экземпляр PasswdService

//     // GHashTable *user_data = NULL;
//     // gchar *user_name;
//     // gchar *old_password;
//     // gchar *new_password;

//     // g_hash_table_new (g_str_hash, g_str_equal);

//     // g_hash_table_insert (user_data, "user_name", argv[1]);
//     // g_hash_table_insert (user_data, "old_password", argv[2]);
//     // g_hash_table_insert (user_data, "new_password", argv[3]);


//     // g_hash_table_destroy (user_data);

//     return 0;
// }