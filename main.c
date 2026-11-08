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

void color_print(char *text, char *color) {
	printf("%s%s", color, text);
}

void repeat_print(int repeats, char *text, char *color) {
	for (int i = 0; i < repeats; i++) {
		color_print(text, color);
	}
}

void process_directory(char *path, int indents, char *argFlags[ARGUMENT_COUNT], int directoryLines) {
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
		char *fileName = file -> d_name;
		
		repeat_print(indents - 1, "┃ ", "\e[0;36m");

		if (i == buffer.size - 1) {
			color_print("┗━", "\e[0;36m");
		} else {
			color_print("┣━", "\e[0;36m");
		}
		color_print(fileName, "\e[0;37m");
		printf("\n");

		if (file -> d_type == DT_DIR) {
			int pathLength = strlen(path) + strlen(fileName) + 2; // +2 for null terminators
			char subDirectoryPath[pathLength];

			snprintf(subDirectoryPath, pathLength, "%s/%s", path, fileName);
			process_directory(subDirectoryPath, indents + 1, argFlags);
		}
	}
}


int main(int argc, char **argv) {
	char *argFlags[ARGUMENT_COUNT] = {".", "0", "0"};
	

	process_arguments(argc, argv, argFlags);
	color_print(argFlags[ARG_PATH], "\e[0;36m");
	printf("\n");
	color_print("┃", "\e[0;36m");
	printf("\n");
	process_directory(argFlags[ARG_PATH], 1, argFlags);

	return 0;
}
