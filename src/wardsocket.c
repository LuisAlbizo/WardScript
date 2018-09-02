/* Socket Interface libray file: wardsocket.c
 * Luis Albizo "08-30-18"
 */

#include "object.h"
#include "scope.h"
#include "stack.h"
#include "error.h"
#include "dict.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* Socket module interface:
 *
 *	A socket designates an abstract concept by which two programs (possibly located in different 
 *	computers) can exchange any data flow, generally in a reliable and orderly manner.
 *
 *	This library offers an interface similar to the original Berkeley Sockets.
 *
 * 	socket members:
 * 		socket() -> <node> sock ; creates a socket
 *
 * 		sock:bind(<node> sock_addr) -> <byte> success ; used in the server side, the server will 
 * 			recieve connections on the 'sock_addr' network address, sock_addr must be like:
 * 				sock_addr := {
 * 					port := [144, 31],
 * 					ipv4 := [127, 0, 0, 1]
 * 				};
 * 				? This address is 127.0.0.1:8080 ?
 * 			with port being a 2 byte list, and ipv4 a 4 byte list.
 * 			The success byte returns 0 if an error ocurrs and can not assign that
 * 			address, or 1 if there is no errors.
 *
 *		sock:listen(<byte> backlog) -> nil ; used in the server side, backlog represents
 *			the number of pending connections that can be queued up at any one time.
 *
 *		sock:accept() -> <node> conn ; used in the server side, it waits until a new connection
 *			appears.
 *
 *			the conn node is like this:
 *				conn := {
 *					conn := <node> sock,
 *					addr := <node> sock_addr
 *				};
 *				(sock and sock_addr as described before)
 *			The connection member is the socket where the communication can be made.
 *
 *		sock:connect(<node> sock_addr) -> <byte> success ; used in client side,
 *			establish a connection with another socket (server) that is listening
 *			on sock_addr address. succes returns 0 if it cannot connect to that address
 *			and 1 if the connection has established succesfully.
 *
 *		sock:send(<node> data) -> <node> size ; send a list of bytes (in string format) and return
 *			a 2 byte list, the data format must be:
 *				data := {
 *					$char := <byte> 64,
 *					$nextchar := <node> data_l
 *				};
 *				? data_l follow the same format, or can be a nil if no data is left. ?
 *			and size follow the list format.
 *			[0, 4] is 1024 (1 KB)
 *
 *		sock:recv(<node> size) -> <node> data ; recieve data, size is a 2 byte list with the
 *			max size of bytes to receive, returns the data.
 *
 *		sock:close() -> nil ; closes a socket, free the port the socket was using.
 */

/* Auxiliar functions */

char *ipv4_to_str(B_Node *l) {
	char *sip = malloc(16);
	l = (B_Node *) Bnode_Get(l, "$root");
	if (l->type != B_NODE)
		raiseError(TYPE_ERROR, "empty list on ipv4 member");
	for (char b = 0; b < 4; b++) {
		sprintf(sip, "%d", ((B_Byte *) Bnode_Get(l, "$data"))->byte);
		l = (B_Node *) Bnode_Get(l, "$next");
		if (l->type != B_NODE)
			break;
		else
			sprintf(sip, ".");
	}
	sip[15] = '\0';
	return sip;
}

short port_to_short(B_Node *p) {
	short s;
	p = (B_Node *) Bnode_Get(p, "$root");
	if (p->type != B_NODE)
		raiseError(TYPE_ERROR, "empty list on ipv4 member");
	s = ((B_Byte *) Bnode_Get(p, "$data"))->byte;
	p = (B_Node *) Bnode_Get(p, "$next");
	s = s + (((B_Byte *) Bnode_Get(p, "$data"))->byte * 256);
	return s;
}

B_Object *short_to_port(short s) {
	char b1 = s % 256, b2 = s / 256;
	B_Node *port = (B_Node *) new_ListItem(new_bbyte(b1));
	Bnode_Set(port, "$next", (dict_Data *) new_ListItem(new_bbyte(b2)));
	B_Node *l = (B_Node *) new_List(newstack());
	Bnode_Set(l, "$root", (dict_Data *) port);
	return (B_Object *) l;
}

/* Socket functions */

void assign_socket_methods(dict *d, Scope *S) {
	// Function to assign the socket methods to a node
	dict_update(d, "bind",		(dict_Data *) Scope_Get(S, ".socket-bind"));
	dict_update(d, "listen",	(dict_Data *) Scope_Get(S, ".socket-listen"));
	dict_update(d, "accept",	(dict_Data *) Scope_Get(S, ".socket-accept"));
	dict_update(d, "connect",	(dict_Data *) Scope_Get(S, ".socket-connect"));
	dict_update(d, "send",		(dict_Data *) Scope_Get(S, ".socket-send"));
	dict_update(d, "recv",		(dict_Data *) Scope_Get(S, ".socket-recv"));
	dict_update(d, "close",		(dict_Data *) Scope_Get(S, ".socket-close"));
}

