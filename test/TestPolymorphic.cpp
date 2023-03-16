#include "SirafTestingBase.hpp"

#include <Siraf/Support/string.hpp>

template <class SomeType>
struct Base : POLYMORPHIC()
{
    SERIALIZABLE(Base)

public:
    SomeType data;

public:
    Base() = default;
    Base(const SomeType& data) : data(data) {}
};

template <typename> struct is_base : std::false_type {};
template <typename T> struct is_base<Base<T>> : std::true_type {};

SERIALIZATION(Save, Base<std::string>)
{
    // more specialize version
    archive & self.data;
}

CONDITIONAL_SERIALIZATION(SaveLoad, is_base<T>::value)
{
    archive & self.data;
}

namespace internal // example namespace
{

struct Derived : Base<std::string>
{
    SERIALIZABLE(internal::Derived)

public:
    float value;

public:
    Derived() = default;
    Derived(const std::string& data, float c) : Base(data), value(c) {}
};

} // namespace internal

SERIALIZATION(SaveLoad, internal::Derived)
{
    archive & hierarchy<Base<std::string>>(self);
    archive & self.value;
}

// we can overload polymorphic key for each class (or full template specialization)

// smae as EXPORT_POLYMORPHIC_KEY("Base<double>", Base<double>)
EXPORT_POLYMORPHIC(Base<std::string>)

// EXPORT_POLYMORPHIC(internal::Derived) // - not required, but allow
// EXPORT_POLYMORPHIC_KEY("Other Derived", "internal::Derived) // - possible use

TEST(TestCommon, TestPolymorphic)
{
    using siraf::core::Access; // need for trait validation

    using Parent = Base<std::string>;
    using Child  = internal::Derived;

    static std::string sv_p_d = "Fuzz";

    static std::string sv_c_d = "Buzz";
    static float sv_c_v = 1291.238f;

    std::vector<unsigned char> storage;
    {
        Parent* p = new Parent(sv_p_d);
        Parent* c = new Child(sv_c_d, sv_c_v);

        auto ar = oarchive(storage);
        ar & p & c;
    }
    {
        Parent* p = nullptr;
        Parent* c = nullptr;

        auto ar = iarchive(storage);
        ar & p & c;

        ASSERT("polymorphic.inited", p != nullptr);
        ASSERT("polymorphic.trait", Access::trait<Parent>() == Access::trait(*p));

        EXPECT("polymorphic.value", p->data == sv_p_d);

        auto d = dynamic_cast<Child*>(c);

        ASSERT("polymorphic.derived inited", d != nullptr);
        ASSERT("polymorphic.derived trait", Access::trait<Child>() == Access::trait(*c));

        EXPECT("polymorphic.derived value", d->data == sv_c_d && d->value == sv_c_v);
    }
}
