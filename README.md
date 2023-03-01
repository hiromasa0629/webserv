# **Refrence**
- HTTP server build from scratch - https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
- C++ Network Programming Part 1: Sockets - https://www.youtube.com/watch?v=gntyAFoZp-E
- Man pages - https://beej.us/guide/bgnet/html/#bindman
- epoll() (Not available on Mac, alternatives - kqueue) - https://web.archive.org/web/20120504033548/https://banu.com/blog/2/how-to-use-epoll-a-complete-example-in-c/
- chunked request - https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Transfer-Encoding
# **Server side**
- Step 1: Load up address structs `getaddrinfo()`
- Step 2: Create a socket `socket()`
- Step 3: Bind the socket `bind()`
- Step 4: Listen on the socket `listen()`
  - insert socketfd into the fds pool
  - set a timeout on `poll()`
  - `poll()` the fd pool to watch for `POLLIN` or `POLLOUT` events
- Step 5: Accept a connection `accept()`
  - set accepted connection fd to non-blocking

# **Client side**
- Step 1: Loud up address structs `getaddrinfo()`
- Step 2: Create socket `socket()`
- Step 3: Connect to the server `connect()`
  
---
I found this answer a great explanation for `poll()` in c on StackOverflow - https://stackoverflow.com/questions/19557941/implementing-poll-on-a-tcp-servers-read-write

```c
int i, n;

n = poll(ufds, num_fds_in_array, timeout_value);

/* errors or timeout? */
if (n < 1)
    ;

for (i = 0; i < num_fds_in_array; i++) {
    /* were there any events for this socket? */
    if (!ufds[i].revents)
        continue;

    /* is it our listening socket? */
    if (!i) {
         if (ufds[0].revents & POLLIN)
             /* call accept() and add the new socket to ufds */
         else
             /* error */

         continue;
    }

    /* is there incoming data on the socket? */
    if (ufds[i].revents & POLLIN)
        /* call recv() on the socket and decide what to do from there */
}
```
# **Functions**
### includes
- sys/socket.h
- sys/types.h
- netdb.h  

getaddrinfo()
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
  char   *ai_canonname;      // canonical name for nodename
  struct sockaddr  *ai_addr; // binary address
  struct addrinfo  *ai_next; // next structure in linked list
};

// return information on a particular host name (such as its IP address) and load up a struct sockaddr
// 0 on success, non-zero on error. Use gai_strerror() to view error value
int getaddrinfo(const char *hostname, const char *service_name, const struct addrinfo *hints, struct addrinfo **res);
```
socket()
```c++
// Returns a socket descriptor, -1 on error
// domain	= addrinfo->ai_family
// type		= addrinfo->ai_socktype
// protocol = addrinfo->ai_protocol
int socket(int domain, int type, int protocol);
```
bind()
```c++
// Returns 0 on success, -1 on error
// my_addr	= addrinfo->ai_addr
// addrlen	= addrinfo->ai_addrlen
int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);
```
listen()
```c++
// Returns 0 on success, -1 on error
// backlog	= number of pending connections you can have before the kernel start rejecting new ones
int listen(int sockfd, int backlog);
```
accept()
```c++
// Returns the newly connected sockfd, -1 on error
// sockfd	= the listenig sockfd
// addr		= client's addrinfo->ai_addr
// addrlen	= client's addrinfo->ai_addrlen
int accept(int sockfd, struct sockaddr *client_addr, socklen_t *client_addrlen);
```
connect()
```c++
// Returns 0 on success, -1 on error
// getaddrinfo 
// serv_addr	= addrinfo
int connect(int sockfd, const struct sockaddr *serv_addr, socklen_t serv_addrlen);
```
send() and recv()
```c++
// Returns the number of bytes sent out, sometimes its different from actual bytes sent, manage to send 1K or below, -1 on error
// flags = man send to know more information, 0 for now
int send(int sockfd, const void *msg, int len, int flags); 
// Returns the number of bytes actually received, -1 on error
int recv(int sockfd, void *buf, int len, int flags);
```
poll()
```c++
#include <sys/poll.h>

struct pollfd {
    int fd;         // the socket descriptor
    short events;   // bitmap of events we're interested in
    short revents;  // when poll() returns, bitmap of events that occurred
};

// POLLIN		Alert me when data is ready to recv() on this socket.
// POLLOUT		Alert me when I can send() data to this socket without blocking.
// POLLPRI		Alert me when out-of-band data is ready to recv() on this socket.
// POLLERR		An error has occurred on this socket.
// POLLHUP		The remote side of the connection hung up.
// POLLNVAL		Something was wrong with the socket descriptor fd—maybe it’s uninitialized?

// Returns the number of elements in the ufds that had event occur, 0 if timeout, -1 on error
// ufds		= fd pool
// nfds		= nunber of fds in the pool
// timeout	= in ms
int poll(struct pollfd *ufds, unsigned int nfds, int timeout);
```
# **Server configuration**
```
server      {
	listen	[port]		listening port
	root	[root dir]	root directory for files to be served
	
	error_page	[error_code 404] [path to page]	display page if error
	error_page	[405]			 [path to page]
	
	server_name	[domain]
	
	cgi [extension] [command]	for execve to execute
	
	location [path] {
		limit_except			[methods] [methods] [methods]	limitting request methods (multiple)
		index					[index file]	serve default page for path
		redirect				[path]			redirect to path
		client_max_body_size	[size]
		autoindex 				[on/off]
		root
	}
}
```
# **Requests examples**
**CRLF** - Carriage return, line feed (\r\n) 
<!-- - **Chunked**
```
HTTP/1.1 200 OK\r\n
Content-Type: text/plain\r\n
Transfer-Encoding: chunked\r\n
\r\n
7\r\n
Mozilla\r\n	
11\r\n
Developer Network\r\n
0\r\n
\r\n
```  -->
- **GET**
```
GET /index.html HTTP/1.1\r\n
Host: www.example.com\r\n
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36\r\n
Accept-Language: en-US,en;q=0.9\r\n
\r\n
```
- **POST**
```
POST /api/login HTTP/1.1\r\n
Host: www.example.com\r\n
Content-Type: application/x-www-form-urlencoded\r\n
Content-Length: 23\r\n
\r\n
username=johndoe&password=secret\r\n
\r\n
```
example of form data. Each line ends with crlf
```
------WebKitFormBoundaryEzYoUBJs8wtxsDPZ
Content-Disposition: form-data; name="file"; filename="helloworld.jpeg"
Content-Type: image/jpeg

hello world

------WebKitFormBoundaryEzYoUBJs8wtxsDPZ--

body_size: 7

------WebKitFormBoundaryEzYoUBJs8wtxsDPZ
Content-Disposition: form-data; name="file"; filename="helloworld.jpeg"
Content-Type: image/jpeg

hello world

------WebKitFormBoundaryEzYoUBJs8wtxsDPZ--
```
