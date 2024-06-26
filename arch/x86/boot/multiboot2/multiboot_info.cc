#include <x86/boot/multiboot2/multiboot_info.h>


namespace x86 {

/* Multiboot info tag types. */
enum MultibootInfoTagType : uint32_t {
	BootCommandLine = 1, // Tag containing the boot command line.
	BasicMemInfo = 4, // Tag containing basic memory info.
	MemoryMap = 6, // Tag containing memory mappings.
};

/* The first fixed fields in the multiboot info structure. */
struct MultibootInfoFixedPart {
	uint32_t total_size; // Total size of the multiboot info.
	uint32_t reserved; // Reserved, must be zero.
};

/* The first fields in every tag. */
struct MultibootInfoTagHead {
	uint32_t type; // Type of the tag.
	uint32_t size; // Size of the tag.
};


/* Multiboot info tag struct based on its type. */
template<MultibootInfoTagType type> struct MultibootInfoTag;

/* Boot command line tag struct. */
template<> struct MultibootInfoTag<BootCommandLine> {
	MultibootInfoTagHead head; // Tag head
	char *string; // Boot command line string
};

/* Basic memory info tag struct. */
template<> struct MultibootInfoTag<BasicMemInfo> {
	MultibootInfoTagHead head; // Tag head.
	uint32_t mem_lower; // Amount of lower memory in kilobytes.
	uint32_t mem_upper; // Amount of upper memory in kilobytes.
};

/* Memory map info tag struct. */
template<> struct MultibootInfoTag<MemoryMap> {
	MultibootInfoTagHead head; // Tag head.
	uint32_t entry_size; // Each entry size.
	uint32_t entry_version; // Entry version.
	MultibootInfo::MMap::Entry entries[0]; // Entries array.
};

/* Just converts the tag pointer to the specified type pointer. */
template<MultibootInfoTagType type> static const MultibootInfoTag<type> *
get_multiboot_info_tag(const MultibootInfoTagHead *tag_head)
{
	return reinterpret_cast<const MultibootInfoTag<type> *>(tag_head);
}


/* Read multiboot info tag given the type as a template argument. */
template<MultibootInfoTagType type> static void read_multiboot_info_tag_(
		const MultibootInfoTagHead *tag_head, MultibootInfo& info);

/* Read multiboot info tag with boot command line type. */
template<> void read_multiboot_info_tag_<BootCommandLine>(
		const MultibootInfoTagHead *tag_head, MultibootInfo& info)
{
	auto *tag = get_multiboot_info_tag<BootCommandLine>(tag_head);
	info.boot_cmd_line = tag->string;
}

/* Read multiboot info tag with basic memory info type. */
template<> void read_multiboot_info_tag_<BasicMemInfo>(
		const MultibootInfoTagHead *tag_head, MultibootInfo& info)
{
	auto *tag = get_multiboot_info_tag<BasicMemInfo>(tag_head);
	info.basic_mem_info.mem_lower = tag->mem_lower;
	info.basic_mem_info.mem_upper = tag->mem_upper;
}

/* Read multiboot info tag with memory map type. */
template<> void read_multiboot_info_tag_<MemoryMap>(
		const MultibootInfoTagHead *tag_head, MultibootInfo& info)
{
	auto *tag = get_multiboot_info_tag<MemoryMap>(tag_head);
	uint32_t entry_arr_mem_size = tag->head.size - sizeof(*tag);

	info.mmap.entry_version = tag->entry_version;
	info.mmap.nr_entries = entry_arr_mem_size / tag->entry_size;
	info.mmap.entries = tag->entries;
}


/* Read multiboot info tag with given type as function argument. */
static void read_multiboot_info_tag(const MultibootInfoTagHead *tag, MultibootInfo& info);
/* Get the tag pointer after the current one */
static const MultibootInfoTagHead *get_next_tag(const MultibootInfoTagHead *curr_tag);
/* Get padded size. */
static uint32_t padded_size(uint32_t size);


void read_multiboot_info(const void *tags_struct, MultibootInfo& info)
{
	// Get the fixed part.
	auto *fixed_part = static_cast<const MultibootInfoFixedPart *>(tags_struct);

	// Get the total_size.
	uint32_t total_size = fixed_part->total_size;
	// Fixed part is read. Decrease the total size.
	total_size -= sizeof(MultibootInfoFixedPart);

	// Get the first tag.
	auto *tag = reinterpret_cast<const MultibootInfoTagHead *>(fixed_part + 1);

	int i = 0;

	while (true) {
		read_multiboot_info_tag(tag, info); // read multiboot info tag
		const int tag_size = padded_size(tag->size);
		if (total_size > tag_size)
			total_size -= tag_size; // decrease the size by tag's size
		else
			break;
		tag = get_next_tag(tag); // update the tag pointer
	}
}


static void read_multiboot_info_tag(const MultibootInfoTagHead *tag, MultibootInfo& info)
{
	switch (tag->type) {
	case BootCommandLine:
		read_multiboot_info_tag_<BootCommandLine>(tag, info);
		break;
	case BasicMemInfo:
		read_multiboot_info_tag_<BasicMemInfo>(tag, info);
		break;
	case MemoryMap:
		read_multiboot_info_tag_<MemoryMap>(tag, info);
		break;
	}
}

inline const MultibootInfoTagHead *get_next_tag(const MultibootInfoTagHead *curr_tag)
{
	uintptr_t next_tag_addr = reinterpret_cast<uintptr_t>(curr_tag) + padded_size(curr_tag->size);
	if (next_tag_addr & 7)
		next_tag_addr = (1 + ((next_tag_addr) >> 3)) << 3;
	return reinterpret_cast<const MultibootInfoTagHead *>(next_tag_addr);
}

inline static uint32_t padded_size(uint32_t size)
{
	if (size & 7)
		return (1 + (size >> 3)) << 3;
	else
		return size;
}

}
