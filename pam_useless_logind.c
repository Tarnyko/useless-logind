#include <pwd.h>
#include <string.h>

#include <security/pam_modules.h>	/* for pam_get_user() */
#include <security/pam_ext.h>		/* for pam_syslog() */


_public_ PAM_EXTERN int
pam_useless_logind_open (pam_handle_t *handle,
			 int flags, int argc, const char *argv[])
{
	const char *service = NULL;
	const char *username = NULL;
	struct passwd *pw = NULL;
	int res;

	res = pam_get_user (handle, &username, NULL);
	if (res != PAM_SUCCESS) {
		pam_syslog (handle, LOG_ERR, "Failed to get username.");
		return res;
	}

	pw = pam_modutil_getpwnam (handle, username);
	if (!pw) {
		pam_syslog (handle, LOG_ERR, "Failed to get user details.");
		return PAM_USER_UNKNOWN;
	}

	return 0;
}
