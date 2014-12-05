 /* Copyright 2014 Tarnyko <tarnyko@tarnyko.net> */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/var/run/useless-logind"
#define SOCKET      SOCKET_PATH "/sock"

extern void cleanup_sessions (void);
extern void handle_messages (const char *msg_buffer);


static void
signal_handler (int signal, siginfo_t *sig_info, void *ctx)
{
	fprintf (stderr, "Closing socket and sessions...\n");

	if (access (SOCKET, F_OK) == 0)
		unlink (SOCKET);
	cleanup_sessions();

	/* if (signal == SIGHUP)
	reload config and restart */
}

int
setup_signals ()
{
	struct sigaction sig;
	sigset_t sig_mask;

	sigemptyset (&sig_mask);
	sigaddset (&sig_mask, SIGINT);
	sigaddset (&sig_mask, SIGTERM);
	sigaddset (&sig_mask, SIGHUP);

	sig.sa_mask = sig_mask;
	sig.sa_sigaction = signal_handler;
	sig.sa_flags = SA_SIGINFO;

	if (sigaction (SIGINT, &sig, NULL) == -1)
		return -1;
	if (sigaction (SIGTERM, &sig, NULL) == -1)
		return -1;
	if (sigaction (SIGHUP, &sig, NULL) == -1)
		return -1;

	return 0;
}

int
setup_socket_path ()
{
	if ((mkdir (SOCKET_PATH, 0755) == -1) && (errno != EEXIST)) {
		fprintf(stderr, "Could not create runtime directory \"%s\"\n", SOCKET_PATH);
		return -1;
	}

	return 0;
}

int
setup_listening_socket ()
{
	int sock_fd = -1;
	struct sockaddr_un sock_addr;

	 /* delete a previous socket which might be a leftover */
	if (access (SOCKET, F_OK) == 0)
		unlink (SOCKET);

	sock_fd = socket (AF_UNIX, SOCK_STREAM, 0);
	if (sock_fd == -1) {
		fprintf (stderr, "Could not create socket\n");
		goto listening_socket_err;
	}

	sock_addr.sun_family = AF_UNIX;
	strncpy (sock_addr.sun_path, SOCKET, sizeof (sock_addr.sun_path) - 1);
	if (bind (sock_fd, (struct sockaddr *) &sock_addr, sizeof (sock_addr)) == -1) {
		fprintf (stderr, "Could not bind socket to \"%s\"\n", SOCKET);
		goto listening_socket_err;
	}

	if (listen (sock_fd, 1) == -1) {
		fprintf (stderr, "Could not start listening on socket\n");
		goto listening_socket_err;
	}

	return sock_fd;

listening_socket_err:
	if (access (SOCKET, F_OK) == 0)
		unlink (SOCKET);
	if (sock_fd != -1)
		close (sock_fd);

	return -1;
}

int
listen_to_socket (int sock_fd)
{
	int msg_fd, msg_size = -1;
	char msg_buffer[1024];

	msg_fd = accept (sock_fd, NULL, 0);
	if (msg_fd == -1) {
		fprintf(stderr, "Could not listen to socket. Exiting...\n");
		goto listen_to_socket_err;
	}

	do {
		msg_size = read (msg_fd, msg_buffer, 1024);
	} while (msg_size > 0);

	 /* treat messages here */
	handle_messages (msg_buffer);

	return 0;

listen_to_socket_err:
	if (access (SOCKET, F_OK) == 0)
		unlink (SOCKET);

	return -1;
}


int
main (int argc, char *argv[])
{
	int sock_fd;

	if (getuid() != 0) {
		fprintf(stderr, "useless-logind MUST be run as root ! Exiting...\n");
		return -1;
	}

	if (setup_signals() == -1) {
		fprintf(stderr, "Could not setup signals. Exiting...\n");
		return -1;
	}

	if (setup_socket_path() == -1) {
		fprintf(stderr, "Could not create socket directory. Exiting...\n");
		return -1;
	}

	sock_fd = setup_listening_socket();
	if (sock_fd == -1) {
		fprintf(stderr, "Could not setup listening socket. Exiting...\n");
		return -1;
	}

	cleanup_sessions();

	if (listen_to_socket (sock_fd) == -1)
		return -1;

	return 0;
}
