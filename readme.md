How to build:

Linux:
- `mkdir Release`
- `cd Release`
- `cmake .. -DCMAKE_BUILD_TYPE=Release` *(you can add -DFAST:BOOL=true to this command to build with additional 3rdParty libraries (abseil and pqsort)*
- `make`

Windows:
- `mkdir Release`
- `cd Release`
- `cmake ..` *(you can add -DFAST:BOOL=true to this command to build with additional 3rdParty libraries (abseil and pqsort)*
- `cmake --build . --config Release`
