 #ifndef OBJECTS_H
#define OBJECTS_H

#include <cstring>
#include "game.h"

namespace Objects {

    const int MAX_ATTRS = 32;
    const int MAX_TEMPLATES = 32;
    const int MAX_INSTANCES = 128;
    const int MAX_OBJECT_INSTANCES = 128;

    struct Attribute {
        char key[32];
        char value[64];
    };

    struct ObjectTemplate {
        char name[32];
        Attribute attrs[MAX_ATTRS];
        int attrCount;
    };

    struct ObjectInstance {
        int id;
        Attribute attrs[MAX_ATTRS];
        int attrCount;
    };

    extern ObjectInstance instances[MAX_INSTANCES];
    extern int instanceCount;

    extern StaticBody objectInstances[MAX_OBJECT_INSTANCES];
    extern int objectInstanceCount;

    void InitCache();
    int  Create(const char *name);
    const int *Get(const char *key, const char *value, int *outCount);
    bool HasType(int id, const char *type);
    void SetAttr(int id, const char *key, const char *value);
    int  Spawn(int id, Vector3 position = {0, 0, 0}, Vector3 scale = {1, 1, 1}, float rotation = 0);
    void UnloadObjectInstances();
    void UnloadAll();

}

#endif
