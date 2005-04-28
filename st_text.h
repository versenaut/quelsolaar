

extern uint		st_find_next_word(char *text);
extern boolean	st_find_word_compare(char *text_a, char *text_b);
extern uint		st_text_copy(uint length, char *dest, char *source);
extern uint		st_word_copy(uint length, char *dest, char *source);
extern char		*st_text_copy_allocate(char *source);
extern uint		st_text_copy_until(uint length, char *dest, char *source, char *until);
