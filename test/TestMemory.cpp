#include "SirafTestingBase.hpp"

#include <Siraf/Support/unique_ptr.hpp>

struct Parent : POLYMORPHIC()
{
    SERIALIZABLE(Parent)

    virtual ~Parent() = default;
    int p = 0;
};

SERIALIZATION(SaveLoad, Parent)
{
    archive & self.p;
}

struct Child : Parent
{
    SERIALIZABLE(Child)

    int c = 0;
};

SERIALIZATION(SaveLoad, Child)
{
    archive & hierarchy<Parent>(self) & self.c;
}

bool operator== (const Child& lhs, const Child& rhs)
{
    return lhs.p == rhs.p && lhs.c == rhs.c;
}

TEST(TestMemory, TestUniquePtr)
{
    static int sv_i = 8791;

    Child sv_c;
    sv_c.p = 506;
    sv_c.c = 1234;

    std::vector<unsigned char> storage;
    {
        std::unique_ptr<int> u_i(new int(sv_i));
        std::unique_ptr<Parent> u_p(new Child(sv_c));

        auto ar = oarchive(storage);
        ar & u_i & u_p;
    }
    {
        std::unique_ptr<int> u_i;
        std::unique_ptr<Parent> u_p;

        auto ar = iarchive(storage);
        ar & u_i & u_p;

        ASSERT("std::unique_ptr<>", u_i != nullptr);
        EXPECT("std::unique_ptr<>", *u_i == sv_i);
        
        ASSERT("std::unique_ptr<polymorphic>", u_p != nullptr);
       
        auto raw_u_c = dynamic_cast<Child*>(u_p.get());
        
        ASSERT("std::unique_ptr<polymorphic>", raw_u_c != nullptr);
        EXPECT("std::unique_ptr<polymorphic>", *raw_u_c == sv_c);
    }
}

#include <Siraf/Support/shared_ptr.hpp>

struct A : POLYMORPHIC()
{
    SERIALIZABLE(A)

    int a = 0;
};

struct B : virtual A
{
    SERIALIZABLE(B)

    int b = 0;
};

struct C : virtual A
{
    SERIALIZABLE(C)

    int c = 0;
};

struct D : B, C
{
    SERIALIZABLE(D)

    int d = 0;
};

SERIALIZATION(SaveLoad, A)
{
    archive & self.a;
}

SERIALIZATION(SaveLoad, B)
{
    archive & hierarchy<A>(self)
            & self.b;
}

SERIALIZATION(SaveLoad, C)
{
    archive & hierarchy<A>(self)
            & self.c;
}

SERIALIZATION(SaveLoad, D)
{
    archive & hierarchy<A, B, C>(self) // type order does not matter
            & self.d;
}

bool operator== (const D& lhs, const D& rhs)
{
    return lhs.a == rhs.a
        && lhs.b == rhs.b
        && lhs.c == rhs.c
        && lhs.d == rhs.d;
}

TEST(TestMemory, TestSharedPtr)
{
    D sv_d;
    sv_d.a = 1;
    sv_d.b = 2;
    sv_d.c = 3;
    sv_d.d = 4;
    
    std::vector<unsigned char> storage;
    {
    	std::shared_ptr<D> s_d = std::make_shared<D>(sv_d);
    	std::shared_ptr<B> s_b = s_d;
    	std::shared_ptr<C> s_c = s_d;
    	std::shared_ptr<A> s_a = s_d;
    	
        auto ar = oarchive(storage);
    	ar & s_b & s_a & s_c & s_d; // special shuffle
    }
    {
        std::shared_ptr<D> s_d;
        std::shared_ptr<B> s_b;
        std::shared_ptr<C> s_c;
        std::shared_ptr<A> s_a;
    	
        {
            auto ar = iarchive(storage);
            ar & s_c & s_a & s_d & s_b; // one more shuffle
        }

    	using siraf::memory::pure;
    	
        ASSERT("std::shared_ptr<polymorphic>",
            s_a != nullptr && s_b != nullptr && s_c != nullptr && s_d != nullptr);
    	
        EXPECT("std::shared_ptr<polymorphic>",
            s_a.use_count() == 4 && s_b.use_count() == 4 &&
            s_c.use_count() == 4 && s_d.use_count() == 4);

        EXPECT("std::shared_ptr<polymorphic>",
            pure(s_a) == pure(s_b) && pure(s_b) == pure(s_c) && pure(s_c) == pure(s_d));
    	
    	auto raw_s_d = s_d.get();
    	auto raw_s_b = s_b.get();
    	auto raw_s_c = s_c.get();
    	auto raw_s_a = s_a.get();
    	
    	EXPECT("std::shared_ptr<polymorphic>", *raw_s_d == sv_d);
    	EXPECT("std::shared_ptr<polymorphic>", *dynamic_cast<D*>(raw_s_b) == sv_d);
    	EXPECT("std::shared_ptr<polymorphic>", *dynamic_cast<D*>(raw_s_c) == sv_d);
    	EXPECT("std::shared_ptr<polymorphic>", *dynamic_cast<D*>(raw_s_a) == sv_d);
    }
}

#include <Siraf/Support/weak_ptr.hpp>

TEST(TestMemory, TestWeakPtr)
{
    static int sv_i = 983258;

    Child sv_c;
    sv_c.p = 7458;
    sv_c.c = 589;

    std::vector<unsigned char> storage;
    {
        auto s_i = std::make_shared<int>(sv_i);
        auto s_p = std::make_shared<Child>(sv_c);

        std::weak_ptr<int> w_i = s_i;
        std::weak_ptr<Parent> w_p = s_p;

        auto ar = oarchive(storage);
        ar & w_i & w_p;
    }
    {
        std::weak_ptr<int> w_i;
        std::weak_ptr<Parent> w_p;

        auto ar = iarchive(storage);
        ar & w_i & w_p;

        ASSERT("std::weak_ptr<>", !w_i.expired());
        EXPECT("std::weak_ptr<>", *w_i.lock() == sv_i);
        
        ASSERT("std::weak_ptr<polymorphic>", !w_p.expired());

        auto s_c = std::dynamic_pointer_cast<Child>(w_p.lock());

        EXPECT("std::weak_ptr<polymorphic>", *s_c == sv_c);
    }
}


TEST(TestMemory, TestSharedAndWeakPtr)
{
    static float sv_f = 0.5f;

    std::vector<unsigned char> storage;
    {
        std::shared_ptr<float> s_f = std::make_shared<float>(sv_f);
        std::weak_ptr<float> w1_f = s_f;
        std::weak_ptr<float> w2_f = s_f;

        auto ar = oarchive(storage);
        ar & w1_f & s_f & w2_f; // special shuffle
    }
    {
        std::shared_ptr<float> s_f;
        std::weak_ptr<float> w1_f;
        std::weak_ptr<float> w2_f;

        {
            auto ar = iarchive(storage);
            ar & w1_f & w2_f & s_f; // one more shuffle
        }

        ASSERT("std::shared_ptr<>", s_f != nullptr);
        ASSERT("std::weak_ptr<>", !w1_f.expired() && !w2_f.expired());

        EXPECT("std::shared_ptr<>", s_f.use_count() == 1);

        auto s1_f = w1_f.lock();
        auto s2_f = w2_f.lock();

        EXPECT("std::weak_ptr<>", w1_f.use_count() == 3 && w2_f.use_count() == 3);

        EXPECT("std::shared_ptr<>", *s_f == sv_f);
        EXPECT("std::weak_ptr<>", *w1_f.lock() == sv_f && *w2_f.lock() == sv_f);
    }
}