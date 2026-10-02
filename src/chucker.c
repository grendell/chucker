#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    RETURN_CODE_SUCCESS = 0,
    RETURN_CODE_INVALID_ARGS = 1,
    RETURN_CODE_INVALID_DLL = 2,
    RETURN_CODE_WRITE_FAILED = 3,
} returnCode_t;

const uint32_t EXPECTED_DLL_SIZE = 68244992u;

const uint32_t RELATIVE_VIRTUAL_ADDRESS = 0x2e8cafeu;
const uint32_t IL2CPP_SECTION_DELTA = 0x1200u;
const uint32_t DLL_OFFSET = RELATIVE_VIRTUAL_ADDRESS - IL2CPP_SECTION_DELTA;

const uint32_t PATCH_LENGTH = 12u;
const uint8_t SRC[] = {
    0x83u, 0xc4u, 0x08u,                      // add esp, 8
    0x83u, 0xf8u, 0x02u,                      // cmp eax, 2    ; Enemy
    0x0fu, 0x85u, 0x9eu, 0x02u, 0x00u, 0x00u, // jne 0x2e8cad8 ; +0x29e
};

const uint8_t DST[] = {
    0x59u,                                    // pop ecx
    0x59u,                                    // pop ecx
    0x66u, 0xa9u, 0x02u, 0x08u,               // test ax, 0x0802 ; Rival | Enemy
    0x0fu, 0x84u, 0x9eu, 0x02u, 0x00u, 0x00u, // je 0x2e8cad8    ; +0x29e
};

const char PATCHED_FILENAME[] = "GameAssembly_patched.dll";

uint8_t * readAndConfirmDll(const char * const filename) {
    fprintf(stdout, "Reading DLL file...\n");

    FILE * file = fopen(filename, "rb");

    if (!file) {
        fprintf(stderr, "Failed to open %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long s = ftell(file);

    if (s < 0l) {
        fprintf(stderr, "DLL size could not be verified\n");
        fclose(file);
        return NULL;
    }

    uint32_t size = (uint32_t) s;
    if (size != EXPECTED_DLL_SIZE) {
        fprintf(stderr, "Unexpected DLL file size: %u\n", size);
        fclose(file);
        return NULL;
    }

    uint8_t * dll = malloc(size);
    if (!dll) {
        fprintf(stderr, "Failed to allocate %s\n", filename);
        fclose(file);
        return NULL;
    }

    fseek(file, 0, SEEK_SET);
    if (fread(dll, 1, size, file) != size) {
        fprintf(stderr, "Failed to read %s\n", filename);
        fclose(file);
        free(dll);
        return NULL;
    }
    fclose(file);

    bool match = true;
    for (uint32_t i = 0u; i < PATCH_LENGTH; ++i) {
        if (dll[DLL_OFFSET + i] != SRC[i]) {
            match = false;
            break;
        }
    }

    if (!match) {
        fprintf(stderr, "Unexpected instruction in DLL\n");
        free(dll);
        return NULL;
    }

    fprintf(stdout, "\tVerified DLL.\n\n");
    return dll;
}

void patchDll(uint8_t * dll) {
    fprintf(stdout, "Updating game logic...\n");

    for (uint32_t i = 0u; i < PATCH_LENGTH; ++i) {
        dll[DLL_OFFSET + i] = DST[i];
    }

    fprintf(stdout, "\tDone.\n\n");
}

bool writePatchedDll(uint8_t * dll) {
    fprintf(stdout, "Writing patched DLL file...\n");

    FILE * file = fopen(PATCHED_FILENAME, "wb");

    if (!file) {
        fprintf(stderr, "Failed to open %s\n", PATCHED_FILENAME);
        return false;
    }

    if (fwrite(dll, 1, EXPECTED_DLL_SIZE, file) != EXPECTED_DLL_SIZE) {
        fprintf(stderr, "Failed to write %s\n", PATCHED_FILENAME);
        fclose(file);
        return false;
    }
    fclose(file);

    fprintf(stdout, "\tDone.\n");
    return true;
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <DLL file>\n", argv[0]);
        return RETURN_CODE_INVALID_ARGS;
    }

    uint8_t * dll = readAndConfirmDll(argv[1]);
    if (!dll) {
        return RETURN_CODE_INVALID_DLL;
    }

    patchDll(dll);

    if (!writePatchedDll(dll)) {
        free(dll);
        return RETURN_CODE_WRITE_FAILED;
    }

    free(dll);
    return RETURN_CODE_SUCCESS;
}