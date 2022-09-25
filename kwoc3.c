/*
 * kwoc3.c
 *
 * Starter file provided to students for Assignment #3, SENG 265,
 * Spring 2020
 */
#define _GNU_SOURCE
#define MAX_KEYWORD_LEN 40
#define MAX_LINE_LEN 100


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include "emalloc.h"
#include "listy.h"


void print_word(node_t *node, void *arg)
{
    char *format = (char *)arg;
    printf(format, node->text);
}

//struct that stores an array, its current capacity and max capacity
struct array_plus_line {
	char **array;
	int num_elements;
	int max_elements;
};

//struct that stores a boolean and the current word to be checked
struct input_check {
	int *boolean;
	char word[MAX_KEYWORD_LEN];
};

//comparator function for apply
void cmp(node_t *node, void *arg) {
	//arg is a void* pointer that is cast to be a struct * pointer
	struct input_check *temp = (struct input_check *) arg;
	//compare the word in the current node with the word we are checking
	if (strcmp(node->text, temp->word) == 0) {
		//change the pointer to our flag
		*(temp->boolean) = 0;
	}
}

void *ecalloc(int num, size_t size_of_num) {
	void *p;
	p = calloc(num, size_of_num);
	if (p == NULL) {
		printf("calloc failed\n");
		exit(1);
	}
	
	return p;
	
}

//function that doubles the memory in a 2d array once an array is full
void grow_and_copy(struct array_plus_line *my_struct) {
	my_struct->array = (char **) realloc(my_struct->array, 2 * my_struct->max_elements * MAX_LINE_LEN * sizeof(char));
	for (int i = my_struct->max_elements; i < my_struct->max_elements * 2; i++) {
		my_struct->array[i] = (char *) ecalloc(MAX_LINE_LEN, sizeof(char));
	}
	my_struct->max_elements = my_struct->max_elements * 2;
}

//create a 2d character array that stores the input file, line by line
//takes in pointers to a filename and a struct to store the file line by line
void by_lines(struct array_plus_line *array_by_line, FILE *file_name) {
	size_t buffer_len;
	char *buffer;
	buffer = NULL;
	array_by_line->array = (char **) ecalloc(10, sizeof(char*));
	array_by_line->num_elements = 0;
	//10 rows to start, double as needed
	for (int i = 0; i < 10; i++) {
		array_by_line->array[i] = (char *) ecalloc(MAX_LINE_LEN, sizeof(char));
	}
	char* each_line;
	int line_size;
	array_by_line->max_elements = 10;
	line_size = getline(&buffer, &buffer_len, file_name);
	while(line_size >= 0) {
		each_line = (char *) ecalloc((strlen(buffer) + 1), sizeof(char));
		strncpy(each_line, buffer, strlen(buffer + 1));
		// check whether array_by_line is full
		if (array_by_line->num_elements >= array_by_line->max_elements) {
			grow_and_copy(array_by_line);
		}
		//at this point we know our array has room, and can add the line to it
		strncpy(array_by_line->array[array_by_line->num_elements], each_line, strlen(each_line) + 1);
		array_by_line->num_elements++;
		line_size = getline(&buffer, &buffer_len, file_name);
		free(each_line);
	}
	free(buffer);
	
}

//create a linked list that contains all exception words
node_t * get_exception(FILE *file_name) {
	size_t buffer_len;
	char * buffer;
	buffer = NULL;
	char * each_word;
	struct node_t *temp_node = NULL;
	struct node_t *head = NULL;
	int line_size = getline(&buffer, &buffer_len, file_name);
	while (line_size >= 0) {
		each_word = (char *) ecalloc(strlen(buffer) + 1, sizeof(char));
		strncpy(each_word, buffer, strlen(buffer + 1));
		temp_node = new_node(each_word);
      head = add_end(head, temp_node);
      line_size = getline(&buffer, &buffer_len, file_name);
      free(each_word);
	}
	free(buffer);
	return head;
}

//create a 2d character array that stores only the unique keywords
//takes in the struct created from by_lines, the linked list created from get_exception, and creates the unique keywords struct
void create_unique(struct array_plus_line *infile, node_t *exception, struct array_plus_line *to_make) {
	to_make->array = (char **) ecalloc(10, sizeof(char*));
	to_make->num_elements = 0;
	int flag = 1;
	struct input_check check;
	check.boolean = &flag;
	char buffer[MAX_LINE_LEN];
	//10 rows to start, double as needed
	to_make->max_elements = 10;
	for (int i = 0; i < 10; i++) {
		to_make->array[i] = (char *) ecalloc(MAX_KEYWORD_LEN, sizeof(char));
	}
	//keep track of the line number
	for (int i = 0; i < infile->num_elements; i++) {
		//create a copy
		strncpy(buffer, infile->array[i], MAX_LINE_LEN);
		//keep track of the current word in the current line
		char *token;
		//go through the current line and strtok to isolate each individual word
		token = strtok(buffer, " ");
    	while (token != NULL) {
    		strncpy(check.word, token, strlen(token) + 1);
    		//go through what we already have in the unique word array
    		for (int k = 0; k < to_make->num_elements; k++) {
    			if (strcmp(token, to_make->array[k]) == 0) {
    				flag = 0;
    			}
    		}
    		//go through exclusion linked list
    		apply(exception, cmp, &check);

    		//either the word is to be excluded or it is a duplicate
    		if (flag == 0) {
    			flag = 1;
    		}
    		//add it to the unique array
    		else {
    			//double if needed
    			if (to_make->num_elements >= to_make->max_elements) {
					grow_and_copy(to_make);
				}
				//confirmed that we have enough room
    			strncpy(to_make->array[to_make->num_elements], token, strlen(token) + 1);
      		to_make->num_elements++;
    		}
    	token = strtok(NULL, " ");
   	}
	}
}

