# Objects system — implementation plan

## Phase 1: data types

- [x] 1.1. Create `src/objects.h` with `#ifndef` guard + includes
- [x] 1.2. Define `Attribute` struct with `char key[32]` and `char value[64]`
- [x] 1.3. Define `ObjectTemplate` struct with `char name[32]`, `Attribute attrs[MAX_ATTRS]`, `int attrCount`
- [x] 1.4. Define `ObjectInstance` struct with `int id`, `Attribute attrs[MAX_ATTRS]`, `int attrCount`
- [x] 1.5. Define constants: `MAX_ATTRS 32`, `MAX_TEMPLATES 32`, `MAX_INSTANCES 128`
- [x] 1.6. Declare `namespace Objects` with all function prototypes

## Phase 2: JSON parser

- [x] 2.1. Create `src/objects.cpp` with `#include "objects.h"` and standard headers
- [x] 2.2. Write `ParseJsonFile(const char *path, ObjectTemplate *tpl)`:
    - [x] 2.2.1. Open file via `fopen`, read entire contents with `fread`
    - [x] 2.2.2. Walk char-by-char, ignore whitespace and `{}` braces
    - [x] 2.2.3. Extract `"key": "value"` pairs — split on `:`, strip quotes
    - [x] 2.2.4. Store each pair in `tpl->attrs[tpl->attrCount++]`
    - [x] 2.2.5. Close file, return
- [x] 2.3. Handle edge cases: missing file (return silently), malformed JSON (skip)

## Phase 3: directory scanning

- [x] 3.1. Write `ScanObjectsDir()` — opens `objects/` with `<dirent.h>`
- [x] 3.2. For each entry that is a directory (skip `.` and `..`):
    - [x] 3.2.1. Re-open directory, scan for any file ending in `.json`
    - [x] 3.2.2. Call `ParseJsonFile` on the first `.json` found
    - [x] 3.2.3. Store result as `ObjectTemplate` with name = dirname
    - [x] 3.2.4. Increment `templateCount`, stop if `MAX_TEMPLATES` reached
- [x] 3.3. Store global arrays: `ObjectTemplate templates[MAX_TEMPLATES]`, `int templateCount`
- [x] 3.4. Store instance array: `ObjectInstance instances[MAX_INSTANCES]`, `int instanceCount`, `int nextId`

## Phase 4: `Objects::InitCache()`

- [x] 4.1. Call `ScanObjectsDir()`
- [x] 4.2. Print summary to stdout (optional debug)

## Phase 5: `Objects::InitObject(name)`

- [x] 5.1. Linear scan `templates[]` for matching `name`
- [x] 5.2. Return `-1` if not found
- [x] 5.3. Fill next `instances[instanceCount]`:
    - [x] 5.3.1. `id = nextId++`
    - [x] 5.3.2. `attrCount = template->attrCount`
    - [x] 5.3.3. `memcpy` attrs from template
- [x] 5.4. Increment `instanceCount`
- [x] 5.5. Return `id`

## Phase 6: `Objects::Get(key, value)`

- [x] 6.1. Accept `const char *key`, `const char *value`, `int *outCount`
- [x] 6.2. Loop through all instances
- [x] 6.3. For each instance, loop through its attrs
- [x] 6.4. If `strcmp(attr.key, key) == 0 && strcmp(attr.value, value) == 0`, collect instance `id`
- [x] 6.5. Write count to `*outCount`, return pointer to static result array
- [x] 6.6. Return `nullptr` if no matches

## Phase 7: `Objects::HasType(id, type)`

- [x] 7.1. Find instance by linear id scan
- [x] 7.2. Check attrs for `key == "type"` and `value == type`
- [x] 7.3. Return `true`/`false`

## Phase 8: `Objects::SetAttr(id, key, value)`

- [x] 8.1. Find instance by `id`
- [x] 8.2. Scan existing attrs — if `key` found, overwrite `value`
- [x] 8.3. If not found and `attrCount < MAX_ATTRS`, add new attr

## Phase 9: cleanup

- [x] 9.1. `Objects::UnloadAll()` — reset all globals (no models to unload since we don't own them)
- [x] 9.2. No memory to free — all storage is fixed-size arrays

## Phase 10: integration into `main.cpp`

- [x] 10.1. Add `#include "src/objects.h"`
- [x] 10.2. Call `Objects::InitCache()` in `init()`
- [x] 10.3. Add example spawn (commented out or conditional)
- [x] 10.4. Call `Objects::UnloadAll()` before `CloseWindow()`
- [x] 10.5. Verify build compiles (`make`)

## Phase 11: test with an actual `.json` file

- [x] 11.1. Write `objects/Deagle/deagle.json` with some test key-value pairs
- [ ] 11.2. Run the game, verify `InitCache` finds it (blocked by macOS linker)
- [ ] 11.3. Call `InitObject("Deagle")`, print returned id (blocked by macOS linker)
- [ ] 11.4. Call `Get` with known key/value, verify it returns the id (blocked by macOS linker)
- [ ] 11.5. Call `SetAttr` to change runtime data, verify it stuck (blocked by macOS linker)

## Phase 12: update `AGENTS.md`

- [x] 12.1. Document the `objects/` folder convention
- [x] 12.2. Document the `.json` file format
- [x] 12.3. Document the public API functions
- [x] 12.4. Document how to add new objects
