 /* Copyright 2014 Tarnyko <tarnyko@tarnyko.net> */

#ifndef _LIBUSELESS_LOGIND_SD_LOGIN_H
#define _LIBUSELESS_LOGIND_SD_LOGIN_H

#ifdef __cplusplus
extern "C" {
#endif

int
sd_pid_get_session (pid_t pid, char **session);

int
sd_pid_get_owner_uid (pid_t pid, uid_t *uid);

int
sd_pid_get_unit (pid_t pid, char **unit);		/* NOTIMPL */

int
sd_pid_get_user_unit (pid_t pid, char **unit);		/* NOTIMPL */

int
sd_pid_get_machine_name (pid_t pid, char **name);	/* NOTIMPL */

int
sd_pid_get_slice (pid_t pid, char **name);		/* NOTIMPL ? */

int
sd_uid_is_on_seat (uid_t uid, int require_active, const char *seat);

int
sd_uid_get_sessions (uid_t uid, int require_active, char ***sessions);

int
sd_uid_get_seats (uid_t uid, int require_active, char ***seats);

int
sd_session_is_active (const char *session);

int
sd_session_get_state (const char *session, char **state);

int
sd_session_get_uid (const char *session, uid_t *uid);

int
sd_session_get_seat (const char *session, char **seat);

int
sd_session_get_service (const char *session, char **service);

int
sd_session_get_type (const char *session, char **type);

int
sd_session_get_class (const char *session, char **clazz);

int
sd_session_get_display (const char *session, char **display);	/* NOTIMPL ? */

int
sd_session_get_tty (const char *session, char **display);

int
sd_session_get_vt (const char *session, unsigned *vtnr);

int
sd_seat_get_active (const char *seat, char **session, uid_t *uid);

int
sd_seat_get_sessions (const char *seat, char ***sessions, uid_t **uid,
		      unsigned *n_uids);

int
sd_seat_can_multi_session (const char *seat);

int
sd_seat_can_tty (const char *seat);

int
sd_seat_can_graphical (const char *seat);

int
sd_get_seats (char ***seats);

int
sd_get_sessions (char ***sessions);

int
sd_get_uids (uid_t **users);

int
sd_get_machine_names (char ***machines);		/* NOTIMPL */

int
sd_get_sessions (char ***sessions);

int
sd_get_uids (uid_t **users);

int
sd_get_machine_names(char ***machines);			/* NOTIMPL */


typedef struct sd_login_monitor sd_login_monitor;

int
sd_login_monitor_new (const char *category, sd_login_monitor** ret);

sd_login_monitor *
sd_login_monitor_unref (sd_login_monitor *m);

int
sd_login_monitor_flush (sd_login_monitor *m);

int
sd_login_monitor_get_fd (sd_login_monitor *m);

int
sd_login_monitor_get_events (sd_login_monitor *m);

int
sd_login_monitor_get_timeout (sd_login_monitor *m, uint64_t *timeout_usec);

#ifdef __cplusplus
}
#endif

#endif /* _LIBUSELESS_LOGIND_SD_LOGIN_H */
