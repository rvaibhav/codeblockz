struct dictionary{
	string word<>;
	string meaning<>;
};

struct lookupword{
	string word<>;
	string meaning<>;
};

struct resultlong{
	long success;
};

program DICTIONARY_PROG {
	version DICTIONARY_VERS {
		resultlong INSERT(dictionary) = 1;
		lookupword LOOKUP(dictionary) = 2;
		resultlong DELETEWORD(dictionary) = 3;
	} = 1;
} = 0x31240000;
