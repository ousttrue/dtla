#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>
#include <fpalg.h>
#include <DirectXMath.h>

TEST_CASE("dot", "[dot]")
{
    REQUIRE(fpalg::Dot(std::array<float, 3>{1, 2, 3}, std::array<float, 3>{1, 2, 3}) == 14);
    REQUIRE(fpalg::Dot(DirectX::XMFLOAT3{1, 2, 3}, DirectX::XMFLOAT3{1, 2, 3}) == 14);
}
