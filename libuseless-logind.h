 /* Copyright 2014 Tarnyko <tarnyko@tarnyko.net> */

#ifndef _LIBUSELESS_LOGIND_H
#define _LIBUSELESS_LOGIND_H

#ifdef __cplusplus
extern "C" {
#endif

struct session;

int
sessions_count ();

struct session *
session_get_for_number (int num);

#ifdef __cplusplus
}
#endif

#endif /* _LIBUSELESS_LOGIND__H */
