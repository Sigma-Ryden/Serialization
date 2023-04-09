#include "SFTestingBase.hpp"

struct Vector
{
    Vector(float x = 0.f, float y = 0.f, float z = 0.f)
        : X(x), Y(y), Z(z) {}

    float X;
    float Y;
    float Z;
};

struct Box
{
    Box() {}
    Box(const Vector& min, const Vector& max)
        : Min(min), Max(max) {}

    Vector Min;
    Vector Max;
};

SERIALIZATION(SaveLoad, Vector)
{
    archive & self.X & self.Y & self.Z;
}

SERIALIZATION(SaveLoad, Box)
{
    archive & self.Min & self.Max;
}

TEST(TestCommon, TestUserType)
{
    static Vector s_min(0.1f, 1.3f, 2.1f);
    static Vector s_max(3.2f, 2.f, 3.5f);

    static auto equal = [](const Vector& A, const Vector& B)
    {
        return A.X == B.X && A.Y == B.Y && A.Z == B.Z;
    };

    std::vector<unsigned char> storage;
    {
        Box box(s_min, s_max);

        auto ar = oarchive(storage);
        ar & box;
    }
    {
        Box box;

        auto ar = iarchive(storage);
        ar & box;

        EXPECT("Struct", equal(box.Min, s_min) && equal(box.Max, s_max));
    }
}

#include <SF/Support/string.hpp>
#include <SF/Support/any.hpp>

struct Product
{
    std::string name;
    std::size_t series;
    std::size_t price;
};

SERIALIZATION(SaveLoad, Product)
{
    archive & self.name & self.series & self.price;
}

struct Printer : Product
{
#if __cplusplus >= 201703L
    std::any owner;
#endif // if
};

SERIALIZATION(SaveLoad, Printer)
{
    archive & hierarchy<Product>(self);
#if __cplusplus >= 201703L
    archive & self.owner;
#endif // if
}

TEST(TestCommon, TestInheritance)
{
    static std::string s_owner = "Jen";

    static Printer s_p;
    s_p.name = "Canon";
    s_p.series = 37868723;
    s_p.price = 1000;

#if __cplusplus >= 201703L
    s_p.owner = sf::serializable(s_owner);
#endif // if

    std::vector<unsigned char> storage;
    {
        Printer p = s_p;

        auto ar = oarchive(storage);
        ar & p;
    }
    {
        Printer p = s_p;

        auto ar = iarchive(storage);
        ar & p;

    #if __cplusplus >= 201703L
        auto owner = std::any_cast<std::string>(&p.owner);
        EXPECT("inheritance.inited", owner != nullptr && *owner == s_owner);
    #endif // if

        EXPECT("inheritance.value",
            p.name == s_p.name && p.series == s_p.series && p.price == s_p.price);
    }
}
