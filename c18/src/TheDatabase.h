#ifndef ROBOFTHEDATABASE_H_
#define ROBOFTHEDATABASE_H_
#include <string>

namespace GraphicsDatabase { class Model; }

using GraphicsDatabase::Model;

class TheDatabase
{
public:
    static void create();
    static void destroy();
    static TheDatabase instance();
    static bool did_create();

private:
    TheDatabase();

public:
    ~TheDatabase();
    void create(const std::string& model_id, const std::string& batch_id) const;
    Model* find(const std::string& id) const;
};

#endif
