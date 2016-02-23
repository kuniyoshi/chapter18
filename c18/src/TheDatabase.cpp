#include "TheDatabase.h"
#include <cassert>
#include "GraphicsDatabase/Database.h"
#include "GraphicsDatabase/Model.h"

using GraphicsDatabase::Database;
using GraphicsDatabase::Model;

namespace
{

class Impl
{
private:
    Database* db_;

public:
    Impl();
    ~Impl();
    void create(const std::string& model_id, const std::string& batch_id);
    Model* find(const std::string& id);
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

void Impl::create(const std::string& model_id, const std::string& batch_id)
{
    db_->create(model_id, batch_id);
}

Model* Impl::find(const std::string& id) { return db_->find(id); }

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

void TheDatabase::create(   const std::string& model_id,
                            const std::string& batch_id) const
{
    g_impl->create(model_id, batch_id);
}

Model* TheDatabase::find(const std::string& id) const
{
    return g_impl->find(id);
}
