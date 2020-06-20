#define IAR_MAGIC 0x1A4C1A4C1A4C1A4C

typedef struct {
	uint64_t magic;
	uint64_t version;
	uint64_t root_node_offset;
	uint64_t page_bytes; // this isn't really needed for the KOS, as each file is loaded in its entirety in memory using pread anyway, so knowing if we can use mmap on this platform is useless
	                     // HOWEVER, some devices may rely on files previously loaded over here, so it still is important to include this
} iar_header_t;

typedef struct {
	uint64_t node_count;
	uint64_t node_offsets_offset;
	
	uint64_t name_bytes;
	uint64_t name_offset;
	
	uint64_t data_bytes;
	uint64_t data_offset;
} iar_node_t;

typedef struct {
	FILE* fp;
	int fd;
	
	iar_header_t header;
	iar_node_t root_node;
} iar_file_t;

int iar_open(iar_file_t* self, const char* path) {
	self->fp = fopen(path, "r");
	if (!self->fp) {
		printf("ERROR Failed to read %s\n", path);
		return 1;
	}
	
	self->fd = fileno(self->fp);
	pread(self->fd, &self->header, sizeof(self->header), 0);
	
	if (self->header.magic != IAR_MAGIC) {
		printf("ERROR %s is not a valid IAR file\n", path);
		fclose(self->fp);
		return 1;
	}
	
	if (self->header.version != 0) {
		printf("ERROR %s is of an invalid version (%lu) (AQUA only supports IAR version 0)\n", path, self->header.version);
		fclose(self->fp);
		return 1;
	}
	
	pread(self->fd, &self->root_node, sizeof(self->root_node), self->header.root_node_offset);
	return 0;
}

uint64_t iar_find_node(iar_file_t* self, iar_node_t* node, const char* name, iar_node_t* parent) { // return file index of found file or -1 if nothing found, 
	uint64_t node_offsets_bytes = parent->node_count * sizeof(uint64_t);
	uint64_t* node_offsets = (uint64_t*) malloc(node_offsets_bytes);
	pread(self->fd, node_offsets, node_offsets_bytes, parent->node_offsets_offset);
	
	uint64_t found = -1;
	for (int i = 0; i < parent->node_count; i++) {
		iar_node_t child_node;
		pread(self->fd, &child_node, sizeof(child_node), node_offsets[i]);
		
		char* node_name = (char*) malloc(child_node.name_bytes);
		pread(self->fd, node_name, child_node.name_bytes, child_node.name_offset);
		
		uint8_t condition = strncmp(name, node_name, sizeof(node_name)) == 0;
		free(node_name);
		
		if (condition) {
			memcpy(node, &child_node, sizeof(child_node));
			found = i;
			break;
		}
	}
	
	free(node_offsets);
	return found;
}

int iar_read_node_contents(iar_file_t* self, iar_node_t* node, char* buffer) {
	if (node->data_offset) {
		pread(self->fd, buffer, node->data_bytes, node->data_offset);
		return 0;
		
	} else {
		printf("ERROR Provided node is not a file and thus contains no data\n");
		return 1;
	}
}

void iar_free(iar_file_t* self) {
	fclose(self->fp);
}
