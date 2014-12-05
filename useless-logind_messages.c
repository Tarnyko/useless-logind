 /* Copyright 2014 Tarnyko <tarnyko@tarnyko.net> */

#include <stdlib.h>
#include <string.h>

extern void session_open_for_message  (const char *message);
extern void session_close_for_message (const char *message);


void
handle_messages (const char *msg_buffer)
{
	char *buffer, *field = NULL;
	const char *message = NULL;

	 /* a message shorter than 20 characters is invalid */
	if (strlen (msg_buffer) < 20)
		return;

	buffer = strdup (msg_buffer);
	field = strtok (buffer, "\n");
	if (field == NULL)
		goto handle_messages_end;

	if (strcmp (field, "session") == 0) {
		field = strtok (NULL, "\n");
		if (field == NULL)
			goto handle_messages_end;
		if (strcmp (field, "open") == 0) {
				message = buffer + 13;
				session_open_for_message (message);
		} else if (strcmp (field, "close") == 0) {
				message = buffer + 14;
				session_close_for_message (message);
		}
	}

handle_messages_end:
	free (buffer);
}


#if 0
int main (int argc, char *argv[])
{
	char *string;
	string = strdup ("session\nopen\nroot\ntty\nuser\nweston\nseat0\n2\n");

	handle_messages (string);

	free (string);

	return 0;
}
#endif
