# Bad Apple!! Terminal ASCII Player

A lightweight, high-performance C program that plays the iconic **Bad Apple!!** ASCII animation directly in your terminal. 

To prevent stuttering and I/O bottlenecks, the program pre-loads all text frames into RAM before playback. It also uses ANSI escape sequences for zero-flicker rendering, ensuring a perfectly smooth 30 FPS experience.

## Features
- **RAM Caching**: Pre-loads up to 10,000 frames into memory for seamless playback.
- **Flicker-Free Rendering**: Uses ANSI cursor manipulation (`\033[H`) to overwrite frames instead of constantly clearing the screen.
- **Natural Sorting Algorithm**: Correctly reads files in numerical order (e.g., `frame2.txt` comes before `frame10.txt`).
- **Optional Audio**: Prompts to play the synchronized background track using `mpv`.

## Project Structure
Make sure your repository and local files are structured like this before running:

```text
.
├── frames-ascii/    # Folder containing your text frames (e.g., frame1.txt, frame2.txt)
├── bad_apple.mp3    # The background audio track
├── main.c           # Main C source code
└── README.md
```

## Requirements
- **C Compiler**: `gcc` or `clang`
- **Audio Player (Optional)**: `mpv` installed and added to your system path if you want background music.

## Build and Run

You don't need any build systems like `make`. You can compile it directly from your terminal using standard GCC.

### Linux / macOS / Termux
1. Compile the code (the `-O2` flag optimizes the performance):
   ```bash
   gcc main.c -O2 -o bad_apple
   ```
2. Run the executable:
   ```bash
   ./bad_apple
   ```

### Windows (MinGW / MSYS2)
1. Compile the code:
   ```cmd
   gcc main.c -O2 -o bad_apple.exe
   ```
2. Run the executable:
   ```cmd
   bad_apple.exe
   ```

## Controls
- The program will ask if you want to play audio: `(y/n)`.
- Press `Ctrl + C` at any time to stop the animation.
