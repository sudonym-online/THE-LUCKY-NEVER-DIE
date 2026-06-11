#ifndef OBJECTS_H
#define OBJECTS_H

#include <cstring>

namespace Objects {

    const int MAX_ATTRS = 32;
    const int MAX_TEMPLATES = 32;
    const int MAX_INSTANCES = 128;

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

    void InitCache();
    int  InitObject(const char *name);
    const int *Get(const char *key, const char *value, int *outCount);
    bool HasType(int id, const char *type);
    void SetAttr(int id, const char *key, const char *value);
    void UnloadAll();

}

#endif
