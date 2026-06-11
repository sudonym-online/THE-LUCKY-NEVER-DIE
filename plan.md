# Objects system — implementation plan

## Phase 1: data types

- [x] 1.1. Create `src/objects.h` with `#ifndef` guard + includes
- [x] 1.2. Define `Attribute` struct with `char key[32]` and `char value[64]`
- [x] 1.3. Define `ObjectTemplate` struct with `char name[32]`, `Attribute attrs[MAX_ATTRS]`, `int attrCount`
- [x] 1.4. Define `ObjectInstance` struct with `int id`, `Attribute attrs[MAX_ATTRS]`, `int attrCount`
- [x] 1.5. Define constants: `MAX_ATTRS 32`, `MAX_TEMPLATES 32`, `MAX_INSTANCES 128`
- [x] 1.6. Declare `namespace Objects` with all function prototypes

## Phase 2: `_meta` parser

- [ ] 2.1. Create `src/objects.cpp` with `#include "objects.h"` and standard headers
- [ ] 2.2. Write helper `TrimWhitespace(char *str)` — strips leading/trailing spaces
- [ ] 2.3. Write `ParseMetaFile(const char *path, Attribute *out, int *outCount)`:
    - [ ] 2.3.1. Open file via `fopen`
    - [ ] 2.3.2. Read line by line with `fgets`
    - [ ] 2.3.3. Skip blank lines
    - [ ] 2.3.4. Skip lines starting with `#`
    - [ ] 2.3.5. Split on first `:` — left side = key, right side = value
    - [ ] 2.3.6. Trim both, store in `out[keyCount++]`
    - [ ] 2.3.7. Stop if `outCount` reaches `MAX_ATTRS`
    - [ ] 2.3.8. Close file, return
- [ ] 2.4. Handle edge cases: missing file (return silently), malformed line (skip it)

## Phase 3: directory scanning

- [ ] 3.1. Write `ScanObjectsDir()` — opens `objects/` with `<dirent.h>`
- [ ] 3.2. For each entry that is a directory (skip `.` and `..`):
    - [ ] 3.2.1. Build path: `objects/<dirname>/<dirname>_meta.txt`
    - [ ] 3.2.2. Call `ParseMetaFile` on that path
    - [ ] 3.2.3. Store result as `ObjectTemplate` with name = dirname
    - [ ] 3.2.4. Increment `templateCount`, stop if `MAX_TEMPLATES` reached
- [ ] 3.3. Store global arrays: `ObjectTemplate templates[MAX_TEMPLATES]`, `int templateCount`
- [ ] 3.4. Store instance array: `ObjectInstance instances[MAX_INSTANCES]`, `int instanceCount`, `int nextId`

## Phase 4: `Objects::InitCache()`

- [ ] 4.1. Call `ScanObjectsDir()`
- [ ] 4.2. Print summary to stdout (optional debug)

## Phase 5: `Objects::InitObject(name)`

- [ ] 5.1. Linear scan `templates[]` for matching `name`
- [ ] 5.2. Return `-1` if not found
- [ ] 5.3. Fill next `instances[instanceCount]`:
    - [ ] 5.3.1. `id = nextId++`
    - [ ] 5.3.2. `attrCount = template->attrCount`
    - [ ] 5.3.3. `memcpy` attrs from template
- [ ] 5.4. Increment `instanceCount`
- [ ] 5.5. Return `id`

## Phase 6: `Objects::Get(key, value)`

- [ ] 6.1. Accept `const char *key`, `const char *value`, `int *outCount`
- [ ] 6.2. Loop through all instances
- [ ] 6.3. For each instance, loop through its attrs
- [ ] 6.4. If `strcmp(attr.key, key) == 0 && strcmp(attr.value, value) == 0`, collect instance `id`
- [ ] 6.5. Write count to `*outCount`, return pointer to static result array
- [ ] 6.6. Return `nullptr` if no matches

## Phase 7: `Objects::HasType(id, type)`

- [ ] 7.1. Find instance by linear id scan
- [ ] 7.2. Check attrs for `key == "type"` and `value == type`
- [ ] 7.3. Return `true`/`false`

## Phase 8: `Objects::SetAttr(id, key, value)`

- [ ] 8.1. Find instance by `id`
- [ ] 8.2. Scan existing attrs — if `key` found, overwrite `value`
- [ ] 8.3. If not found and `attrCount < MAX_ATTRS`, add new attr

## Phase 9: cleanup

- [ ] 9.1. `Objects::UnloadAll()` — reset all globals (no models to unload since we don't own them)
- [ ] 9.2. No memory to free — all storage is fixed-size arrays

## Phase 10: integration into `main.cpp`

- [ ] 10.1. Add `#include "src/objects.h"`
- [ ] 10.2. Call `Objects::InitCache()` in `init()`
- [ ] 10.3. Add example spawn (commented out or conditional)
- [ ] 10.4. Call `Objects::UnloadAll()` before `CloseWindow()`
- [ ] 10.5. Verify build compiles (`make`)

## Phase 11: test with an actual `_meta` file

- [ ] 11.1. Write `objects/Deagle/deagle_meta.txt` with some test keys
- [ ] 11.2. Run the game, verify `InitCache` finds it
- [ ] 11.3. Call `InitObject("Deagle")`, print returned id
- [ ] 11.4. Call `Get` with known key/value, verify it returns the id
- [ ] 11.5. Call `SetAttr` to change runtime data, verify it stuck

## Phase 12: update `AGENTS.md`

- [ ] 12.1. Document the `objects/` folder convention
- [ ] 12.2. Document the `_meta.txt` format
- [ ] 12.3. Document the public API functions
- [ ] 12.4. Document how to add new objects
