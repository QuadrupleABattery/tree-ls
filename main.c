#include <stddef.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define ARGUMENT_COUNT 3
#define ARG_PATH 0
#define ARG_SHOW_HIDDEN 1
#define ARG_RECURSIVE 2

struct file_buffer {
	struct dirent **data;
	int size;
	int capacity;
};

struct file_buffer new_file_buffer(int capacity) {
	struct file_buffer buffer = {
		.data = malloc(capacity * sizeof(struct dirent)),
		.size = 0,
		.capacity = capacity,
	};

	return buffer;
}

void push_to_file_buffer(struct file_buffer *buffer, struct dirent *file) {
	if (buffer -> size >= buffer -> capacity) {
		buffer -> data = realloc(buffer -> data, 2 * buffer -> capacity * sizeof(struct dirent));
		buffer -> capacity = 2 * buffer -> capacity;
	}

	buffer -> data[buffer -> size] = file;
	buffer -> size++;
}

struct dirent *pop_from_file_buffer(struct file_buffer *buffer) {
	struct dirent *file = buffer -> data[buffer -> size - 1];

	buffer -> size--;

	return file;
}


void process_arguments(int argc, char **arguments, char *argFlags[ARGUMENT_COUNT]) {
	for (int arg = 1; arg < argc; arg++) {
		char *value = arguments[arg];

		if (value[0] != '-') {
			argFlags[ARG_PATH] = value;
		} else if (strcmp(value, "-r") == 0) {
			argFlags[ARG_RECURSIVE] = "1";
		} else if (strcmp(value, "-a") == 0) {
			argFlags[ARG_SHOW_HIDDEN] = "1";
		}
	}
}

void print_file(struct dirent *file, int indents) {
	printf("%s\n", file -> d_name);
}

void process_directory(char *path, int indents, char *argFlags[ARGUMENT_COUNT]) {
	int showHidden = strcmp(argFlags[ARG_SHOW_HIDDEN], "1") == 0;
	int recursive = strcmp(argFlags[ARG_RECURSIVE], "1") == 0;

	struct file_buffer buffer = new_file_buffer(2);
	struct dirent *nextChild;

	DIR *directory = opendir(path);

	while ((nextChild = readdir(directory)) != NULL) {
		char *fileName = nextChild -> d_name;
		
		if (!showHidden && fileName[0] == '.') {
			continue;
		}

		if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) {
			continue;
		}

		push_to_file_buffer(&buffer, nextChild);
	}

	for (int i = 0; i < buffer.size; i++) {
		struct dirent *file = buffer.data[i];

		print_file(file, indents);
	}
}


int main(int argc, char **argv) {
	char *argFlags[ARGUMENT_COUNT] = {".", "0", "0"};

	process_arguments(argc, argv, argFlags);

	process_directory(argFlags[ARG_PATH], 0, argFlags);
}
