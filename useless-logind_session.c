 /* Copyright 2014 Tarnyko <tarnyko@tarnyko.net> */

#ifdef FREEBSD
#define _WITH_GETLINE
#elif LINUX
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>

struct session {
	char *user;
	char *type;
	char *class;
	char *desktop;
	char *seat;
	int vtnr;
};

#define SESSION_MAX   "100"
#define SESSION_PATH  "/var/run/useless-logind"

static int session_count = 0;

 /* ------------------------------------ */
 /* Internal functions */

#define SESSION_MAX_I  atoi(SESSION_MAX)

struct session *
session_from_file (char *file_path);

struct session *
session_create ()
{
	struct session *session;

	session = calloc (1, sizeof *session);
	session->user = NULL;
	session->type = NULL;
	session->class = NULL;
	session->desktop = NULL;
	session->seat = NULL;
	session->vtnr = -1;

	return session;
}

void
session_free (struct session *session)
{
	if (session == NULL)
		return;

	if (session->user)
		free (session->user);
	if (session->type)
		free (session->type);
	if (session->class)
		free (session->class);
	if (session->desktop)
		free (session->desktop);
	if (session->seat)
		free (session->seat);
	free (session);
}

int
session_verify (struct session *session)
{
	struct passwd *pwd = NULL;
	char *found = NULL;
	int ok = 0;

	if ((session == NULL) || 
	    (session->user == NULL) || (session->type == NULL) ||
	    (session->class == NULL) || (session->desktop == NULL) ||
	    (session->seat == NULL) || (session->vtnr == -1))
		return -1;

	pwd = getpwent ();
	while (pwd != NULL) {
		if (strcmp (pwd->pw_name, session->user) == 0) {
			ok = 1;
			break;
		}
		pwd = getpwent ();
	}
	endpwent ();
	if (!ok)
		return -1;

	if ((strcmp ("tty", session->type) != 0) &&
	    (strcmp ("x11", session->type) != 0) &&
	    (strcmp ("wayland", session->type) != 0) &&
	    (strcmp ("web", session->type) != 0) &&
	    (strcmp ("unspecified", session->type) != 0))
		return -1;

	if ((strcmp ("user", session->class) != 0) &&
	    (strcmp ("background", session->class) != 0))
		return -1;

	if (((found = strstr (session->seat, "seat")) == NULL) ||
	    (found - session->seat > 0) || (strlen(found) > 5) ||
	    (strlen(found) < 5) || !isdigit(found[4]))
		return -1;

	if (session->vtnr < 0 || session->vtnr > 9)
		return -1;

	return 0;
}

int
session_opened (struct session *session, int *num)
{
	struct session *temp_session = NULL;
	char *ref_file, *temp_file = NULL;
	int temp_n;

	ref_file = strdup (SESSION_PATH "/session-");
	temp_file = malloc (strlen(ref_file) + strlen(SESSION_MAX) + 1);

	for (temp_n = 1; temp_n < session_count + 1; temp_n++) {
		sprintf (temp_file, "%s%d", ref_file, temp_n);
		if (access (temp_file , F_OK) == 0) {
			temp_session = session_from_file (temp_file);
			if (temp_session != NULL &&
			    strcmp (temp_session->user, session->user) == 0) {
				 /* it exists... give number and return */
				session_free (temp_session);
				free (temp_file);
				free (ref_file);
				if (num != NULL)
					*num = temp_n;
				return 1;
			}
			session_free (temp_session);
		}
	}

	free (temp_file);
	free (ref_file);

	return 0;
}

void
cleanup_sessions (void)
{
	char *ref_file, *cur_file = NULL;
	int temp_n;

	ref_file = strdup (SESSION_PATH "/session-");
	cur_file = malloc (strlen(ref_file) + strlen(SESSION_MAX) + 1);

	for (temp_n = 1; temp_n < SESSION_MAX_I; temp_n++) {
		sprintf (cur_file, "%s%d", ref_file, temp_n);
		/* stop cleaning once we reach a missing session */
		if (access (cur_file , F_OK) != 0)
			break;
		unlink (cur_file);
	}

	free (cur_file);
	free (ref_file);
}

void
renumber_sessions (void)
{
	char *ref_file, *old_file, *new_file = NULL;
	int temp_n, i;

	ref_file = strdup (SESSION_PATH "/session-");
	old_file = malloc (strlen(ref_file) + strlen(SESSION_MAX) + 1);
	new_file = malloc (strlen(ref_file) + strlen(SESSION_MAX) + 1);

	for (temp_n = 1; temp_n < session_count + 1; temp_n++) {
		sprintf (old_file, "%s%d", ref_file, temp_n);
		if (access (old_file , F_OK) != 0) {
			for (i = temp_n+1; i < SESSION_MAX_I; i++) {
				sprintf (new_file, "%s%d", ref_file, i);
				if (access (new_file , F_OK) == 0) {
					 /* renumber file */
					rename (new_file, old_file);
					break;
				}
			}
		} else
			continue;
	}

	free (new_file);
	free (old_file);
	free (ref_file);
}

