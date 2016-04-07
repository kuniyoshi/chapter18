#ifndef ROBOFTHEDATABASE_H_
#define ROBOFTHEDATABASE_H_
#include <string>

namespace GraphicsDatabase { class Model; }
namespace GraphicsDatabase { class Tree; }

using GraphicsDatabase::Model;
using GraphicsDatabase::Tree;

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
    void create(const std::string& id, const std::string& tree_id) const;
    void create_model(  const std::string& model_id,
                        const std::string& batch_id) const;
    Model* find_model(const std::string& id) const;
    Tree* find(const std::string& id) const;
};

#endif
