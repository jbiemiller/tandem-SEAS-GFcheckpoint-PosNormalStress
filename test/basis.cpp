#include "basis/Functions.h"
#include "basis/Quadrature.h"

#include "doctest.h"
#include <array>
#include <cstddef>
#include <functional>
#include <vector>

using tndm::GaussJacobi;
using tndm::gradTetraDubinerP;
using tndm::gradTriDubinerP;
using tndm::TetraDubinerP;
using tndm::TetrahedronQuadrature;
using tndm::TriangleQuadrature;
using tndm::TriDubinerP;

using TriBasisFunction = std::function<double(std::array<double, 2> const&)>;
using GradTriBasisFunction = std::function<std::array<double, 2>(std::array<double, 2> const&)>;
using TetraBasisFunction = std::function<double(std::array<double, 3> const&)>;
using GradTetraBasisFunction = std::function<std::array<double, 3>(std::array<double, 3> const&)>;

TEST_CASE("Basis") {
    std::vector<std::array<double, 2>> testPoints2{{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {0.25, 0.25},
                                                   {0.1, 0.1}, {0.1, 0.2}, {0.2, 0.1}};

    std::vector<std::pair<std::array<unsigned, 2>, TriBasisFunction>> triBFs{
        {{0, 0}, [](std::array<double, 2> const& p) { return 1.0; }},
        {{1, 0}, [](std::array<double, 2> const& p) { return -1.0 + 2.0 * p[0] + p[1]; }},
        {{0, 1}, [](std::array<double, 2> const& p) { return -1.0 + 3.0 * p[1]; }},
        {{2, 0},
         [](std::array<double, 2> const& p) {
             return 1.0 - 6.0 * p[0] + 6.0 * p[0] * p[0] - 2.0 * p[1] + 6.0 * p[0] * p[1] +
                    p[1] * p[1];
         }},
        {{1, 1},
         [](std::array<double, 2> const& p) {
             return 1.0 - 2.0 * p[0] - 6.0 * p[1] + 10.0 * p[0] * p[1] + 5.0 * p[1] * p[1];
         }},
        {{0, 2},
         [](std::array<double, 2> const& p) { return 1.0 - 8.0 * p[1] + 10.0 * p[1] * p[1]; }}};

    SUBCASE("Dubiner polynomials on triangle") {
        for (auto& t : triBFs) {
            for (auto& p : testPoints2) {
                CHECK(TriDubinerP(t.first, p) == doctest::Approx(t.second(p)));
            }
        }
    }

    std::vector<std::pair<std::array<unsigned, 2>, GradTriBasisFunction>> gradTriBFs{
        {{0, 0},
         [](std::array<double, 2> const& p) -> std::array<double, 2> {
             return {0.0, 0.0};
         }},
        {{1, 0},
         [](std::array<double, 2> const& p) -> std::array<double, 2> {
             return {2.0, 1.0};
         }},
        {{0, 1},
         [](std::array<double, 2> const& p) -> std::array<double, 2> {
             return {0.0, 3.0};
         }},
        {{2, 0},
         [](std::array<double, 2> const& p) -> std::array<double, 2> {
             return {-6.0 + 12.0 * p[0] + 6.0 * p[1], -2.0 + 6.0 * p[0] + 2.0 * p[1]};
         }},
        {{1, 1},
         [](std::array<double, 2> const& p) -> std::array<double, 2> {
             return {-2.0 + 10.0 * p[1], -6.0 + 10.0 * p[0] + 10.0 * p[1]};
         }},
        {{0, 2}, [](std::array<double, 2> const& p) -> std::array<double, 2> {
             return {0.0, -8.0 + 20.0 * p[1]};
         }}};

    SUBCASE("Dubiner polynomials gradients on triangle") {
        for (auto& t : gradTriBFs) {
            for (auto& p : testPoints2) {
                auto grad = gradTriDubinerP(t.first, p);
                auto refGrad = t.second(p);
                for (std::size_t d = 0; d < grad.size(); ++d) {
                    CHECK(grad[d] == doctest::Approx(refGrad[d]));
                }
            }
        }
    }

    std::vector<std::array<double, 3>> testPoints3{
        {0.0, 0.0, 0.0},    {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0},
        {0.25, 0.25, 0.25}, {0.1, 0.1, 0.1}, {0.1, 0.2, 0.3}, {0.2, 0.1, 0.3}};

    std::vector<std::pair<std::array<unsigned, 3>, TetraBasisFunction>> tetraBFs{
        {{0, 0, 0}, [](std::array<double, 3> const& p) { return 1.0; }},
        {{1, 0, 0}, [](std::array<double, 3> const& p) { return -1.0 + 2.0 * p[0] + p[1] + p[2]; }},
        {{0, 1, 0}, [](std::array<double, 3> const& p) { return -1.0 + 3.0 * p[1] + p[2]; }},
        {{0, 0, 1}, [](std::array<double, 3> const& p) { return -1.0 + 4.0 * p[2]; }},
        {{2, 0, 0},
         [](std::array<double, 3> const& p) {
             return 1.0 - 6.0 * p[0] + 6.0 * p[0] * p[0] - 2.0 * p[1] + 6.0 * p[0] * p[1] +
                    p[1] * p[1] - 2.0 * p[2] + 6.0 * p[0] * p[2] + 2.0 * p[1] * p[2] + p[2] * p[2];
         }},
        {{1, 1, 0},
         [](std::array<double, 3> const& p) {
             return 1.0 - 2.0 * p[0] - 6.0 * p[1] + 10.0 * p[0] * p[1] + 5.0 * p[1] * p[1] -
                    2.0 * p[2] + 2.0 * p[0] * p[2] + 6.0 * p[1] * p[2] + p[2] * p[2];
         }},
        {{0, 2, 0},
         [](std::array<double, 3> const& p) {
             return 1.0 - 8.0 * p[1] + 10.0 * p[1] * p[1] - 2.0 * p[2] + 8.0 * p[1] * p[2] +
                    p[2] * p[2];
         }},
        {{1, 0, 1},
         [](std::array<double, 3> const& p) {
             return 1.0 - 2.0 * p[0] - p[1] - 7.0 * p[2] + 12.0 * p[0] * p[2] + 6.0 * p[1] * p[2] +
                    6.0 * p[2] * p[2];
         }},
        {{0, 1, 1},
         [](std::array<double, 3> const& p) {
             return 1.0 - 3.0 * p[1] - 7.0 * p[2] + 18.0 * p[1] * p[2] + 6.0 * p[2] * p[2];
         }},
        {{0, 0, 2},
         [](std::array<double, 3> const& p) { return 1.0 - 10.0 * p[2] + 15.0 * p[2] * p[2]; }}};

    SUBCASE("Dubiner polynomials on tetrahedron") {
        for (auto& t : tetraBFs) {
            for (auto& p : testPoints3) {
                CHECK(TetraDubinerP(t.first, p) == doctest::Approx(t.second(p)));
            }
        }
    }

    std::vector<std::pair<std::array<unsigned, 3>, GradTetraBasisFunction>> gradTetraBFs{
        {{0, 0, 0},
         [](std::array<double, 3> const& p) -> std::array<double, 3> {
             return {0.0, 0.0, 0.0};
         }},
        {{1, 0, 0},
         [](std::array<double, 3> const& p) -> std::array<double, 3> {
             return {2.0, 1.0, 1.0};
         }},
        {{0, 1, 0},
         [](std::array<double, 3> const& p) -> std::array<double, 3> {
             return {0.0, 3.0, 1.0};
         }},
        {{0, 0, 1},
         [](std::array<double, 3> const& p) -> std::array<double, 3> {
             return {0.0, 0.0, 4.0};
         }},
        {{2, 0, 0},
         [](std::array<double, 3> const& p) -> std::array<double, 3> {
             return {-6.0 + 12.0 * p[0] + 6.0 * p[1] + 6.0 * p[2],
                     -2.0 + 6.0 * p[0] + 2.0 * p[1] + 2.0 * p[2],
                     -2.0 + 6.0 * p[0] + 2.0 * p[1] + 2.0 * p[2]};
         }},
        {{1, 1, 0},
         [](std::array<double, 3> const& p) -> std::array<double, 3> {
             return {-2.0 + 10.0 * p[1] + 2.0 * p[2], -6.0 + 10.0 * p[0] + 10.0 * p[1] + 6.0 * p[2],
                     -2.0 + 2.0 * p[0] + 6.0 * p[1] + 2.0 * p[2]};
         }},
        {{0, 2, 0},
         [](std::array<double, 3> const& p) -> std::array<double, 3> {
             return {0.0, -8.0 + 20.0 * p[1] + 8.0 * p[2], -2.0 + 8.0 * p[1] + 2.0 * p[2]};
         }},
        {{1, 0, 1},
         [](std::array<double, 3> const& p) -> std::array<double, 3> {
             return {-2.0 + 12.0 * p[2], -1.0 + 6.0 * p[2],
                     -7.0 + 12.0 * p[0] + 6.0 * p[1] + 12.0 * p[2]};
         }},
        {{0, 1, 1},
         [](std::array<double, 3> const& p) -> std::array<double, 3> {
             return {0.0, -3.0 + 18.0 * p[2], -7.0 + 18.0 * p[1] + 12.0 * p[2]};
         }},
        {{0, 0, 2}, [](std::array<double, 3> const& p) -> std::array<double, 3> {
             return {0.0, 0.0, -10.0 + 30.0 * p[2]};
         }}};

    SUBCASE("Dubiner polynomials gradients on tetrahedron") {
        for (auto& t : gradTetraBFs) {
            for (auto& p : testPoints3) {
                auto grad = gradTetraDubinerP(t.first, p);
                auto refGrad = t.second(p);
                for (std::size_t d = 0; d < grad.size(); ++d) {
                    CHECK(grad[d] == doctest::Approx(refGrad[d]));
                }
            }
        }
    }

    SUBCASE("Gauss Jacobi vs Maple") { // Test from SeisSol
        auto rule = GaussJacobi(5, 1, 3);
        CHECK(rule.points()[0] == doctest::Approx(0.86698568210542769702));
        CHECK(rule.points()[1] == doctest::Approx(0.57652877512667440772));
        CHECK(rule.points()[2] == doctest::Approx(0.17976783188823737401));
        CHECK(rule.points()[3] == doctest::Approx(-.25499675973326581341));
        CHECK(rule.points()[4] == doctest::Approx(-.65399981510135937963));
        CHECK(rule.weights()[0] == doctest::Approx(0.18915446768616357329));
        CHECK(rule.weights()[1] == doctest::Approx(0.58714974961811369751));
        CHECK(rule.weights()[2] == doctest::Approx(0.57657004957734461768));
        CHECK(rule.weights()[3] == doctest::Approx(0.22255926867518051648));
        CHECK(rule.weights()[4] == doctest::Approx(0.024566464443197594119));
    }

    SUBCASE("Triangle vs Maple") { // Test from SeisSol
        auto rule = TriangleQuadrature(2);
        CHECK(rule.points()[0][0] == doctest::Approx(0.64494897427831780982));
        CHECK(rule.points()[1][0] == doctest::Approx(0.64494897427831780982));
        CHECK(rule.points()[2][0] == doctest::Approx(0.15505102572168219018));
        CHECK(rule.points()[3][0] == doctest::Approx(0.15505102572168219018));
        CHECK(rule.points()[0][1] == doctest::Approx(0.28001991549907407200));
        CHECK(rule.points()[1][1] == doctest::Approx(0.075031110222608118175));
        CHECK(rule.points()[2][1] == doctest::Approx(0.66639024601470138669));
        CHECK(rule.points()[3][1] == doctest::Approx(0.17855872826361642311));
        CHECK(rule.weights()[0] == doctest::Approx(0.090979309128011415315));
        CHECK(rule.weights()[1] == doctest::Approx(0.090979309128011415315));
        CHECK(rule.weights()[2] == doctest::Approx(0.15902069087198858472));
        CHECK(rule.weights()[3] == doctest::Approx(0.15902069087198858472));
    }

    SUBCASE("Triangle") {
        auto rule = TriangleQuadrature(1);
        auto f = [](std::array<double, 2> const& xi) { return xi[0] + xi[1]; };
        double intf = 0.0;
        for (std::size_t i = 0; i < rule.points().size(); ++i) {
            intf += f(rule.points()[i]) * rule.weights()[i];
        }
        CHECK(intf == doctest::Approx(1.0 / 3.0));
    }

    SUBCASE("Tetrahedron") {
        auto rule = TetrahedronQuadrature(3);
        auto intBFPair = [&](std::size_t i, std::size_t j) {
            double intf = 0.0;
            for (std::size_t q = 0; q < rule.points().size(); ++q) {
                intf += tetraBFs[i].second(rule.points()[q]) *
                        tetraBFs[j].second(rule.points()[q]) * rule.weights()[q];
            }
            return intf;
        };
        CHECK(doctest::Approx(intBFPair(8, 9)) == 0.0);
        CHECK(doctest::Approx(intBFPair(0, 9)) == 0.0);
        CHECK(doctest::Approx(intBFPair(6, 3)) == 0.0);
        CHECK(doctest::Approx(intBFPair(0, 1)) == 0.0);
    }
}
