#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <falg.h>
#include <DirectXMath.h>

TEST_CASE("T", "[template]")
{
    REQUIRE(falg::EachMul(std::array<float, 2>{1, 2}, std::array<float, 2>{1, 2}) == std::array<float, 2>{1, 4});
    REQUIRE(falg::Dot(std::array<float, 3>{1, 2, 3}, std::array<float, 3>{1, 2, 3}) == 14);
    REQUIRE(falg::Dot(DirectX::XMFLOAT3{1, 2, 3}, DirectX::XMFLOAT3{1, 2, 3}) == 14);
    REQUIRE(falg::Dot(std::array<float, 4>{1, 2, 3, 4}, std::array<float, 4>{1, 2, 3, 4}) == 30);
    REQUIRE(falg::Length(std::array<float, 3>{0, 0, 2}) == 2);
    REQUIRE(falg::Length(std::array<float, 4>{0, 3, 4, 0}) == 5);
}

TEST_CASE("f16", "[decomposition]")
{
    {
        auto t = falg::TranslationMatrix(1, 2, 3);
        auto trs = falg::Decompose(t);
        REQUIRE(trs.translation == std::array<float, 3>{1, 2, 3});
        REQUIRE(trs.rotation == std::array<float, 4>{0, 0, 0, 1});
        REQUIRE(falg::Nearly(trs.rotation, std::array<float, 4>{0, 0, 0, 1}));
    }

    {
        auto r = falg::QuaternionMatrix(falg::QuaternionAxisAngle({1, 0, 0}, 1));
        auto t = falg::TranslationMatrix(1, 2, 3);
        auto trs = falg::Decompose(r * t);
        REQUIRE(trs.translation == std::array<float, 3>{1, 2, 3});
        REQUIRE(trs.rotation == falg::QuaternionAxisAngle({1, 0, 0}, 1));
    }

    {
        auto r = falg::QuaternionMatrix(falg::QuaternionAxisAngle({1, 0, 0}, 1));
        auto t = falg::TranslationMatrix(1, 2, 3);
        auto s = falg::ScaleMatrix(1, 2, 3);
        auto trs = falg::Decompose(s * r * t);
        REQUIRE(trs.translation == std::array<float, 3>{1, 2, 3});
        REQUIRE(trs.rotation == falg::QuaternionAxisAngle({1, 0, 0}, 1));
        REQUIRE(trs.scale == std::array<float, 3>{1, 2, 3});
    }
}

TEST_CASE("Transform", "[order]")
{
    auto a = falg::Transform{{1, 0, 0}, falg::QuaternionAxisAngle({1, 0, 0}, 90.0f * falg::TO_RADIANS)};
    auto aa = a.ApplyPosition({1, 2, 3});
    REQUIRE(falg::Nearly(aa, std::array<float, 3>{2, -3, 2}));

    auto b = falg::Transform{{1, 0, 0}, falg::QuaternionAxisAngle({0, 1, 0}, 90.0f * falg::TO_RADIANS)};
    auto c = (a * b).ApplyPosition({1, 0, 0});
    REQUIRE(falg::Nearly(c, std::array<float, 3>{1, 0, -2}));
}
