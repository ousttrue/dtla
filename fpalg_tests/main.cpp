#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <fpalg.h>
#include <DirectXMath.h>

TEST_CASE("f3", "[dot]")
{
    REQUIRE(fpalg::Dot(std::array<float, 3>{1, 2, 3}, std::array<float, 3>{1, 2, 3}) == 14);
    REQUIRE(fpalg::Dot(DirectX::XMFLOAT3{1, 2, 3}, DirectX::XMFLOAT3{1, 2, 3}) == 14);
}

TEST_CASE("f16", "[decomposition]")
{
    {
        auto t = fpalg::TranslationMatrix(1, 2, 3);
        auto trs = fpalg::Decompose(t);
        REQUIRE(trs.translation == std::array<float, 3>{1, 2, 3});
        REQUIRE(trs.rotation == std::array<float, 4>{0, 0, 0, 1});
        REQUIRE(fpalg::Nearly(trs.rotation, std::array<float, 4>{0, 0, 0, 1}));
    }

    {
        auto r = fpalg::QuaternionMatrix(fpalg::QuaternionAxisAngle({1, 0, 0}, 1));
        auto t = fpalg::TranslationMatrix(1, 2, 3);
        auto trs = fpalg::Decompose(r * t);
        REQUIRE(trs.translation == std::array<float, 3>{1, 2, 3});
        REQUIRE(trs.rotation == fpalg::QuaternionAxisAngle({1, 0, 0}, 1));
    }

    {
        auto r = fpalg::QuaternionMatrix(fpalg::QuaternionAxisAngle({1, 0, 0}, 1));
        auto t = fpalg::TranslationMatrix(1, 2, 3);
        auto s = fpalg::ScaleMatrix(1, 2, 3);
        auto trs = fpalg::Decompose(s * r * t);
        REQUIRE(trs.translation == std::array<float, 3>{1, 2, 3});
        REQUIRE(trs.rotation == fpalg::QuaternionAxisAngle({1, 0, 0}, 1));
        REQUIRE(trs.scale == std::array<float, 3>{1, 2, 3});
    }
}

TEST_CASE("Transform", "[order]")
{
    auto a = fpalg::Transform{{1, 0, 0}, fpalg::QuaternionAxisAngle({1, 0, 0}, 90.0f * fpalg::TO_RADIANS)};
    auto aa = a.ApplyPosition({1, 2, 3});
    REQUIRE(fpalg::Nearly(aa, std::array<float, 3>{2, -3, 2}));

    auto b = fpalg::Transform{{1, 0, 0}, fpalg::QuaternionAxisAngle({0, 1, 0}, 90.0f * fpalg::TO_RADIANS)};
    auto c = (a * b).ApplyPosition({1, 0, 0});
    REQUIRE(fpalg::Nearly(c, std::array<float, 3>{1, 0, -2}));
}
