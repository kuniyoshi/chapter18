#include "TheDatabase.h"
#include <cassert>
#include "GraphicsDatabase/Database.h"
#include "GraphicsDatabase/Model.h"
#include "GraphicsDatabase/Tree.h"

using GraphicsDatabase::Database;
using GraphicsDatabase::Model;
using GraphicsDatabase::Tree;

namespace
{

class Impl
{
private:
    Database* db_;

public:
    Impl();
    ~Impl();
    void create(const std::string& id, const std::string& tree_id);
    void create_model(  const std::string& model_id,
                        const std::string& batch_id);
    Model* find_model(const std::string& id);
    Tree* find(const std::string& id);
};

Impl::Impl()
: db_(0)
{
    db_ = new Database("data/models.json");
}

Impl::~Impl()
{
    delete db_;
    db_ = 0;
}

void Impl::create(const std::string& id, const std::string& tree_id)
{
    db_->create_tree(id, tree_id);
}

void Impl::create_model(    const std::string& model_id,
                            const std::string& batch_id)
{
    db_->create(model_id, batch_id);
}

Tree* Impl::find(const std::string& id) { return db_->find_tree(id); }

Model* Impl::find_model(const std::string& id) { return db_->find(id); }

Impl* g_impl = 0;

} // namespace -

void TheDatabase::create()
{
    assert(!g_impl);
    g_impl = new Impl();
}

void TheDatabase::destroy()
{
    assert(!!g_impl);
    delete g_impl;
    g_impl = 0;
}

TheDatabase TheDatabase::instance() { return TheDatabase(); }

bool TheDatabase::did_create() { return !!g_impl; }

TheDatabase::TheDatabase() {}

TheDatabase::~TheDatabase() {}

void TheDatabase::create(   const std::string& id,
                            const std::string& tree_id) const
{
    g_impl->create(id, tree_id);
}

void TheDatabase::create_model( const std::string& model_id,
                                const std::string& batch_id) const
{
    g_impl->create_model(model_id, batch_id);
}

Tree* TheDatabase::find(const std::string& id) const
{
    return g_impl->find(id);
}

Model* TheDatabase::find_model(const std::string& id) const
{
    return g_impl->find_model(id);
}