struct session *
session_from_message (const char *message)
{
	struct session *session = NULL;
	char *msg, *field = NULL;

	if (message == NULL)
		return NULL;

	msg = strdup (message);
	field = strtok (msg, "\n");
	if (field == NULL) {
		free (msg);
		return NULL;
	}

	/* TODO : check number of fields */

	session = session_create ();

	session->user = strdup (field);
	field = strtok (NULL, "\n");
	session->type = strdup (field);
	field = strtok (NULL, "\n");
	session->class = strdup (field);
	field = strtok (NULL, "\n");
	session->desktop = strdup (field);
	field = strtok (NULL, "\n");
	session->seat = strdup (field);
	field = strtok (NULL, "\n");
	session->vtnr = atoi (field);

	free (msg);

	return session;
}

struct session *
session_from_file (char *file_path)
{
	struct session *session = NULL;
	FILE *file = NULL;
	char *line, *found = NULL;
	char *id, *value = NULL;
	int id_len, value_len = 0;
	size_t len = 0;

	file = fopen (file_path, "r");
	if (file == NULL)
		return NULL;

	session = session_create ();

	line = malloc (1); /* for OpenBSD */
	id = malloc (1);

	while ((getline (&line, &len, file)) != -1) {
		if ((found = strchr (line, '=')) != NULL) {
			id_len = found-line;
			id = realloc (id, id_len+1);
			strncpy (id, line, id_len);
			id[id_len] = '\0';

			value_len = strlen(line)-strlen(id)-2;
			value = malloc (value_len+1);
			strncpy (value, found+1, value_len);
			value[value_len] = '\0';

			if (strcmp (id, "user") == 0)
				session->user = value;
			else if (strcmp (id, "type") == 0)
				session->type = value;
			else if (strcmp (id, "class") == 0)
				session->class = value;
			else if (strcmp (id, "desktop") == 0)
				session->desktop = value;
			else if (strcmp (id, "seat") == 0)
				session->seat = value;
			else if (strcmp (id, "vtnr") == 0)
				session->vtnr = atoi (value);
		}
	}

	free (id);
	free (line);
	fclose (file);	

	return session;
}

int
session_to_file (struct session *session, char *file_path)
{
	FILE *file = NULL;

	file = fopen (file_path, "w");

	fputs ("user=", file);
	fprintf (file, "%s\n", session->user);
	fputs ("type=", file);
	fprintf (file, "%s\n", session->type);
	fputs ("class=", file);
	fprintf (file, "%s\n", session->class);
	fputs ("desktop=", file);
	fprintf (file, "%s\n", session->desktop);
	fputs ("seat=", file);
	fprintf (file, "%s\n", session->seat);
	fputs ("vtnr=", file);
	fprintf (file, "%d\n", session->vtnr);

	fclose (file);

	return 0;
}


void
session_open_for_message (const char *message)
{
	struct session *session = NULL;
	char *session_file = NULL;

	session = session_from_message (message);

	 /* verify the session is valid and not already opened */
	if ((session == NULL) ||
	    (session_verify (session) == -1) ||
	    (session_opened (session, NULL) == 1) ||
	    session_count >= SESSION_MAX_I)
		return;

	 /* we will now create a new one */
	session_count++;

	session_file = strdup (SESSION_PATH "/session-");
	session_file = realloc (session_file, strlen (session_file) +
	                                      strlen (SESSION_MAX) + 1);
	sprintf (session_file, "%s%d", session_file, session_count);
	 /* delete exisiting leftover file */
	if (access (session_file, F_OK) == 0)
		unlink (session_file);
	session_to_file (session, session_file);

	printf ("SESSION_FILE : %s\n", session_file);

	free (session_file);
	session_free (session);	
}

void
session_close_for_message (const char *message)
{
	struct session *session = NULL;
	char *session_file = NULL;
	int session_number;

	session = session_from_message (message);

	 /* verify the session is valid and already opened */
	if ((session == NULL) ||
	    (session_verify (session) == -1) ||
	    (session_opened (session, &session_number) != 1))
		return;

	 /* we will close the existing one */
	session_file = strdup (SESSION_PATH "/session-");
	session_file = realloc (session_file, strlen (session_file) +
	                                      strlen (SESSION_MAX) + 1);
	sprintf (session_file, "%s%d", session_file, session_number);
	unlink (session_file);

	printf ("NUMBER : %d\n", session_number);

	session_count--;

	renumber_sessions ();

	free (session_file);
	session_free (session);
}


#if 0
int main (int argc, char *argv[])
{
	char *string, *string2;
	string = strdup ("root\ntty\nuser\nweston\nseat1\n2\n");
	string2 = strdup ("pulse\ntty\nuser\nweston\nseat2\n4\n");

	session_open_for_message (string);
	session_open_for_message (string2);
	session_close_for_message (string);
	session_close_for_message (string2);

	free (string);
	free (string2);

	return 0;
}
#endif

 /* ------------------------------------ */
 /* Public functions */

int
sessions_count ()
{
	return session_count;
}

struct session *
session_get_for_number (int num)
{
	struct session *session = NULL;
	char *session_file = NULL;

	session_file = strdup (SESSION_PATH "/session-");
	session_file = realloc (session_file, strlen (session_file) +
	                                      strlen (SESSION_MAX) + 1);
	sprintf (session_file, "%s%d", session_file, num);

	if (access (session_file, F_OK) != 0)
		return NULL;

	session = session_from_file (session_file);

	free (session_file);

	return session;
}

