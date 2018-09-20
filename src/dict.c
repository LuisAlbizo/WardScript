/* Dictionary Data Structure implementation file: dict.c
 * Luis Albizo "07-20-18"
 */
#include "dict.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* AVL Functions */

dict_node *newdict_node(char *key, dict_Data *d, dict_node *parent, dict_node *l, dict_node *r) {
	dict_node *n = malloc(sizeof(dict_node));
	if (!n) {
		printf("can't create new dict node\n");
		exit(1);
		return NULL;
	}
	strncpy(n->key, key, MAX_DICT_KEY);
	n->key[MAX_DICT_KEY-1] = '\x00';
	n->data = d;
	n->parent = parent;
	n->l = l;
	n->r = r;
	return n;
}
	/* Auxiliar Functions */

unsigned int nchilds(dict_node *n) {
	return (n->l ? 1 : 0) + (n->r ? 1 : 0);
}

int getpos(dict_node *n) {
	if(!n->parent)
		return -1;
	else if (strcmp(n->key, n->parent->key) > 0)
		return 1;
	else
		return 0;
}

void setpos(int pos, dict_node *root, dict_node *n) {
	switch (pos) {
		case 1:
			root->r = n;
			break;
		case 0:
			root->l = n;
			break;
		default:
			break;
	}
}

dict_node *onechild(dict_node *n) {
	if (n->l)
		return n->l;
	else
		return n->r;
}

dict_node *mostleft(dict_node *n) {
	if (!n->l)
		return n;
	else
		return mostleft(n->l);
}

unsigned int avl_height(dict_node *n) {
	if (!n)
		return 0;
	unsigned int lh = avl_height(n->l), rh = avl_height(n->r);
	if (lh > rh)
		return lh + 1;
	else
		return rh + 1;
}

unsigned int avl_len(dict_node *n) {
	if (!n)
		return 0;
	else
		return 1 + avl_len(n->l) + avl_len(n->r);
}

	/* Remove Functions */
/* NOTE: The functions remove0 and remove1 only work for non-root dict_nodes.
 *
 * This functions return the dict_node removed only, and does not free the 
 * memory or rebalance the tree.
 */  
dict_node *remove0(dict_node *n) {
	setpos(getpos(n), n->parent, NULL);
	return n;
}

dict_node *remove1(dict_node *n) {
	setpos(getpos(n), n->parent, onechild(n));
	return n;
}

dict_node *remove2(dict_node *n) {
	dict_node *ml = mostleft(n->r);
	dict_Data *d = ml->data;
	if (nchilds(ml) == 1)
		remove1(ml);
	else
		remove0(ml);
	n->data = d;
	return ml;
}

	/* Rotation Functions */

void llrotate(dict_node *n) {
	dict_node *aux = n->l;
	aux->parent = n->parent;
	n->parent = aux;
	n->l = aux->r;
	aux->r = n;
	if (aux->parent)
		setpos(getpos(aux), aux->parent, aux);
}

void rrrotate(dict_node *n) {
	dict_node *aux = n->r;
	aux->parent = n->parent;
	n->parent = aux;
	n->r = aux->l;
	aux->l = n;
	if (aux->parent)
		setpos(getpos(aux), aux->parent, aux);
}

void lrrotate(dict_node *n) {
	//60(30(_,40(_,_)),_)
	//40(30(_,_),60(_,_))
	rrrotate(n->l);
	llrotate(n);
}

void rlrotate(dict_node *n) {
	//30(_,60(40(_,_),_))
	//40(30(_,_),60(_,_))
	llrotate(n->r);
	rrrotate(n);
}

	/* Balance Functions */

int balance(dict_node *n) {
	return avl_height(n->r) - avl_height(n->l);
}

void rebalance(dict_node *n) {
	int fe = balance(n);
	if (fe > 1 && fe < -1) {
		int path[2];
		for (int i = 0; i < 2; i++) {
			if (fe < -1) {
				path[i] = 0;
				fe = balance(n->l);
			} else {
				path[i] = 1;
				fe = balance(n->r);
			}
		}
		if (path[0] == 0) {
			if (path[1] == 0)
				llrotate(n);
			else
				lrrotate(n);
		} else {
			if (path[1] == 0)
				rlrotate(n);
			else
				rrrotate(n);
		}
	} else {
		if (!n->parent)
			return;
		else
			rebalance(n->parent);
	}
}

/* Dict Functions */

void dict_update(dict *d, char *key, dict_Data *v) {
	/* Update value if key exists else inserts a new key-value on the dict.
	 */
	if (!d->root) {
		d->root = newdict_node(key, v, NULL, NULL, NULL);
		return;
	}
	dict_node *curdict_node = d->root;
	while (1) {
		if (strcmp(key, curdict_node->key) < 0) {
			if (curdict_node->l)
				curdict_node = curdict_node->l;
			else {
				dict_node *n = newdict_node(key, v, curdict_node, NULL, NULL);
				curdict_node->l = n;
				break;
			}
		} else if (strcmp(key, curdict_node->key) > 0) {
			if (curdict_node->r)
				curdict_node = curdict_node->r;
			else {
				dict_node *n = newdict_node(key, v, curdict_node, NULL, NULL);
				curdict_node->r = n;
				break;
			}
		} else {
			curdict_node->data = v;
			break;
		}
	}
}

dict_node *dict_search(dict *d, char *key) {
	if (!d->root)
		return NULL;
	dict_node *curdict_node = d->root;
	while (1) {
		if (strcmp(key, curdict_node->key) < 0) {
			if (curdict_node->l)
				curdict_node = curdict_node->l;
			else {
				break;
			}
		} else if (strcmp(key, curdict_node->key) > 0) {
			if (curdict_node->r)
				curdict_node = curdict_node->r;
			else {
				break;
			}
		} else {
			return curdict_node;
		}
	}
	return NULL;
}

void dict_remove(dict *d, char *key) {
	/* This functions search the dict_node by key, and if it found the dict_node
	 * then remove the dict_node, rebalance the tree and free the memory of the dict_node.
	 */
	dict_node *match = dict_search(d, key);
	if (!match)
		return;
	if (!match->parent) { // in case match is root
		switch (nchilds(match)) {
			case 0:
				free(d->root->data);
				free(d->root);
				d->root = NULL;
				break;
			case 1:
				free(d->root->data);
				d->root = onechild(d->root);
				free(match);
				break;
			case 2:
				match = remove2(d->root);
				free(match);
				break;
			default:
				break;
		}
	} else {
		switch (nchilds(match)) {
			case 0:
				match = remove0(match);
				free(match->data);
				free(match);
				break;
			case 1:
				match = remove1(match);
				free(match->data);
				free(match);
				break;
			case 2:
				match = remove2(match);
				free(match);
				break;
			default:
				break;
		}
	}
}

dict *newdict() {
	dict *d = malloc(sizeof(dict));
	if (!d) {
		printf("memory insuficient for dict alloc\n");
		exit(1);
	}
	return d;
}

void free_dict_node(dict_node *n) {
	/* Free the memory recursively, starting on the leaves,
	 * and up to the dict_node *n
	 */
	if (!n)
		return;
	free(n->key);
	free(n->data);
	free_dict_node(n->l);
	free_dict_node(n->r);
	free(n);
}

void free_dict(dict *d) {
	/* Free all the tree then free the dict
	 */
	free_dict_node(d->root);
	free(d);
}

