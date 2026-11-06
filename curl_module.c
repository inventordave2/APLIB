#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "curl_module.h"
#include <curl/websockets.h>

struct aplib_curl_session {
	CURL *easy;
	CURLSH *share;
	char *cookie_jar;
};

struct aplib_curl_websocket {
	CURL *easy;
	struct curl_slist *headers;
};

struct aplib_curl_buffer {
	char *data;
	size_t size;
	int allocation_failed;
};

static size_t aplib_curl_append(char *data, size_t size, size_t count, void *userdata) {
	struct aplib_curl_buffer *buffer = (struct aplib_curl_buffer *)userdata;
	size_t bytes;
	char *resized;

	if (size != 0 && count > SIZE_MAX / size)
		goto allocation_error;

	bytes = size * count;
	if (bytes > SIZE_MAX - buffer->size - 1)
		goto allocation_error;

	resized = (char *)realloc(buffer->data, buffer->size + bytes + 1);
	if (resized == NULL)
		goto allocation_error;

	buffer->data = resized;
	memcpy(buffer->data + buffer->size, data, bytes);
	buffer->size += bytes;
	buffer->data[buffer->size] = '\0';
	return bytes;

allocation_error:
	buffer->allocation_failed = 1;
	return 0;
}

static char *aplib_curl_copy_string(const char *value) {
	size_t size;
	char *copy;

	if (value == NULL)
		return NULL;

	size = strlen(value) + 1;
	copy = (char *)malloc(size);
	if (copy != NULL)
		memcpy(copy, value, size);
	return copy;
}

static CURLcode aplib_curl_make_headers(const char *const *headers,
	size_t header_count, struct curl_slist **list) {
	size_t i;

	*list = NULL;
	if (header_count != 0 && headers == NULL)
		return CURLE_BAD_FUNCTION_ARGUMENT;

	for (i = 0; i < header_count; ++i) {
		struct curl_slist *updated;
		if (headers[i] == NULL)
			return CURLE_BAD_FUNCTION_ARGUMENT;
		updated = curl_slist_append(*list, headers[i]);
		if (updated == NULL)
			return CURLE_OUT_OF_MEMORY;
		*list = updated;
	}
	return CURLE_OK;
}

static CURLcode aplib_curl_set_auth(CURL *easy, const char *username,
	const char *password, long http_auth) {
	CURLcode result;

	if (username == NULL && password == NULL)
		return CURLE_OK;

	result = curl_easy_setopt(easy, CURLOPT_USERNAME, username == NULL ? "" : username);
	if (result == CURLE_OK)
		result = curl_easy_setopt(easy, CURLOPT_PASSWORD, password == NULL ? "" : password);
	if (result == CURLE_OK)
		result = curl_easy_setopt(easy, CURLOPT_HTTPAUTH,
			http_auth == 0 ? (long)CURLAUTH_BASIC : http_auth);
	return result;
}

CURLcode aplib_curl_init(void) {
	return curl_global_init(CURL_GLOBAL_DEFAULT);
}

void aplib_curl_cleanup(void) {
	curl_global_cleanup();
}

CURLcode aplib_curl_session_create(aplib_curl_session **session) {
	aplib_curl_session *created;

	if (session == NULL)
		return CURLE_BAD_FUNCTION_ARGUMENT;
	*session = NULL;

	created = (aplib_curl_session *)calloc(1, sizeof(*created));
	if (created == NULL)
		return CURLE_OUT_OF_MEMORY;

	created->easy = curl_easy_init();
	if (created->easy == NULL) {
		free(created);
		return CURLE_FAILED_INIT;
	}
	created->share = curl_share_init();
	if (created->share == NULL) {
		curl_easy_cleanup(created->easy);
		free(created);
		return CURLE_FAILED_INIT;
	}
	if (curl_share_setopt(created->share, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE) != CURLSHE_OK ||
		curl_easy_setopt(created->easy, CURLOPT_SHARE, created->share) != CURLE_OK) {
		curl_easy_cleanup(created->easy);
		curl_share_cleanup(created->share);
		free(created);
		return CURLE_FAILED_INIT;
	}

	*session = created;
	return CURLE_OK;
}

void aplib_curl_session_destroy(aplib_curl_session *session) {
	if (session == NULL)
		return;

	curl_easy_cleanup(session->easy);
	curl_share_cleanup(session->share);
	free(session->cookie_jar);
	free(session);
}

CURLcode aplib_curl_session_set_cookie_jar(aplib_curl_session *session,
	const char *path) {
	char *copy = NULL;

	if (session == NULL)
		return CURLE_BAD_FUNCTION_ARGUMENT;
	if (path != NULL) {
		copy = aplib_curl_copy_string(path);
		if (copy == NULL)
			return CURLE_OUT_OF_MEMORY;
	}

	free(session->cookie_jar);
	session->cookie_jar = copy;
	return CURLE_OK;
}

void aplib_curl_response_free(aplib_curl_response *response) {
	if (response == NULL)
		return;

	free(response->body);
	free(response->headers);
	memset(response, 0, sizeof(*response));
}

