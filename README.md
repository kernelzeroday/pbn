# pbn - P2P Bot Net
```
                    ..
              . uW8"
 .d``         `t888          u.    u.
 @8Ne.   .u    8888   .    x@88k u@88c.
 %8888:u@88N   9888.z88N  ^"8888""8888"
  `888I  888.  9888  888E   8888  888R
   888I  888I  9888  888E   8888  888R
   888I  888I  9888  888E   8888  888R
 uW888L  888'  9888  888E   8888  888R
'*88888Nu88P  .8888  888"  "*88*" 8888"
~ '88888F`     `%888*%"      ""   'Y"
   888 ^          "`
   *8E
   '8>      P 2 P   B O T N E T
    "
```

pbn is a fully distributed peer to peer botnet client written in C using tox as a base for communications.

pbn features include DHT peer discovery, keypair message signature verification, onion message passing, and fully encrypted communication.

pbn was written as a proof of concept for a botnet with no points of failure during a night of heavy drinking at defcon a few years ago.

Yes pbn works. No pbn is not battle tested. Yes pbn probably has bugs. 

In order to build you will need libsodum and c-toxcore. 

I have not worked on this for like 2 years and I felt it was time to open it up for other people to take a look at and improve. 

There is/was a python controller and a c controller, I don't know exactly what is working and what is not working, I am starting off by releasing the client and I will see where to go from there.

Expect more documentation and examples and shit like that eventually.

Cheers


# NOTES:

When I wrote this i was pretty intoxicated, and did not document anything. Good luck on that. Also, c-toxcore was not documented at all and I just read the implementation to write this. Shit is probably wrong in many places, or changed since I last wrote it. I just built it against toktok/c-toxcore and it worked, but idk if conferences ever got implemented which was something I was looking at, and idk where i left group messages. Needs work, gl, hf.


IDK the status of pytox, which was used when writing owner.py. I am including it as a reference for myself or others looking to build a new botnet controller interface.


cli.c is an unauthenticated botnet client. Don't use it.

I forget what i was doing with slim.c maybe it does the group stuff. IDK i will look at it in a bit.


pytox works if u do 
```
pip3 install --user 'https://github.com/TokTok/py-toxcore-c/archive/v0.2.0.tar.gz'
```

added the half static makefile, builds on my osx but u probbaly need to modify it for linux. ill add the line that worked on my old linux box as a comment.

the binary is only 422kb on my mac built half static btw
