#ifndef datatypes_h
#define datatypes_h

#define TY_NUMBER 1
#define TY_STRING 2
#define TY_FUNCTION 3
#define TY_LIST 4
#define TY_DICT 5

struct bw_Type {
	unsigned int datatype;
};

struct bw_String {
	unsigned int datatype;
	char *value;
};

struct bw_Number {
	unsigned int datatype;
	double value;
}:

struct bw_List {
	unsigned int datatype;
	struct bw_Type **items;
}:

struct bw_Dict {
	unsigned int datatype;
	dict *t;
}:

struct bw_Function {
	unsigned int datatype;
	char *return_name;
	char **arg_names;
	st_block *code;
}:

typedef struct bw_Type		bw_Type;
typedef struct bw_String	bw_String;
typedef struct bw_Number	bw_Number;
typedef struct bw_List		bw_List;
typedef struct bw_Dict		bw_Dict;
typedef struct bw_Function	bw_Function;


#endif
