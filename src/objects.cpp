#include "objects.h"
#include "raylib.h"
#include <cstdio>
#include <cstring>
#include <dirent.h>

static void ParseJsonFile(const char *path, Objects::ObjectTemplate *tpl)
{
    FILE *f = fopen(path, "r");
    if (!f) return;

    char buf[4096];
    int len = fread(buf, 1, sizeof(buf) - 1, f);
    buf[len] = '\0';
    fclose(f);

    char *p = buf;
    while (*p && *p != '{') p++;
    if (!*p) return;
    p++;

    while (*p && tpl->attrCount < Objects::MAX_ATTRS) {
        while (*p && *p != '"') p++;
        if (!*p) break;
        p++;

        char *key = p;
        while (*p && *p != '"') p++;
        if (!*p) break;
        *p = '\0';
        p++;

        while (*p && *p != ':') p++;
        if (!*p) break;
        p++;

        while (*p && *p != '"') p++;
        if (!*p) break;
        p++;

        char *val = p;
        while (*p && *p != '"') p++;
        if (!*p) break;
        *p = '\0';
        p++;

        strncpy(tpl->attrs[tpl->attrCount].key, key, sizeof(tpl->attrs[0].key) - 1);
        strncpy(tpl->attrs[tpl->attrCount].value, val, sizeof(tpl->attrs[0].value) - 1);
        tpl->attrCount++;

        while (*p && *p != ',' && *p != '}') p++;
        if (*p == ',') p++;
    }
}

namespace {

    Objects::ObjectTemplate templates[Objects::MAX_TEMPLATES];
    int templateCount = 0;

    int nextId = 1;

    inline bool isDirectory(const dirent *entry) { return entry->d_type == DT_DIR; }
    inline bool isNavDirectory(const dirent *entry) { return strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0; }
    inline bool isJsonFile(const dirent *entry) { return strcmp(entry->d_name + strlen(entry->d_name) - 5, ".json") == 0; }

    void ScanObjectsDir()
    {
        DIR *dir = opendir("objects");
        if (!dir) return;

        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr && templateCount < Objects::MAX_TEMPLATES) {
            if (!isDirectory(entry)) continue;
            if (isNavDirectory(entry)) continue; // avoids recursion

            char dirpath[256];
            snprintf(dirpath, sizeof(dirpath), "objects/%s", entry->d_name);

            DIR *sub = opendir(dirpath);
            if (!sub) continue;

            struct dirent *file;
            while ((file = readdir(sub)) != nullptr) {
                if (!isJsonFile(file)) continue;

                char filepath[512];
                snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, file->d_name);

                Objects::ObjectTemplate *tpl = &templates[templateCount];
                tpl->attrCount = 0;
                strncpy(tpl->name, entry->d_name, sizeof(tpl->name) - 1);
                ParseJsonFile(filepath, tpl);
                templateCount++;
                break;
            }
            closedir(sub);
        }
        closedir(dir);
    }

}

Objects::ObjectInstance Objects::instances[Objects::MAX_INSTANCES];
int Objects::instanceCount = 0;

StaticBody Objects::objectInstances[Objects::MAX_OBJECT_INSTANCES];
int Objects::objectInstanceCount = 0;

void Objects::InitCache()
{
    ScanObjectsDir();
    printf("[Objects] Loaded %d template(s)\n", templateCount);
}

int Objects::Create(const char *name)
{
    for (int i = 0; i < templateCount; i++) {
        if (strcmp(templates[i].name, name) == 0) {
            if (instanceCount >= MAX_INSTANCES) return -1;
            ObjectInstance *inst = &instances[instanceCount];
            inst->id = nextId++;
            inst->attrCount = templates[i].attrCount;
            memcpy(inst->attrs, templates[i].attrs, sizeof(Attribute) * inst->attrCount);
            instanceCount++;
            return inst->id;
        }
    }
    return -1;
}

const int *Objects::Get(const char *key, const char *value, int *outCount)
{
    static int results[MAX_INSTANCES];
    int count = 0;
    for (int i = 0; i < instanceCount; i++) {
        for (int j = 0; j < instances[i].attrCount; j++) {
            if (strcmp(instances[i].attrs[j].key, key) == 0 &&
                strcmp(instances[i].attrs[j].value, value) == 0) {
                results[count++] = instances[i].id;
                break;
            }
        }
    }
    *outCount = count;
    return count > 0 ? results : nullptr;
}

bool Objects::HasType(int id, const char *type)
{
    for (int i = 0; i < instanceCount; i++) {
        if (instances[i].id != id) continue;
        for (int j = 0; j < instances[i].attrCount; j++) {
            if (strcmp(instances[i].attrs[j].key, "type") == 0 &&
                strcmp(instances[i].attrs[j].value, type) == 0)
                return true;
        }
        return false;
    }
    return false;
}

void Objects::SetAttr(int id, const char *key, const char *value)
{
    for (int i = 0; i < instanceCount; i++) {
        if (instances[i].id != id) continue;
        for (int j = 0; j < instances[i].attrCount; j++) {
            if (strcmp(instances[i].attrs[j].key, key) == 0) {
                strncpy(instances[i].attrs[j].value, value, sizeof(instances[i].attrs[0].value) - 1);
                return;
            }
        }
        if (instances[i].attrCount < MAX_ATTRS) {
            strncpy(instances[i].attrs[instances[i].attrCount].key, key, sizeof(instances[i].attrs[0].key) - 1);
            strncpy(instances[i].attrs[instances[i].attrCount].value, value, sizeof(instances[i].attrs[0].value) - 1);
            instances[i].attrCount++;
        }
        return;
    }
}

int Objects::Spawn(int id, Vector3 position, Vector3 scale, float rotation)
{
    if (objectInstanceCount >= MAX_OBJECT_INSTANCES) return -1;

    ObjectInstance *inst = nullptr;
    for (int i = 0; i < instanceCount; i++) {
        if (instances[i].id == id) {
            inst = &instances[i];
            break;
        }
    }
    if (!inst) return -1;

    const char *modelPath = nullptr;
    for (int i = 0; i < inst->attrCount; i++) {
        if (strcmp(inst->attrs[i].key, "model") == 0) {
            modelPath = inst->attrs[i].value;
            break;
        }
    }
    if (!modelPath) return -1;

    StaticBody &body = objectInstances[objectInstanceCount];
    body.model = LoadModel(modelPath);
    body.position = position;
    body.scale = scale;
    body.rotation = rotation;
    body.UpdateAABB();

    return objectInstanceCount++;
}

void Objects::UnloadObjectInstances()
{
    for (int i = 0; i < objectInstanceCount; i++)
        UnloadModel(objectInstances[i].model);
    objectInstanceCount = 0;
}

void Objects::UnloadAll()
{
    UnloadObjectInstances();
    templateCount = 0;
    instanceCount = 0;
    nextId = 1;
}
