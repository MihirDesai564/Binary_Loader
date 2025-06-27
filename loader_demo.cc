#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include "loader.h"


int 
main(int argc, char *argv[])
{
    size_t i;
    Binary bin;
    Section *sec;
    Symbol *sym;
    std::string fname;

    if(argc < 2){
        printf("Usage for the base view: %s <binary>\n", argv[0]);
        printf("Usage for extended section view: %s <binary> -s <name of section>\n", argv[0]);
        return 1;
    }

    fname.assign(argv[1]);
    if(load_binary(fname, &bin, Binary::BIN_TYPE_AUTO) < 0){
        return 1;
    }

    if(argc == 4 && strcmp(argv[2], "-s") == 0){
        bool found = false;
        for(i = 0; i < bin.sections.size(); i++){
            sec = &bin.sections[i];
            if (sec->name == std::string(argv[3])){
                printf("Section: %-10s @ 0x%08lx, size = %zu bytes\n", 
                    sec->name.c_str(), sec->vma, sec->size);
                    
                for(size_t off = 0; off < sec->size; ++off){
                    if (off % 16 == 0){
                        printf("  0x%08lx  ", sec->vma + off);
                    }
                    printf("%02x", (unsigned char)sec->bytes[off]);
                    if (off % 16 == 15 || off+1 == sec->size){
                        printf("\n");
                    }
                }
                found = true;
                break;
                }
            }
        if (found){
            printf("End of section dump.\n");
        } else {
            printf("Section '%s' not found\n", argv[3]);
        }
        unload_binary(&bin);
        printf("Section view completed.\n");
        return 0;
    }

    
    printf("loaded binary '%s' %s%s (%u bits) entry@0x%016jx\n", 
        bin.filename.c_str(), 
        bin.type_str.c_str(), bin.arch_str.c_str(), 
        bin.bits, bin.entry);

    for(i = 0; i < bin.sections.size(); i++){
        sec = &bin.sections[i];
        printf("  0x%016jx %-8ju %-20s %s\n",
            sec->vma, sec->size, sec->name.c_str(),
            sec->type == Section::SEC_TYPE_CODE ? "CODE" : "DATA"
        );
    }

    if(bin.symbols.size() > 0){
        printf("scanned symbol tables\n");
        for(i = 0; i < bin.symbols.size(); i++){
            sym = &bin.symbols[i];
            printf("  %-40s 0x%016jx %s\n", 
                sym->name.c_str(), sym->addr,
                (sym->type == Symbol::SYM_TYPE_FUNC) ? "FUNC" : "");
        }
    }

    unload_binary(&bin);

    return 0;
}