CURLcode aplib_curl_request(aplib_curl_session *session,
	const aplib_curl_request_options *options,
	aplib_curl_response *response) {
	struct aplib_curl_buffer body = { NULL, 0, 0 };
	struct aplib_curl_buffer headers_buffer = { NULL, 0, 0 };
	struct curl_slist *headers = NULL;
	CURLcode result;
	const char *method;

	if (session == NULL || options == NULL || response == NULL || options->url == NULL)
		return CURLE_BAD_FUNCTION_ARGUMENT;
	if (options->body_size != 0 && options->body == NULL)
		return CURLE_BAD_FUNCTION_ARGUMENT;
	if (options->body_size > (size_t)INT64_MAX)
		return CURLE_BAD_FUNCTION_ARGUMENT;

	aplib_curl_response_free(response);
	method = options->method == NULL ? "GET" : options->method;
	result = aplib_curl_make_headers(options->headers, options->header_count, &headers);
	if (result != CURLE_OK)
		goto done;

	curl_easy_reset(session->easy);
	result = curl_easy_setopt(session->easy, CURLOPT_URL, options->url);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_SHARE, session->share);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_PROTOCOLS_STR, "http,https");
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_FOLLOWLOCATION,
			options->follow_redirects ? 1L : 0L);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_MAXREDIRS, 10L);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_SSL_VERIFYPEER, 1L);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_SSL_VERIFYHOST, 2L);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_CONNECTTIMEOUT_MS,
			options->connect_timeout_ms > 0 ? options->connect_timeout_ms : 15000L);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_TIMEOUT_MS,
			options->timeout_ms > 0 ? options->timeout_ms : 60000L);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_ACCEPT_ENCODING, "");
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_USERAGENT, "APLIB/1.0");
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_COOKIEFILE, "");
	if (result == CURLE_OK && session->cookie_jar != NULL)
		result = curl_easy_setopt(session->easy, CURLOPT_COOKIEFILE, session->cookie_jar);
	if (result == CURLE_OK && session->cookie_jar != NULL)
		result = curl_easy_setopt(session->easy, CURLOPT_COOKIEJAR, session->cookie_jar);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_HTTPHEADER, headers);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_WRITEFUNCTION, aplib_curl_append);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_WRITEDATA, &body);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_HEADERFUNCTION, aplib_curl_append);
	if (result == CURLE_OK)
		result = curl_easy_setopt(session->easy, CURLOPT_HEADERDATA, &headers_buffer);
	if (result == CURLE_OK)
		result = aplib_curl_set_auth(session->easy, options->username,
			options->password, options->http_auth);
	if (result == CURLE_OK) {
		if (strcmp(method, "GET") == 0)
			result = curl_easy_setopt(session->easy, CURLOPT_HTTPGET, 1L);
		else if (strcmp(method, "HEAD") == 0) {
			result = curl_easy_setopt(session->easy, CURLOPT_NOBODY, 1L);
			if (result == CURLE_OK)
				result = curl_easy_setopt(session->easy, CURLOPT_CUSTOMREQUEST, method);
		} else if (strcmp(method, "POST") == 0)
			result = curl_easy_setopt(session->easy, CURLOPT_POST, 1L);
		else
			result = curl_easy_setopt(session->easy, CURLOPT_CUSTOMREQUEST, method);
	}
	if (result == CURLE_OK && options->body != NULL) {
		result = curl_easy_setopt(session->easy, CURLOPT_POSTFIELDS, options->body);
		if (result == CURLE_OK)
			result = curl_easy_setopt(session->easy, CURLOPT_POSTFIELDSIZE_LARGE,
				(curl_off_t)options->body_size);
	}
	if (result == CURLE_OK)
		result = curl_easy_perform(session->easy);
	if (result == CURLE_OK)
		result = curl_easy_getinfo(session->easy, CURLINFO_RESPONSE_CODE,
			&response->status_code);

	if (body.allocation_failed || headers_buffer.allocation_failed)
		result = CURLE_OUT_OF_MEMORY;

	if (result == CURLE_OK) {
		if (body.data == NULL)
			body.data = (char *)calloc(1, 1);
		if (headers_buffer.data == NULL)
			headers_buffer.data = (char *)calloc(1, 1);
		if (body.data == NULL || headers_buffer.data == NULL)
			result = CURLE_OUT_OF_MEMORY;
	}

	if (result == CURLE_OK) {
		response->body = body.data;
		response->body_size = body.size;
		response->headers = headers_buffer.data;
		response->headers_size = headers_buffer.size;
		body.data = NULL;
		headers_buffer.data = NULL;
	}

done:
	if (headers != NULL) {
		curl_easy_setopt(session->easy, CURLOPT_HTTPHEADER, NULL);
		curl_slist_free_all(headers);
	}
	free(body.data);
	free(headers_buffer.data);
	if (result != CURLE_OK)
		aplib_curl_response_free(response);
	return result;
}