struct w_socket {
	int fd; // File Descriptor
	struct sockaddr_in addr; // Socket Address
};

typedef struct w_socket w_socket;

/* SOCKET */
B_Object *ward_socket(stack *args, Scope *S) {
	w_socket *sock = malloc(sizeof(w_socket));
	if (!sock)
		raiseError(MEMORY_ERROR, "can't create new socket");
	sock->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock->fd == -1) {
		// The socket can not be created
		free(sock);
		return new_bnil();
	}
	// Create the ward node
	dict *d = newdict();
	dict_update(d, ".socket", (dict_Data *) sock);
	assign_socket_methods(d, S);
	return new_bnode(d);
}

/* BIND */
B_Object *ward_socket_bind(stack *args, Scope *S) {
	char success = 1;
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "bind requires 2 arguments: socket, sock_addr");
	B_Node *sock_node = (B_Node *) stack_pop(args);
	if (sock_node->type != B_NODE)
		raiseError(TYPE_ERROR, "socket must be a node");
	B_Node *sock_addr = (B_Node *) stack_pop(args);
	if (sock_addr->type != B_NODE)
		raiseError(TYPE_ERROR, "sock_addr must be a node");
	// Bind
	B_Node *port = (B_Node *) Bnode_Get(sock_addr, "port");
	if (port->type != B_NODE)
		raiseError(TYPE_ERROR, "port must be a node");
	B_Node *ipv4 = (B_Node *) Bnode_Get(sock_addr, "ipv4");
	if (ipv4->type != B_NODE)
		raiseError(TYPE_ERROR, "ipv4 must be a node");
	// Construct sockaddr_in
	w_socket *sock = (w_socket *) Bnode_Get(sock_node, ".socket");
	sock->addr.sin_addr.s_addr = inet_addr(ipv4_to_str(ipv4));
	sock->addr.sin_family = AF_INET;
	sock->addr.sin_port = htons(port_to_short(port));
	// Calling c interface
	if (bind(sock->fd, (struct sockaddr *) &sock->addr, sizeof(struct sockaddr)) < 0)
		success = 0;
	return new_bbyte(success);
}

/* LISTEN */
B_Object *ward_socket_listen(stack *args, Scope *S) {
	char success = 1;
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "listen requires 2 arguments: socket, backlog");
	B_Node *sock_node = (B_Node *) stack_pop(args);
	if (sock_node->type != B_NODE)
		raiseError(TYPE_ERROR, "socket must be a node");
	B_Byte *blog = (B_Byte *) stack_pop(args);
	if (blog->type != B_BYTE)
		raiseError(TYPE_ERROR, "backlog must be a node");
	// get the socket
	w_socket *sock = (w_socket *) Bnode_Get(sock_node, ".socket");
	// calling c interface
	if (listen(sock->fd, blog->byte) < 0)
		success = 0;
	return new_bbyte(success);
}

/* ACCEPT */
B_Object *ward_socket_accept(stack *args, Scope *S) {
	if (args->count != 1)
		raiseError(ARGCOUNT_ERROR, "accept requires 2 arguments: socket");
	B_Node *sock_node = (B_Node *) stack_pop(args);
	if (sock_node->type != B_NODE)
		raiseError(TYPE_ERROR, "socket must be a node");
	// Create the client socket
	w_socket *sock = (w_socket *) Bnode_Get(sock_node, ".socket"),
	*client = malloc(sizeof(w_socket));
	if (!client)
		raiseError(MEMORY_ERROR, "can't create new client");
	// calling the c interface
	int c = sizeof(struct sockaddr_in);
	client->fd = accept(sock->fd, (struct sockaddr *) &client->addr, (socklen_t *) &c);
	if (client->fd < 0) {
		free(client);
		return new_bnil();
	}
	// Create conn node
	dict *cd = newdict();
	dict_update(cd, ".socket", (dict_Data *) client);
	assign_socket_methods(cd, S);
	B_Object *conn = new_bnode(cd);
	// Create addr node

	// Create the ward node
	dict *wd = newdict();
	dict_update(wd, "conn", (dict_Data *) conn);
	//dict_update(wd, "addr", (dict_Data *) addr);
	return new_bnode(wd);
}

