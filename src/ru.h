#include <stdlib.h>
#include <stdbool.h>
#define ROZ 100

enum message_type {
	ACK_NACK,
	GET_INFO,
	SHOW_ALL,
	SET_MAC_INTRO,
	SET_MAC
};

struct message {
	enum message_type x;
	char* wiad;
};

int send_ack_nack(int fd, bool is_error, const char* error_msg);
int send_log_in(int fd, const char* name);
int send_user_list(int fd);
int send_user_list_reply(int fd, const char* names[], size_t len);
struct message* receive_message(int fd);
void delete_message(struct message* m);
int send_bytes(int fd, const char* msg, size_t len);
int send_ifs(int fd);
int send_inf(int fd,const char* interfejs);