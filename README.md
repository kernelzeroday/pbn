# pbn - P2P Bot Net

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