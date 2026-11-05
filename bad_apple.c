#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <termios.h>

#define MAX_FRAMES 10000
#define MAX_PATH 1024
#define FRAME_DELAY_US 33323

// Numeric string comparison for natural sorting (e.g., frame2 comes before frame10)
int natural_sort_cmp(const void *a, const void *b) {
    const char *str1 = *(const char **)a;
    const char *str2 = *(const char **)b;
    
    while (*str1 && *str2) {
        if ((*str1 >= '0' && *str1 <= '9') && (*str2 >= '0' && *str2 <= '9')) {
            long val1 = strtol(str1, (char **)&str1, 10);
            long val2 = strtol(str2, (char **)&str2, 10);
            if (val1 != val2) {
                return (val1 > val2) - (val1 < val2);
            }
        } else {
            if (*str1 != *str2) {
                return (unsigned char)*str1 - (unsigned char)*str2;
            }
            str1++;
            str2++;
        }
    }
    return (unsigned char)*str1 - (unsigned char)*str2;
}

int main(int argc, char *argv[]) {
    // Check for help flag
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        printf("\nUsage: %s [-h|--help]\n", argv[0]);
        printf("  Plays ASCII animation of Bad Apple!! from 'frames-ascii' directory.\n\n");
        return 0;
    }

    // Audio prompt
    char choice;
    printf("\nDo you want to use mpv to play sound? You need mpv installed to do that. (y/n): ");
    fflush(stdout);
    if (scanf(" %c", &choice) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    if (choice == 'y' || choice == 'Y') {
        // Check if mpv is available
        if (system("command -v mpv > /dev/null 2>&1") != 0) {
            fprintf(stderr, "\nmpv is not installed. Please install it to use this feature.\n\n");
            return 1;
        }
        // Play audio in background
        system("mpv ./bad_apple.mp3 > /dev/null 2>&1 &");
    }

    const char *frames_dir = "./frames-ascii";
    DIR *dir = opendir(frames_dir);
    if (!dir) {
        fprintf(stderr, "Error: Frames directory not found at %s\n", frames_dir);
        return 1;
    }

    // Scan directory for files
    struct dirent *entry;
    char *filenames[MAX_FRAMES];
    int frame_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue; // Skip hidden files / '.' / '..'
        
        char fullpath[MAX_PATH];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", frames_dir, entry->d_name);
        
        struct stat st;
        if (stat(fullpath, &st) == 0 && S_ISREG(st.st_mode)) {
            filenames[frame_count] = strdup(entry->d_name);
            frame_count++;
            if (frame_count >= MAX_FRAMES) break;
        }
    }
    closedir(dir);

    if (frame_count == 0) {
        fprintf(stderr, "Error: No frame files found in %s\n", frames_dir);
        return 1;
    }

    // Sort filenames naturally (frame1, frame2, ..., frame10)
    qsort(filenames, frame_count, sizeof(char *), natural_sort_cmp);

    // Pre-load frames into RAM for smooth playback
    printf("Loading %d frames into memory...\n", frame_count);
    char **frame_buffers = malloc(frame_count * sizeof(char *));
    if (!frame_buffers) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    for (int i = 0; i < frame_count; i++) {
        char path[MAX_PATH];
        snprintf(path, sizeof(path), "%s/%s", frames_dir, filenames[i]);
        
        FILE *f = fopen(path, "rb");
        if (!f) {
            frame_buffers[i] = strdup("");
            continue;
        }
        
        fseek(f, 0, SEEK_END);
        long sz = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *buf = malloc(sz + 1);
        if (buf) {
            fread(buf, 1, sz, f);
            buf[sz] = '\0';
            frame_buffers[i] = buf;
        } else {
            frame_buffers[i] = strdup("");
        }
        fclose(f);
        free(filenames[i]);
    }

    // Hide cursor & Clear screen
    printf("\033[?25l\033c");
    fflush(stdout);

    // Play animation
    for (int i = 0; i < frame_count; i++) {
        // Move cursor to top-left (0,0) and print frame
        printf("\033[H%s", frame_buffers[i]);
        fflush(stdout);

        usleep(FRAME_DELAY_US);
    }

    // Show cursor again
    printf("\033[?25h\n");
    fflush(stdout);

    // Cleanup
    for (int i = 0; i < frame_count; i++) {
        free(frame_buffers[i]);
    }
    free(frame_buffers);

    return 0;
}

