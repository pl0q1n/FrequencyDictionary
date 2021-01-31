How to build:

- `mkdir Release`
- `cd Release`
- `cmake ..` *(you can add -DFAST:BOOL=true to this command to build with additional 3rdParty libraries (abseil and pqsort)*
- `cmake --build . --config Release`