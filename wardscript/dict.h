/* Dictionary Data Structure definition file: dict.h
 * Luis Albizo "07-20-18"
 */
#ifndef dict_h
#define dict_h

/* This Dictionary Implementation uses an AVL-Tree 
 * to index every value associated with a string.
 * 
 * The string is a key the dictionary uses to make 
 * a search, comparing each string with strcmp()
 */

#define MAX_DICT_KEY 128

struct dict_Data {
	unsigned int type;
};

struct dict_node {
	char key[MAX_DICT_KEY]; // Search Key of MAX_DICT_KEY maximum lenght
	struct dict_Data *data; // Data
	struct dict_node *parent; // Pointer to parent node
	struct dict_node *l; // Left Subtree
	struct dict_node *r; // Right Subtree
};

struct dict {
	struct dict_node *root;
};

typedef struct dict_node dict_node;
typedef struct dict dict;
typedef struct dict_Data dict_Data;

/* AVL Functions to insert dict_nodes, remove dict_nodes, rebalance, rotate, etc.
 */

dict_node *newdict_node(char *, dict_Data *, dict_node *, dict_node *, dict_node *);
unsigned int nchilds(dict_node *);
int getpos(dict_node *); /* Return the relative position a node has for his parent-node.
			    return 0 if the node is the left child of his parent-node
			    return 1 if the node is the right child of his parent-node
			    return -1 if the node has no parent-node
			    */
void setpos(int, dict_node *, dict_node *); /* Set a child to a dict_node according to pos
					       pos 0 is for left child
					       pos 1 is for right child
					       */
dict_node *onechild(dict_node *); /* Return only onechild of a dict_node, this function is only usefull
				     for remove1
				     */
dict_node *mostleft(dict_node *); /* Return the node most in the left, if dict_node-l is NULL
					then return dict_node */
unsigned int avl_height(dict_node *); /* Returns 0 if dict_node is NULL else returns 1 + 
					 max(avl_height(dict_node->l), avl_height(dict_node->r))
					 */
unsigned int avl_len(dict_node *); /* Return the number of nodes */
dict_node *remove0(dict_node *); /* Remove a node without childs */
dict_node *remove1(dict_node *); /* Remove a node with only 1 child */
dict_node *remove2(dict_node *); /* Remove a node with 2 childs */
void llrotate(dict_node *); /* Make a left-left rotation 
			       30(20(10(a,b),c),d)
			       20(10(a,b),30(c,d))
				*/
void rrrotate(dict_node *); /* Make a right-right rotation
			       10(a,20(b,30(c,d)))
			       20(10(a,b),30(c,d))
				*/
void lrrotate(dict_node *);
void rlrotate(dict_node *);
int balance(dict_node *);
void rebalance(dict_node *);

/* Dictionary Functions to insert, delete and search a key 
 */

void dict_update(dict *, char *, dict_Data *);
dict_node *dict_search(dict *, char *);
void dict_remove(dict *, char *);

/* Create/Delete functions
 */
dict *newdict();
void free_dict_node(dict_node *);
void free_dict(dict *);

#endif
