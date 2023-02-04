## **Refrence**
- HTTP server build from scratch - https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
- C++ Network Programming Part 1: Sockets - https://www.youtube.com/watch?v=gntyAFoZp-E
- Man pages - https://beej.us/guide/bgnet/html/#bindman
---
## **Server side**
- Step 1: Load up address structs `getaddrinfo()`
- Step 2: Create a socket `socket()`
- Step 3: Bind the socket `bind()`
- Step 4: Listen on the socket `listen()`
- Step 5: Accept a connection `accept()`

## **Client side**
- Step 1: Loud up address structs `getaddrinfo()`
- Step 2: Create socket `socket()`
- Step 3: Connect to the server `connect()`

---
## **Functions**
### sys/socket.h
```c++
// ai_family	= AF_INET for ipv4 | AF_INET6 for ipv6 | AF_UNSPEC for both
// ai_socktype	= SOCK_STREAM for TCP | SOCK_DGRAM for UDP
// ai_flags		= AI_PASSIVE for automatically fills host IP
struct addrinfo {
  int     ai_flags;          // AI_PASSIVE, AI_CANONNAME, ...
  int     ai_family;         // AF_xxx
  int     ai_socktype;       // SOCK_xxx
  int     ai_protocol;       // 0 (auto) or IPPROTO_TCP, IPPROTO_UDP

  socklen_t  ai_addrlen;     // length of ai_addr
  char   *ai_canonname;      // canonical name for hostname
  struct sockaddr  *ai_addr; // binary address
  struct addrinfo  *ai_next; // next structure in linked list
};

// return information on a particular host name (such as its IP address) and load up a struct sockaddr
// 0 on success, non-zero on error. Use gai_strerror() to view error value
int getaddrinfo(const char *hostname, const char *service_name, const struct addrinfo *hints, struct addrinfo **res);
```
```c++
// Returns a socket descriptor, -1 on error
// domain	= addrinfo->ai_family
// type		= addrinfo->ai_socktype
// protocol = addrinfo->ai_protocol
int socket(int domain, int type, int protocol);
```
```c++
// Returns 0 on success, -1 on error
// my_addr	= addrinfo->ai_addr
// addrlen	= addrinfo->ai_addrlen
int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);
```
```c++
// Returns 0 on success, -1 on error
// backlog	= number of pending connections you can have before the kernel start rejecting new ones
int listen(int sockfd, int backlog);
```
```c++
// Returns the newly connected sockfd, -1 on error
// sockfd	= the listenig sockfd
// addr		= client's addrinfo->ai_addr
// addrlen	= client's addrinfo->ai_addrlen
int accept(int sockfd, struct sockaddr *client_addr, socklen_t *client_addrlen);
```
```c++
// Returns 0 on success, -1 on error
// getaddrinfo
// serv_addr	= addrinfo
int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t serv_addrlen);
```
