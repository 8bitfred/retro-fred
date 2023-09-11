#include "Object.hpp"

Sprite::BoxParams const &Object::getBoxParams() const
{
    static BoxParams box_params[] = {
        {       9,      9, { -1,  -1, 18, 18}, {{1, 1, 15, 14}} },
        {   42+10,      9, { -1,  -1, 18, 18}, {{3, 0, 11, 16}} },
        { 2*42+10,      9, { -1,  -1, 18, 18}, {{0, 0, 13, 16}} },
        { 3*42+10,      9, { -1,  -1, 18, 18}, {{0, 0, 15, 15}} },
        { 4*42+10,      9, { -1,  -1, 18, 18}, {{5, 0,  7, 16}} },
        { 5*42+10,      9, { -1,  -1, 18, 18}, {{2, 2, 10, 14}} },
        { 6*42+10,      9, { -1,  -1, 18, 18}, {{0, 1, 16, 15}} },
        { 7*42+10,     10, { -1,  -1, 18, 18}, {{2, 0, 11, 16}} },
    };
    static_assert(std::size(box_params) == static_cast<int>(Type::COUNT));
    return box_params[static_cast<int>(type)];
}

Sprite::RenderParams Object::getRenderParams() const
{
    return {TextureID::OBJECT, false, {}};
}
