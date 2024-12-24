#include <security/pam_appl.h>
#include "../include/passwduser.h"
#include "../include/translate.h"
#include <stdio.h>
#include <json-glib/json-glib.h>

#define	PASSWD_SERVICE	"passwd"
#define PAM_OLDPASS 0
#define PAM_NEWPASS 1
#define PAM_SKIPASS 2

gchar *CONV_ERROR = NULL;

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
                g_free (CONV_ERROR);
                CONV_ERROR = g_strdup (message->msg);
                // g_printerr ("%s\n", message->msg);
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
setup_pam (PasswdUser *user, JsonObject *object)
{
    g_assert (user != NULL);

    pam_handle_t *pamh = NULL;
    struct pam_conv conv = { non_interactive_conv, user };
    int retval;

    retval = pam_start (PASSWD_SERVICE, user->user_name, &conv, &pamh);
    if (retval != PAM_SUCCESS) {
        json_object_set_string_member(object, "pam_start_error", get_translate_by_pam_retval(pamh, retval));
        g_printerr("%s\n", pam_strerror(pamh, retval));
        pam_end(pamh, retval);
        return retval;
    }

    retval = pam_chauthtok (pamh, 0);
    if (retval != PAM_SUCCESS) {
        json_object_set_string_member(object, "pam_chauthtok_error", get_translate_by_pam_retval(pamh, retval));
        if (CONV_ERROR != NULL) {
            json_object_set_string_member(object, "pam_conv_error", CONV_ERROR);
        }
        pam_end(pamh, retval);
        return retval;
    }

    retval = pam_end (pamh, PAM_SUCCESS);
    if (retval != PAM_SUCCESS) {
        json_object_set_string_member(object, "pam_end_error", get_translate_by_pam_retval(pamh, retval));
        g_printerr("%s\n", pam_strerror(pamh, retval));
        pam_end(pamh, retval);
        return retval;
    }

    return PAM_SUCCESS;
}

JsonNode *
init_json_node ()
{
    JsonNode *root = json_node_new (JSON_NODE_OBJECT);
    JsonObject *object = json_object_new ();

    json_object_set_member(object, "user_name", json_node_new(JSON_NODE_NULL));
    json_object_set_member(object, "main_error", json_node_new(JSON_NODE_NULL));
    json_object_set_member(object, "pam_start_error", json_node_new(JSON_NODE_NULL));
    json_object_set_member(object, "pam_chauthtok_error", json_node_new(JSON_NODE_NULL));
    json_object_set_member(object, "pam_conv_error", json_node_new(JSON_NODE_NULL));
    json_object_set_member(object, "pam_end_error", json_node_new(JSON_NODE_NULL));
    // TODO: добавлять пароли в json?

    json_node_init (root, JSON_NODE_OBJECT);
    json_node_set_object (root, object);

    return root;
}

gchar *
get_string_from_json_node (JsonNode *root)
{
    JsonGenerator *generator = json_generator_new();
    json_generator_set_root (generator, root);
    gchar *json_string = json_generator_to_data (generator, NULL);

    g_object_unref (generator);

    return json_string;
}

void
clear_json_object (JsonObject *object)
{
    GList *members = json_object_get_members (object);

    for (GList *l = members; l != NULL; l = l->next) {
        const gchar *key = (const gchar *) l->data;
        json_object_remove_member (object, key);
    }

    g_list_free (members);
    json_object_unref (object);
}

int main (int argc, char *argv[]) {
    PasswdUser *user = NULL;
    int res;
    
    JsonNode *root = init_json_node ();
    JsonObject *object = json_node_get_object(root);

    g_assert (object != NULL);

    if (argc != 4) {
        json_object_set_string_member(object, "main_error", "Not enough arguments");
        gchar *json_string = get_string_from_json_node (root);
        g_print ("%s\n", json_string);

        //g_printerr("%s\n", "Not enough arguments");

        g_free (json_string);
        clear_json_object (object);
        json_node_free (root);

        return 1;
    }

    user = passwd_user_new (argv[1], argv[2], argv[3]);
    json_object_set_string_member(object, "user_name", user->user_name);

    res = setup_pam (user, object);

    gchar *json_string = get_string_from_json_node (root);
    g_print ("%s\n", json_string);

    g_free (json_string);
    clear_json_object (object);
    json_node_free (root);

    g_free(CONV_ERROR);

    g_object_unref (user);
    return 0;
}