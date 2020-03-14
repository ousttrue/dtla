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
        REQUIRE(trs.position == std::array<float, 3>{1, 2, 3});
        REQUIRE(trs.rotation == std::array<float, 4>{0, 0, 0, 1});
    }

    {
        auto r = fpalg::QuaternionMatrix(fpalg::QuaternionAxisAngle({1, 0, 0}, 1));
        auto t = fpalg::TranslationMatrix(1, 2, 3);
        auto trs = fpalg::Decompose(r * t);
        REQUIRE(trs.position == std::array<float, 3>{1, 2, 3});
        REQUIRE(trs.rotation == fpalg::QuaternionAxisAngle({1, 0, 0}, 1));
    }

    {
        auto r = fpalg::QuaternionMatrix(fpalg::QuaternionAxisAngle({1, 0, 0}, 1));
        auto t = fpalg::TranslationMatrix(1, 2, 3);
        auto s = fpalg::ScaleMatrix(1, 2, 3);
        auto trs = fpalg::Decompose(s * r * t);
        REQUIRE(trs.position == std::array<float, 3>{1, 2, 3});
        REQUIRE(trs.rotation == fpalg::QuaternionAxisAngle({1, 0, 0}, 1));
        REQUIRE(trs.scale == std::array<float, 3>{1, 2, 3});
    }
}