/* CONNECT */
B_Object *ward_socket_connect(stack *args, Scope *S) {
	char success = 1;
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "connect requires 2 arguments: socket, sock_addr");
	B_Node *sock_node = (B_Node *) stack_pop(args);
	if (sock_node->type != B_NODE)
		raiseError(TYPE_ERROR, "socket must be a node");
	B_Node *sock_addr = (B_Node *) stack_pop(args);
	if (sock_addr->type != B_NODE)
		raiseError(TYPE_ERROR, "sock_addr must be a node");
	// Bind
	B_Node *port = (B_Node *) Bnode_Get(sock_addr, "port");
	if (port->type != B_NODE)
		raiseError(TYPE_ERROR, "port must be a node");
	B_Node *ipv4 = (B_Node *) Bnode_Get(sock_addr, "ipv4");
	if (ipv4->type != B_NODE)
		raiseError(TYPE_ERROR, "ipv4 must be a node");
	// Construct sockaddr_in
	w_socket *sock = (w_socket *) Bnode_Get(sock_node, ".socket");
	sock->addr.sin_addr.s_addr = inet_addr(ipv4_to_str(ipv4));
	sock->addr.sin_family = AF_INET;
	sock->addr.sin_port = htons(port_to_short(port));
	// Calling c interface
	if (connect(sock->fd, (struct sockaddr *) &sock->addr, sizeof(struct sockaddr)) < 0)
		success = 0;
	return new_bbyte(success);
}

/* SEND */
B_Object *ward_socket_send(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "send requires 2 arguments: socket, data");
	B_Node *sock_node = (B_Node *) stack_pop(args);
	if (sock_node->type != B_NODE)
		raiseError(TYPE_ERROR, "socket must be a node");
	B_Node *data = (B_Node *) stack_pop(args);
	if (data->type != B_NODE)
		raiseError(TYPE_ERROR, "data must be a node");
	char *data_str = malloc(128);
	char count = 0;
	short size = 0;
	while (data->type == B_NODE) {
		count++;
		if (count == 128) {
			count = 0;
			data_str = realloc(data_str, size + 128);
		}
		data_str[size] = ((B_Byte *) Bnode_Get(data, "$char"))->byte;
		size++;
		data = (B_Node *) Bnode_Get(data, "$next");
	}
	// calling the interface
	w_socket *sock = (w_socket *) Bnode_Get(sock_node, ".socket");
	// Calling c interface
	if (send(sock->fd, data_str, size, 0) < 0) {
		free(data_str);
		return new_bnil();
	}
	free(data_str);
	return short_to_port(size);
}

/* RECV */
B_Object *ward_socket_recv(stack *args, Scope *S) {
	if (args->count != 2)
		raiseError(ARGCOUNT_ERROR, "recv requires 2 arguments: socket, size");
	B_Node *sock_node = (B_Node *) stack_pop(args);
	if (sock_node->type != B_NODE)
		raiseError(TYPE_ERROR, "socket must be a node");
	B_Node *size = (B_Node *) stack_pop(args);
	if (size->type != B_NODE)
		raiseError(TYPE_ERROR, "size must be a node");
	int recv_len = port_to_short(size);
	char *data = malloc(recv_len + 1);
	// Get socket
	w_socket *sock = (w_socket *) Bnode_Get(sock_node, ".socket");
	if (recv(sock->fd, data, recv_len, 0) < 0) {
		free(data);
		return new_bnil();
	}
	size = (B_Node *) new_string(data);
	free(data);
	return (B_Object *) size;
}

/* CLOSE */
B_Object *ward_socket_close(stack *args, Scope *S) {
	if (args->count != 1)
		raiseError(ARGCOUNT_ERROR, "close requires 1 argument: socket");
	B_Node *sock_node = (B_Node *) stack_pop(args);
	if (sock_node->type != B_NODE)
		raiseError(TYPE_ERROR, "socket must be a node");
	// Get socket
	w_socket *sock = (w_socket *) Bnode_Get(sock_node, ".socket");
	close(sock->fd);
	return new_bnil();
}

/* Module Loader */
B_Object *module_loader(stack *args, Scope *S) {
	while (S->upscope)
		S = S->upscope;
	Scope_Set(S, ".socket-bind",	(Scope_Object *) new_cfunction(ward_socket_bind));
	Scope_Set(S, ".socket-listen",	(Scope_Object *) new_cfunction(ward_socket_listen));
	Scope_Set(S, ".socket-accept",	(Scope_Object *) new_cfunction(ward_socket_accept));
	Scope_Set(S, ".socket-connect",	(Scope_Object *) new_cfunction(ward_socket_connect));
	Scope_Set(S, ".socket-send",	(Scope_Object *) new_cfunction(ward_socket_send));
	Scope_Set(S, ".socket-recv",	(Scope_Object *) new_cfunction(ward_socket_recv));
	Scope_Set(S, ".socket-close",	(Scope_Object *) new_cfunction(ward_socket_close));
	// creating the module node
	dict *d = newdict();
	dict_update(d, "socket", (dict_Data *) new_cfunction(ward_socket));
	return new_bnode(d);
}

