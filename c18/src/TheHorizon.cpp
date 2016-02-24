#include "TheHorizon.h"
#include "GameLib/Framework.h"
#include "GraphicsDatabase/Matrix44.h"
#include "GraphicsDatabase/Vector3.h"
#include "Cuboid.h"
#include "View.h"

using GraphicsDatabase::Matrix44;
using GraphicsDatabase::Vector3;

namespace
{

const double TextureHeightInTheView = 2.0;
const double TextureWidthInTheView  = TextureHeightInTheView;
const int MaxDepth          = -10;
const unsigned GrandColor   = 0xffeaeaea;
const unsigned SkyColor     = 0xff101010;

} // namespace -

namespace
{

class Impl
{
private:
    GameLib::Texture* texture_;
    double height_;
    Vector3 uv_vertexes_[4];
    Vector3 vertexes_[4];

public:
    Impl();
    ~Impl();
    void draw(const View& view);
};

Impl::Impl()
:   texture_(0), height_(0.0),
    uv_vertexes_(), vertexes_()
{
    GameLib::Framework f = GameLib::Framework::instance();
    f.createTexture(&texture_, "data/image/stage.tga");
}

Impl::~Impl()
{
    GameLib::Framework f = GameLib::Framework::instance();
    f.destroyTexture(&texture_);
}

namespace
{

// z
// | vertex0 vertex1
// | vertex2 vertex3
// |-------x-------->
// |
// v

void set_4vertexex( Vector3* vertexes,
                    double z_depth,
                    double x_move,
                    double height)
{
    vertexes[0].x = (z_depth + x_move) * TextureWidthInTheView;
    vertexes[0].y = -height;
    vertexes[0].z = z_depth * TextureHeightInTheView;
    vertexes[0].w = 1.0;

    vertexes[1].x = vertexes[0].x + TextureWidthInTheView;
    vertexes[1].y = -height;
    vertexes[1].z = vertexes[0].z;
    vertexes[1].w = 1.0;

    vertexes[2].x = vertexes[0].x;
    vertexes[2].y = -height;
    vertexes[2].z = vertexes[0].z + TextureHeightInTheView;
    vertexes[2].w = 1.0;

    vertexes[3].x = vertexes[1].x;
    vertexes[3].y = -height;
    vertexes[3].z = vertexes[2].z;
    vertexes[3].w = 1.0;
}

void set_uv_vertexes(Vector3* vertexes)
{
    vertexes[0].x = 0.0;
    vertexes[0].y = 0.0;
    vertexes[1].x = 1.0;
    vertexes[1].y = 0.0;
    vertexes[2].x = 0.0;
    vertexes[2].y = 1.0;
    vertexes[3].x = 1.0;
    vertexes[3].y = 1.0;
}

unsigned get_color(int depth)
{
    const int fixed_max_depth = MaxDepth - 1;
    const int alpha      = (GrandColor & 0xff000000) >> 24;
    const int from_red   = (GrandColor & 0x00ff0000) >> 16;
    const int from_green = (GrandColor & 0x0000ff00) >> 8;
    const int from_blue  = (GrandColor & 0x000000ff) >> 0;
    const int to_red     = (SkyColor & 0x00ff0000) >> 16;
    const int to_green   = (SkyColor & 0x0000ff00) >> 8;
    const int to_blue    = (SkyColor & 0x000000ff) >> 0;
    unsigned color = alpha << 24;
    color = color + (0x00ff0000 & ((from_red + (to_red - from_red) * depth / fixed_max_depth) << 16));
    color = color + (0x0000ff00 & ((from_green + (to_green - from_green) * depth / fixed_max_depth) << 8));
    color = color + (0x000000ff & ((from_blue + (to_blue - from_blue) * depth / fixed_max_depth) << 0));
    return color;
}

void set_4vertexex_to_the_horizon(  Vector3* vertexes,
                                    double height,
                                    double far_clip)
{
    vertexes[0].x = -far_clip;
    vertexes[0].y = -height;
    vertexes[0].z = -far_clip;
    vertexes[0].w = 1.0;

    vertexes[1].x = +far_clip;
    vertexes[1].y = -height;
    vertexes[1].z = -far_clip;
    vertexes[1].w = 1.0;

    vertexes[2].x = -far_clip;
    vertexes[2].y = -height;
    vertexes[2].z = height * MaxDepth;
    vertexes[2].w = 1.0;

    vertexes[3].x = +far_clip;
    vertexes[3].y = -height;
    vertexes[3].z = height * MaxDepth;
    vertexes[3].w = 1.0;
}

void set_uv_vertexes_to_the_horizon(Vector3* vertexes)
{
    for (int i = 0; i < 4 * 2; ++i)
    {
        vertexes[i].x = 0.0;
        vertexes[i].y = 0.0;
    }
}

} // namespace -

void Impl::draw(const View& view)
{
    GameLib::Framework f = GameLib::Framework::instance();
    f.setTexture(texture_);

    Matrix44 perspective(view.get_perspective_matrix());

    set_uv_vertexes(uv_vertexes_);

    for (int z = -static_cast< int >(view.near_clip()); z >= MaxDepth; --z)
    {
        unsigned near_color = get_color(z + 1);
        unsigned far_color  = get_color(z);

        for (int x = 0; x < -z * 2; ++x)
        {
            set_4vertexex(vertexes_, z, x, height_);

// if (x == 3 && z == -2)
// {
//             view.draw_debug_string(&vertexes_[0 * 4].x, 1);
//             view.draw_debug_string(&vertexes_[1 * 4].x, 2);
//             view.draw_debug_string(&vertexes_[2 * 4].x, 3);
// }

            perspective.multiply(&vertexes_[0]);
            perspective.multiply(&vertexes_[1]);
            perspective.multiply(&vertexes_[2]);
            perspective.multiply(&vertexes_[3]);

            f.drawTriangle3DH(  &vertexes_[0].x,
                                &vertexes_[1].x,
                                &vertexes_[2].x,
                                &uv_vertexes_[0].x,
                                &uv_vertexes_[1].x,
                                &uv_vertexes_[2].x,
                                far_color,
                                far_color,
                                near_color);
            f.drawTriangle3DH(  &vertexes_[3].x,
                                &vertexes_[1].x,
                                &vertexes_[2].x,
                                &uv_vertexes_[3].x,
                                &uv_vertexes_[1].x,
                                &uv_vertexes_[2].x,
                                near_color,
                                far_color,
                                near_color);
        }
    }

    set_4vertexex_to_the_horizon(vertexes_, height_, view.far_clip());
    perspective.multiply(&vertexes_[0]);
    perspective.multiply(&vertexes_[1]);
    perspective.multiply(&vertexes_[2]);
    perspective.multiply(&vertexes_[3]);
    set_uv_vertexes_to_the_horizon(uv_vertexes_);
    unsigned near_color = get_color(MaxDepth + 1);
    f.drawTriangle3DH(  &vertexes_[0].x,
                        &vertexes_[1].x,
                        &vertexes_[2].x,
                        &uv_vertexes_[0].x,
                        &uv_vertexes_[1].x,
                        &uv_vertexes_[2].x,
                        SkyColor,
                        SkyColor,
                        near_color);
    f.drawTriangle3DH(  &vertexes_[3].x,
                        &vertexes_[1].x,
                        &vertexes_[2].x,
                        &uv_vertexes_[3].x,
                        &uv_vertexes_[1].x,
                        &uv_vertexes_[2].x,
                        near_color,
                        SkyColor,
                        near_color);
}

Impl* g_impl = 0;

} // namespace -

void TheHorizon::create()
{
    ASSERT(!g_impl);
    g_impl = new Impl();
}

void TheHorizon::destroy()
{
    ASSERT(g_impl);
    SAFE_DELETE(g_impl);
}

TheHorizon TheHorizon::instance()
{
    return TheHorizon();
}

bool TheHorizon::did_create() { return !!g_impl; }

TheHorizon::TheHorizon() {}

TheHorizon::~TheHorizon() {}

Cuboid TheHorizon::cuboid() const
{
    return Cuboid(  Vector3(0.0, -100.0, 0.0),
                    Vector3(100.0, 100.0, 100.0));
}

void TheHorizon::draw(const View& view) { g_impl->draw(view); }
