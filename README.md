averyws
=======

websocket server with boost::asio serving tasks that communicate with zmq
averyws aims to be a high-performance websocket server with authentication and permissioning (via Access Control Lists) at its core. (maybe caching as well)
averyws does not aim to be a general purpose websocket server. It aims to support best practices only. This is why aveyws will only work over SSL.

todo:
ssl (http://www.boost.org/doc/libs/1_41_0/doc/html/boost_asio/example/ssl/server.cpp)
EOF to sockets that take too long to send incoming data. guidlines for now are 25ms (40bps) for the first byte and 50kbs for bytes thereafter integer rounded. The idea is that clients should implement a strong retry policy.
permissioning (shouldn't be too hard)
authentication (port https://github.com/matthewaveryusa/node_pg_acl to C++, should be easy)
control frames (ping/pong, should be easy)
authentication/encryption of backend clients (hmac) that are outside of localhost and the trusted network (whitelist todo) (pretty hard for me but should be easy for others)
have a really good protocol between the server and the backend clients. the current implementation is for proof of concept only

Major effort for logging should be done. probably at compile time

possible performance issues:
The sockets are read in a non-greedy way which implies more calls to the os (unless asio buffers) which degrades performance.
os of gcc4.7, unordered sets/maps are unstable and crash when the keys are shared pointers. The will eventually be resolved.
A heuristic is needed to clean the weak pointers that are no longer valid in the session_manager
