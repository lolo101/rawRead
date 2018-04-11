#include <string.h>
#include <cstdlib>
#include <cstdio>

using namespace std;

const size_t SECTOR_SIZE = 512;

unsigned char toPrintableChar(unsigned char byte) {
    return byte >= 0x20 && byte < 0x7f ? byte : '.';
}

void display(unsigned char* buf, off_t offset, size_t bLen) {
    for(unsigned int i = 0; i < bLen >> 4; ++i) {
        printf("%08lx ", offset + i * 16);
        for(unsigned int j = 0; j < 8; ++j) {
            printf("%02hhx ", buf[i * 16 + j]);
        }
        printf(" ");
        for(unsigned int j = 8; j < 16; ++j) {
            printf("%02hhx ", buf[i * 16 + j]);
        }

        for(unsigned int j = 0; j < 8; ++j) {
            printf("%c", toPrintableChar(buf[i * 16 + j]));
        }
        printf(" ");
        for(unsigned int j = 8; j < 16; ++j) {
            printf("%c", toPrintableChar(buf[i * 16 + j]));
        }
        printf("\n");
    }
}

void checkArgs(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: rawRead [--start n] [--length n] <device>");
        printf("where n is the size in sector (0x200 bytes)");
        exit(1);
    }
}

char* parseDrive(int argc, char** argv) {
    return argv[argc - 1];
}

off_t parseStart(int argc, char** argv) {
    for(int n = 0; n < argc - 1; ++n) {
        char *arg = argv[n];
        if (strcmp(arg, "--start") == 0) {
            return strtol(argv[n + 1], NULL, 0);
        }
    }
    return 0;
}

size_t parseLength(int argc, char** argv) {
    for(int n = 0; n < argc - 1; ++n) {
        char *arg = argv[n];
        if (strcmp(arg, "--length") == 0) {
            return strtoul(argv[n + 1], NULL, 0);
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    checkArgs(argc, argv);
    FILE* drive = fopen(parseDrive(argc, argv), "r");
    if (drive == 0) {
        printf("%s not readable", argv[1]);
        exit(1);
    }

    off_t start = parseStart(argc, argv);
    off_t offset = SECTOR_SIZE * start;
    fseeko(drive, offset, SEEK_SET);

    size_t length = parseLength(argc, argv);
    size_t bLen = SECTOR_SIZE * length;
    unsigned char buf[bLen];
    fread(buf, SECTOR_SIZE, length, drive);

    fclose(drive);
    display(buf, offset, bLen);
    return 0;
}
