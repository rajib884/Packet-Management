## Packet Counter

This C program reads a Wireshark packet capture txt file and counts the number of valid IPv4 UDP packets for particular source and destination IP address. It uses linked lists and hash tables for efficient packet counting.

### Usage

1. **Compile the Code:** Use option `-DUSE_UNICODE` if your terminal supports printing unicode characters.

   ```bash
   gcc main.c src/*.c -Iinclude -o main -DUSE_UNICODE
   ```

2. **Run the Program:**
   Provide the path to the Wireshark capture file as a command-line argument.

   ```bash
   ./main <file_path>
   ```

   Replace `<file_path>` with the actual path to your Wireshark capture file.

### Example

```bash
./main data/udp.txt
```
