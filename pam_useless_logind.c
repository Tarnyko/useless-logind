#include <pwd.h>
#include <stdlib.h>			/* for atoi() */
#include <string.h>
#include <syslog.h>			/* for LOG_ERR... */

#include <security/pam_modules.h>	/* for pam_get_user() */
#include <security/pam_ext.h>		/* for pam_syslog() */


PAM_EXTERN int
pam_sm_open_session (pam_handle_t *handle,
		     int flags, int argc, const char *argv[])
{
	const char *service = NULL;
	const char *username = NULL;
	struct passwd *pw = NULL;
	const char *tty, *xdisplay = NULL;
	char *found = NULL;
	int res;

	/* ignore cron and systemd-user */
	pam_get_item (handle, PAM_SERVICE, (const void **) &service);
	if ((strcmp (service, "cron") == 0) ||
	    (strcmp (service, "systemd-user") == 0)
		return PAM_SUCCESS;

	/* get username and verify it really exists */
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

	/* every login manager that I know of fills in PAM_TTY
	 * with either the VT or the X11 display, and only in the
	 * latter case fills in PAM_XDISPLAY with the same value */
	pam_get_item (handle, PAM_TTY, (const void **) &tty);
	 /* begins with "tty" or contains ":" */
	if (((found = strstr (tty, "tty")) != NULL) &&
	    (found - tty == 0) && (strlen(found) == 4) {
		// TTY = yes
		// vtnr = atoi(found+3);
	} else if ((found = strstr (tty, ":")) != NULL)
		// X11
	}

	 /* these variables are supposed to be set by the login manager */
	const char *xdg_session_type, *xdg_session_class,
	           *xdg_session_desktop, *xdg_seat, *xdg_vtnr = NULL;

	// logind a un argument pour écraser ça
	xdg_session_type = pam_getenv (handle, "XDG_SESSION_TYPE");
	if (xdg_session_type == NULL)
		xdg_session_type = getenv ("XDG_SESSION_TYPE");
	// logind a un argument pour écraser ça
	xdg_session_class = pam_getenv (handle, "XDG_SESSION_CLASS");
	if (xdg_session_class == NULL)
		xdg_session_class = getenv ("XDG_SESSION_CLASS");

	xdg_session_desktop = pam_getenv (handle, "XDG_SESSION_DESKTOP");
	if (xdg_session_desktop == NULL)
		xdg_session_desktop = getenv ("XDG_SESSION_DESKTOP");

	xdg_seat = pam_getenv (handle, "XDG_SEAT");
	if (xdg_seat == NULL)
		xdg_seat = getenv ("XDG_SEAT");

	xdg_vtnr = pam_getenv (handle, "XDG_VTNR");
	if (xdg_vtnr == NULL)
		xdg_vtnr = getenv ("XDG_VTNR");



	// send message to useless-logind
	

	pam_get_item (handle, PAM_XDISPLAY, (const void **) &xdisplay);

	pam_syslog (handle, LOG_ERR, "PAM_XDISPLAY : %s", xdisplay);
	pam_syslog (handle, LOG_ERR, "PAM_TTY : %s", tty);

	const char *ruser, *rhost = NULL;
	pam_get_item (handle, PAM_SERVICE, (const void **) &service);
	pam_get_item (handle, PAM_RUSER, (const void **) &ruser);
	pam_get_item (handle, PAM_RHOST, (const void **) &rhost);
	pam_syslog (handle, LOG_ERR, "PAM_SERVICE : %s", service);
	pam_syslog (handle, LOG_ERR, "PAM_RUSER : %s", ruser);
	pam_syslog (handle, LOG_ERR, "PAM_RHOST : %s", rhost);

	return PAM_SUCCESS;
}


PAM_EXTERN int
pam_sm_close_session (pam_handle_t *handle,
		      int flags, int argc, const char *argv[])
{
	return PAM_SUCCESS;
}
