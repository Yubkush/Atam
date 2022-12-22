#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "elf64.h"

#define	ET_NONE	0	//No file type 
#define	ET_REL	1	//Relocatable file 
#define	ET_EXEC	2	//Executable file 
#define	ET_DYN	3	//Shared object file 
#define	ET_CORE	4	//Core file 

#define SYMTAB 0x2

#define STB_GLOBAL 1

/**
 * @brief Find symbol entry with the name symbol_name in symbol table. 
 * 
 * @param elf_header - Elf file header.
 * @param symbol_entry - Result symbol entry.
 * @param exe_file_name - Elf file name.
 * @param symbol_name - Name of symbol
 * 
 * @return 1 - Found global symbol.\
 * @return -1 - Symbol not found.\
 * @return -2 - No global symbol found, only local.
 */
int findSymbolEntry(Elf64_Ehdr *elf_header, Elf64_Sym *symbol_entry, char* exe_file_name, char *symbol_name) {
	FILE *fp = fopen(exe_file_name, "r");
	// prepare section headers
	Elf64_Shdr sections[elf_header->e_shnum];
	fseek(fp, elf_header->e_shoff, SEEK_SET);
	fread(&sections, 1, elf_header->e_shnum * elf_header->e_shentsize, fp);
	for (int i = 0; i < elf_header->e_shnum; i++) {
		if(sections[i].sh_type == SYMTAB) {
			Elf64_Shdr symtab = sections[i];
			Elf64_Shdr strtab = sections[symtab.sh_link];
			// find num of symbol table entries
			int symbol_num = symtab.sh_size / symtab.sh_entsize;
			// fill array of symbol table entries
			Elf64_Sym symbols[symbol_num];
			fseek(fp, symtab.sh_offset, SEEK_SET);
			fread(&symbols, 1, symtab.sh_size, fp);
			// fill string with all strtab
			char *strtab_strings = malloc(strtab.sh_size);
			fseek(fp, strtab.sh_offset, SEEK_SET);
			fread(strtab_strings, 1, strtab.sh_size, fp);
			fclose(fp);
			char *strings = strtab_strings;
			bool find_local_symbol_name = false;
			// iterate over symbol names and compare to symbol_name
			for (int i = 0; i < symbol_num; i++) {
				// if current string is the symbol name
				if(strcmp(strings + symbols[i].st_name, symbol_name) == 0) {
					*symbol_entry = symbols[i];
					unsigned char bind = ELF64_ST_BIND(symbol_entry->st_info);
					if(bind == STB_GLOBAL) {
						free(strtab_strings);
						return 1;
					}
					find_local_symbol_name = true;
				}
			}
			free(strtab_strings);
			if(find_local_symbol_name) {
				return -2;
			}
			return -1;	
		}
	}
	return -1;
}


/* symbol_name		- The symbol (maybe function) we need to search for.
 * exe_file_name	- The file where we search the symbol in.
 * error_val		- If  1: A global symbol was found, and defined in the given executable.
 * 			- If -1: Symbol not found.
 *			- If -2: Only a local symbol was found.
 * 			- If -3: File is not an executable.
 * 			- If -4: The symbol was found, it is global, but it is not defined in the executable.
 * return value		- The address which the symbol_name will be loaded to, if the symbol was found and is global.
 */
unsigned long find_symbol(char* symbol_name, char* exe_file_name, int* error_val) {
    FILE *fp = fopen(exe_file_name, "r");
	Elf64_Ehdr elf_header;
	fread(&elf_header, sizeof(uint8_t), 64, fp);
	fclose(fp);
    // error code -3, ELF type != ET_EXEC
	if (elf_header.e_type != ET_EXEC)
    {
        return -3;
    }
	
	Elf64_Sym symbol_entry;
	int res = findSymbolEntry(&elf_header, &symbol_entry, exe_file_name, symbol_name);
    return 0;
}

int main(int argc, char *const argv[]) {
	int err = 0;
	unsigned long addr = find_symbol(argv[1], argv[2], &err);

	if (addr > 0)
		printf("%s will be loaded to 0x%lx\n", argv[1], addr);
	else if (err == -2)
		printf("%s is not a global symbol! :(\n", argv[1]);
	else if (err == -1)
		printf("%s not found!\n", argv[1]);
	else if (err == -3)
		printf("%s not an executable! :(\n", argv[2]);
	else if (err == -4)
		printf("%s is a global symbol, but will come from a shared library\n", argv[1]);
	return 0;
}