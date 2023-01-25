#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <fcntl.h>
#include <sys/mman.h>
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
#define SHT_DYNSYM 11

#define STB_GLOBAL 1

#define SHN_UNDEF 0

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
	Elf64_Shdr sections[elf_header->e_shnum]; // section table
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
			for (int j = 0; j < symbol_num; j++) {
				// if current string is the symbol name
				if(strcmp(strings + symbols[j].st_name, symbol_name) == 0) {
					*symbol_entry = symbols[j];
					unsigned char bind = ELF64_ST_BIND(symbol_entry->st_info);
					if(bind == STB_GLOBAL) {
						free(strtab_strings);
						if(symbol_entry->st_shndx == SHN_UNDEF) {
							return -4;
						}
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

Elf64_Addr findRelaPltFunc(Elf64_Ehdr *elf_header, char* exe_file_name, char *symbol_name) {
    Elf64_Sym* dynsym;
    char* shstrtab;
	char* strtab;
    unsigned long symbol_index;

    // Open ELF
    int fd = open(exe_file_name, O_RDONLY);
    if (fd == -1) {
        printf("Failed to open file\n");
        exit(1);
    }
    // Map the ELF file into memory
    void *file_memory = mmap(NULL, lseek(fd, 0, SEEK_END), PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_memory == MAP_FAILED) {
        printf("Failed to map file into memory\n");
        exit(1);
    }

    // Get the section headers
    Elf64_Shdr* section_headers;
	section_headers = (Elf64_Shdr*)(file_memory + elf_header->e_shoff);

    // Iterate through the section headers
    for (int i = 0; i < elf_header->e_shnum; i++) {
        // Check if the section is the dynsym section
        if (section_headers[i].sh_type == SHT_DYNSYM) {
            dynsym = (Elf64_Sym*)(file_memory + section_headers[i].sh_offset);
            shstrtab = (char*)(file_memory + section_headers[elf_header->e_shstrndx].sh_offset);
			strtab = (char*)(file_memory + section_headers[section_headers[i].sh_link].sh_offset);
            break;
        }
    }
    
	// Iterate through the section headers to find 
	Elf64_Rela* rela_plt;
	int rela_idx = 0;
    for (int i = 0; i < elf_header->e_shnum; i++) {
        // Check  if the section is the .rela.plt section
        if (strcmp(".rela.plt", shstrtab + section_headers[i].sh_name) == 0) {
            rela_plt = (Elf64_Rela*)(file_memory + section_headers[i].sh_offset);
			rela_idx = i;
            break;
        }
    }
    
	// Iterate through the relocations
	Elf64_Addr func_addr;
    for (int i = 0; i < section_headers[rela_idx].sh_size / sizeof(Elf64_Rela); i++) {
        // Get the symbol index from the relocation entry
        symbol_index = ELF64_R_SYM(rela_plt[i].r_info);
        // Lookup the symbol name in the dynsym table
		char *name = strtab + dynsym[symbol_index].st_name;
        if(strcmp(symbol_name, strtab + dynsym[symbol_index].st_name) == 0) {
            func_addr = rela_plt[i].r_offset;
            break;
        }
	}
    munmap(file_memory, lseek(fd, 0, SEEK_END));
    close(fd);
    return func_addr;
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
	if (elf_header.e_type != ET_EXEC) {
        *error_val = -3;
		return 0;
    }
	
	Elf64_Sym symbol_entry;
	*error_val = findSymbolEntry(&elf_header, &symbol_entry, exe_file_name, symbol_name);
	if(*error_val < 0 && *error_val != -4) {
		return 0;
	}
	else if(*error_val == -4) {
		return findRelaPltFunc(&elf_header, exe_file_name, symbol_name);
	}
    return (unsigned long)symbol_entry.st_value;
}

pid_t run_target(char* argv[])
{
	pid_t pid;
	pid = fork();
	
    if (pid > 0) {
		return pid;
		
    } else if (pid == 0) {
		/* Allow tracing of this process */
		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
			perror("ptrace");
			exit(1);
		}
		/* Replace this process's image with the given program */
		execv(argv[2], argv + 2);	
	}
	return 0;
}

void run_debugger(pid_t child_pid, char *symbol_name, char *exec_name)
{
    int wait_status;
    struct user_regs_struct regs;
	int counter = 1;
	int err = 1;
	unsigned long long base = 0;
	long ret_addr = -1;
	long ret = -1;
	long ret_trap = -1;

    /* Wait for child to stop on its first instruction */
    wait(&wait_status);

    /* find symbol address we're interested in */
    unsigned long func_addr = find_symbol(symbol_name, exec_name, &err);
	unsigned long got_addr = func_addr;
	if (err == -2) {
		printf("PRF:: %s is not a global symbol! :(\n", symbol_name);
		return;
	}
	else if (err == -1) {
		printf("PRF:: %s not found!\n", symbol_name);
		return;
	}
	else if(err == -4){
		func_addr = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)func_addr, NULL);
	}

	unsigned long data = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)func_addr, NULL);
	unsigned long data_trap = (data & 0xFFFFFFFFFFFFFF00) | 0xCC;
	// put break point at function
	ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)data_trap);

	/* Let the child run to the breakpoint and wait for it to reach it */
	ptrace(PTRACE_CONT, child_pid, NULL, NULL);
	wait(&wait_status);

	while(WIFSTOPPED(wait_status)) {
		/* See where the child is now */
		ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

		/* Remove the breakpoint by restoring the previous data */
		ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)data);
		regs.rip -= 1;
		ptrace(PTRACE_SETREGS, child_pid, 0, &regs);
		
		// return address of the function that is at the top of the stack
		if(base == 0) {
			base = regs.rsp;
			ret_addr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.rsp, NULL);

			// Place breakpoint at the return address
			ret = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)ret_addr, NULL);
			ret_trap = (ret & 0xFFFFFFFFFFFFFF00) | 0xCC;
		}
		ptrace(PTRACE_POKETEXT, child_pid, (void*)ret_addr, (void*)ret_trap);

		// run function and recursive calls
		ptrace(PTRACE_CONT, child_pid, NULL, NULL);
		wait(&wait_status);

		// function finished
		ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
		/* Remove the return addres breakpoint*/
		ptrace(PTRACE_POKETEXT, child_pid, (void*)ret_addr, (void*)ret);
		regs.rip -= 1;

		if(base < regs.rsp){
			int ret_val = (int)regs.rax;
			printf("PRF:: run #%d returned with %d\n",counter, ret_val);
			counter++;
			base = 0;
			ptrace(PTRACE_SETREGS, child_pid, 0, &regs);
		}
		else {
			ptrace(PTRACE_SETREGS, child_pid, 0, &regs);
			ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL);
			wait(&wait_status);
			ptrace(PTRACE_POKETEXT, child_pid, (void*)ret_addr, (void*)ret_trap);
		}


		if(err == -4) { // update addr after GOT has the function addr
			func_addr = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)got_addr, NULL);
			data = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)func_addr, NULL);
			data_trap = (data & 0xFFFFFFFFFFFFFF00) | 0xCC;
		}

		// put break point at function again
		ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)data_trap);

		/* The child can continue running now */
		ptrace(PTRACE_CONT, child_pid, 0, 0);

		wait(&wait_status);
	}
	// remove break point
	ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)data);
}

int main(int argc, char* argv[])
{
	int err = 0;
	unsigned long addr = find_symbol(argv[1], argv[2], &err);

	if(err == -3) {
		printf("PRF:: %s not an executable! :(\n", argv[2]);
		return 0;
	}

    pid_t child_pid;

    child_pid = run_target(argv);
	
	// run specific "debugger"
	if(child_pid > 0)
		run_debugger(child_pid, argv[1], argv[2]);

    return 0;
}