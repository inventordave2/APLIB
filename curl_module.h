#ifndef APLIB_CURL_MODULE_H
#define APLIB_CURL_MODULE_H

#include <stddef.h>
#include <curl/curl.h>
#include <curl/multi.h>

typedef struct aplib_curl_session aplib_curl_session;
typedef struct aplib_curl_websocket aplib_curl_websocket;

typedef struct aplib_curl_response {
	char *body;
	size_t body_size;
	char *headers;
	size_t headers_size;
	long status_code;
} aplib_curl_response;

typedef struct aplib_curl_request_options {
	const char *url;
	const char *method;
	const char *body;
	size_t body_size;
	const char *const *headers;
	size_t header_count;
	const char *username;
	const char *password;
	long http_auth;
	long timeout_ms;
	long connect_timeout_ms;
	int follow_redirects;
} aplib_curl_request_options;

CURLcode aplib_curl_init(void);
void aplib_curl_cleanup(void);

CURLcode aplib_curl_session_create(aplib_curl_session **session);
void aplib_curl_session_destroy(aplib_curl_session *session);
CURLcode aplib_curl_session_set_cookie_jar(aplib_curl_session *session, const char *path);
CURLcode aplib_curl_request(aplib_curl_session *session,
	const aplib_curl_request_options *options,
	aplib_curl_response *response);
void aplib_curl_response_free(aplib_curl_response *response);

CURLcode aplib_curl_websocket_open(const char *url,
	aplib_curl_session *session,
	const char *const *headers,
	size_t header_count,
	const char *username,
	const char *password,
	long http_auth,
	long connect_timeout_ms,
	aplib_curl_websocket **websocket,
	long *status_code);
CURLcode aplib_curl_websocket_send(aplib_curl_websocket *websocket,
	const void *data, size_t size, unsigned int flags, size_t *sent);
CURLcode aplib_curl_websocket_receive(aplib_curl_websocket *websocket,
	void *buffer, size_t capacity, size_t *received,
	unsigned int *flags, curl_off_t *bytes_left);
CURLMcode aplib_curl_websocket_wait_readable(aplib_curl_websocket *websocket,
	int timeout_ms, int *readable);
void aplib_curl_websocket_close(aplib_curl_websocket *websocket);

#endif