//change any 2d character array to be lowercase
void to_lower(struct array_plus_line *temp) {
	for (int i = 0; i < temp->num_elements; i++) {
		for (int j = 0; j < strlen(temp->array[i]); j++) {
			temp->array[i][j] = tolower(temp->array[i][j]);
		}
	}
}

//change any 2d character array to be uppercase
void to_upper(struct array_plus_line *temp) {
	for (int i = 0; i < temp->num_elements; i++) {
		for (int j = 0; j < strlen(temp->array[i]); j++) {
			temp->array[i][j] = toupper(temp->array[i][j]);
		}
	}
}


//sort in lexicographical order
void sort(struct array_plus_line *my_struct) {
	char temp [MAX_KEYWORD_LEN];
	for (int i = 0; i < my_struct->num_elements; ++i) {
   	for (int j = i + 1; j < my_struct->num_elements; ++j) {
   		if (strcmp(my_struct->array[i], my_struct->array[j]) > 0) {
				strncpy(temp, my_struct->array[i], MAX_KEYWORD_LEN);
				strncpy(my_struct->array[i], my_struct->array[j], MAX_KEYWORD_LEN);
				strncpy(my_struct->array[j], temp, MAX_KEYWORD_LEN);
      }	  
    }
  }
}

//find the length of the longest unique word
int find_longest(struct array_plus_line *to_check) {
	int temp_max = 0;
	for (int i = 0; i < to_check->num_elements; i++) {
   	if(strlen(to_check->array[i]) > temp_max) {
      	temp_max = strlen(to_check->array[i]);
      }
	}
	return temp_max;
}

//go through a single line and find how many times a certain word occurs
int num_occurences(struct array_plus_line *my_struct, char *word, int current_index) {
	char buffer[MAX_LINE_LEN];
	char *token;
	int num = 0;
	strncpy(buffer, my_struct->array[current_index], MAX_LINE_LEN);
	token = strtok(buffer, " ");
	while (token != NULL) {
		if (strcmp(word, token) == 0) {
			num++;
		}
		token = strtok(NULL, " ");
	}
	return num;
}

void to_console(struct array_plus_line *line_print, struct array_plus_line *line_compare, struct array_plus_line *unique_compare, struct array_plus_line *unique_print, int max) {
	//go through unique words
	for (int i = 0; i < unique_compare->num_elements; i++) {
		int current_word_len = strlen(unique_compare->array[i]);
		//go through each line;
		for (int j = 0; j < line_compare->num_elements; j++) {
			//pass in the pointer to the line_struct, a word from unique, and the current index of the line_struct
			int count_per_line = num_occurences(line_compare, unique_compare->array[i], j);
			//we print here
			if (count_per_line != 0) {
      		printf("%s", unique_print->array[i]);
      		for (int k = 0; k < (max + 2 - current_word_len); k++) {
					printf(" ");
      		}		
      		printf("%s (%d", line_print->array[j], j + 1);
      		if (count_per_line > 1) {
					printf("*");
      		}
      		printf(")\n");
    		}
		}
	}
}

//free the memory in the struct passed in
void free_array_memory(struct array_plus_line *my_struct) {
	for (int i = 0; i < my_struct->max_elements; i++) {
		free(my_struct->array[i]);
	}
	free(my_struct->array);
}






int main(int argc, char *argv[]) {
	//obtain exclusion and input files
    char *exception_file = NULL;
    char *input_file = NULL;
    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 && i+i <= argc) {
            exception_file = argv[++i];
        } else {
            input_file = argv[i];
        }
    }



	//line_keep, line_lower, unique_struct, unique_upper are created
	//line_lower is compared with unique_struct (both in lowercase)
	//line_keep and upper_unique are for printing
	FILE *in = fopen(input_file, "r");
	FILE *in_two = fopen(input_file, "r");
	struct array_plus_line line_keep;
	struct array_plus_line line_lower;
	struct array_plus_line unique_struct;
	struct array_plus_line unique_upper;
	by_lines(&line_keep, in);
	by_lines(&line_lower, in_two);
	FILE *exception = fopen(exception_file, "r");
	node_t *exception_head;
	if (exception_file == NULL) {
		exception_head = NULL;
	}
	else {
		exception_head = get_exception(exception);
	}

	//call functions
	to_lower(&line_lower);
	create_unique(&line_lower, exception_head, &unique_struct);
	create_unique(&line_lower, exception_head, &unique_upper);
	to_upper(&unique_upper);
	sort(&unique_struct);
	sort(&unique_upper);
	int max_len = find_longest(&unique_struct);
	to_console(&line_keep, &line_lower, &unique_struct, &unique_upper, max_len);
	

	//free memory
	free_array_memory(&line_keep);
	free_array_memory(&line_lower);
	free_array_memory(&unique_struct);
	free_array_memory(&unique_upper);
	
	node_t *temp_node = exception_head;
	while (temp_node != NULL) {
   	assert(temp_node != NULL);
   	exception_head = remove_front(exception_head);
   	free(temp_node);
   	temp_node = exception_head;
	}
    assert(exception_head == NULL);
	
	//close file handlers
	fclose(in);
	fclose(in_two);
	if (exception != NULL) {
		fclose(exception);
	}
	
	
	exit(0);
}