CURLcode aplib_curl_websocket_open(const char *url,
	aplib_curl_session *session,
	const char *const *headers, size_t header_count,
	const char *username, const char *password, long http_auth,
	long connect_timeout_ms, aplib_curl_websocket **websocket,
	long *status_code) {
	aplib_curl_websocket *created;
	CURLcode result;
	long response_status = 0;

	if (url == NULL || websocket == NULL)
		return CURLE_BAD_FUNCTION_ARGUMENT;
	*websocket = NULL;
	if (status_code != NULL)
		*status_code = 0;

	created = (aplib_curl_websocket *)calloc(1, sizeof(*created));
	if (created == NULL)
		return CURLE_OUT_OF_MEMORY;
	created->easy = curl_easy_init();
	if (created->easy == NULL) {
		free(created);
		return CURLE_FAILED_INIT;
	}

	result = aplib_curl_make_headers(headers, header_count, &created->headers);
	if (result == CURLE_OK)
		result = curl_easy_setopt(created->easy, CURLOPT_URL, url);
	if (result == CURLE_OK && session != NULL)
		result = curl_easy_setopt(created->easy, CURLOPT_SHARE, session->share);
	if (result == CURLE_OK)
		result = curl_easy_setopt(created->easy, CURLOPT_PROTOCOLS_STR, "ws,wss");
	if (result == CURLE_OK)
		result = curl_easy_setopt(created->easy, CURLOPT_CONNECT_ONLY, 2L);
	if (result == CURLE_OK)
		result = curl_easy_setopt(created->easy, CURLOPT_SSL_VERIFYPEER, 1L);
	if (result == CURLE_OK)
		result = curl_easy_setopt(created->easy, CURLOPT_SSL_VERIFYHOST, 2L);
	if (result == CURLE_OK)
		result = curl_easy_setopt(created->easy, CURLOPT_CONNECTTIMEOUT_MS,
			connect_timeout_ms > 0 ? connect_timeout_ms : 15000L);
	if (result == CURLE_OK)
		result = curl_easy_setopt(created->easy, CURLOPT_HTTPHEADER, created->headers);
	if (result == CURLE_OK)
		result = aplib_curl_set_auth(created->easy, username, password, http_auth);
	if (result == CURLE_OK)
		result = curl_easy_perform(created->easy);
	if (result == CURLE_OK)
		result = curl_easy_getinfo(created->easy, CURLINFO_RESPONSE_CODE, &response_status);
	if (status_code != NULL)
		*status_code = response_status;
	if (result == CURLE_OK && response_status != 101)
		result = CURLE_WEIRD_SERVER_REPLY;

	if (result != CURLE_OK) {
		aplib_curl_websocket_close(created);
		return result;
	}
	*websocket = created;
	return CURLE_OK;
}

CURLcode aplib_curl_websocket_send(aplib_curl_websocket *websocket,
	const void *data, size_t size, unsigned int flags, size_t *sent) {
	if (websocket == NULL || sent == NULL || (size != 0 && data == NULL))
		return CURLE_BAD_FUNCTION_ARGUMENT;
	return curl_ws_send(websocket->easy, data, size, sent, 0, flags);
}

CURLcode aplib_curl_websocket_receive(aplib_curl_websocket *websocket,
	void *buffer, size_t capacity, size_t *received,
	unsigned int *flags, curl_off_t *bytes_left) {
	const struct curl_ws_frame *frame = NULL;
	CURLcode result;

	if (websocket == NULL || received == NULL || flags == NULL || bytes_left == NULL ||
		(capacity != 0 && buffer == NULL))
		return CURLE_BAD_FUNCTION_ARGUMENT;

	result = curl_ws_recv(websocket->easy, buffer, capacity, received, &frame);
	if (result == CURLE_OK && frame != NULL) {
		*flags = (unsigned int)frame->flags;
		*bytes_left = frame->bytesleft;
	} else {
		*flags = 0;
		*bytes_left = 0;
	}
	return result;
}

CURLMcode aplib_curl_websocket_wait_readable(aplib_curl_websocket *websocket,
	int timeout_ms, int *readable) {
	CURLM *multi;
	CURLMcode result;
	struct curl_waitfd waitfd;
	int ready = 0;

	if (websocket == NULL || readable == NULL || timeout_ms < 0)
		return CURLM_BAD_FUNCTION_ARGUMENT;
	*readable = 0;

	memset(&waitfd, 0, sizeof(waitfd));
	if (curl_easy_getinfo(websocket->easy, CURLINFO_ACTIVESOCKET, &waitfd.fd) != CURLE_OK)
		return CURLM_INTERNAL_ERROR;
	waitfd.events = CURL_WAIT_POLLIN;

	multi = curl_multi_init();
	if (multi == NULL)
		return CURLM_OUT_OF_MEMORY;
	result = curl_multi_poll(multi, &waitfd, 1, timeout_ms, &ready);
	curl_multi_cleanup(multi);
	if (result == CURLM_OK)
		*readable = (waitfd.revents & CURL_WAIT_POLLIN) != 0;
	return result;
}

void aplib_curl_websocket_close(aplib_curl_websocket *websocket) {
	if (websocket == NULL)
		return;

	curl_easy_cleanup(websocket->easy);
	curl_slist_free_all(websocket->headers);
	free(websocket);
}