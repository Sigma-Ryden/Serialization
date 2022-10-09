# Sifar - Simple serialization library

`Sifar` is a flexible and extensible C++ library with many features, that will allow you to easily
and quickly turn your objects into a sequence of bits.
The library has a small code base, but enough to use it.

Use macros for code generation or read the documentation and write your own code.
There are 2 main serialization classes under your control: `WriteArchive` and `ReadArchive`.
Both classes can be equipped with additional helper classes via template parameters if needed.
A wrapper for streams will help you flexibly configure the process of writing and reading data.
The type registrar will allow you to properly serialize and deserialize polymorphic and abstract types.

Is a header-only library implemented purely in C++11.

# Functional:

#### Serialization of common types:
- all arithmetic types
- enum or scoped enum
- static or dynamic array
- pointer (with data tracking)
#### Sserialization of library types:
- STL: string, vector, array...
- any usage types
#### serialization of users class:
- common
- hierarchy
- polymorphic
- abstract

### Quick start.
Warning! This is a simple example. You can see real applied examples [here](https://github.com/Sigma-Ryden/Sifar/tree/master/test).

Preparing:

```C++
#include <Sifar/Core.hpp>
```
Let's include support of std::string serialization:
```C++
#include <Sifar/Support/string.hpp>

using namespace sifar::library; // support of string
```
And let's equip our simple class with serialization support:
```C++
#include <iostream> // cout

class Shape
{
    SERIALIZABLE()

private:
    std::string name_;
    float x_, y_;

public:
    Shape() : name_("Unknown"), x_(), y_() {}

    Shape(const char* name, float x, float y)
        : name_(name), x_(x), y_(y) {}

    void print() const
    {
        std::cout << name_ << " shape is lacated at: "
                  << x_ << "; " << y_ << std::endl;
    }
};

SERIALIZATION_SAVE_LOAD(Shape)
{
    archive & self.name_
            & self.x_
            & self.y_;
}
```
Explaining of using macros above:
- ```SERIALIZABLE()``` - Provide us with secure saving and loading of objects.
- ```SERIALIZATION_SAVE_LOAD()``` - Generate save/load serialization functions for given class.
This macro allows you to split into two separate macros: ```SERIALIZATION_SAVE()``` and ```SERIALIZATION_LOAD()``` if needed.

### Using of serialization library:

Sinse we are going to use hard drive storage, let's include standard file stream:
```C++
#include <fstream> // ifstream, ofstream
```
```C++
void save(Shape& shape)
{
    std::ofstream file("example.bin", std::ios::binary);
    if (not file.is_open()) return;

    auto ar = sifar::writer(file);

    ar & shape;

    file.close();
}
```
```C++
void load(Shape& shape)
{
    std::ifstream file("example.bin", std::ios::binary);
    if (not file.is_open()) return;

    auto ar = sifar::reader(file);

    ar & shape;

    file.close();
}
```
Processing:
```C++
int main()
{
    // Saving of Shape object
    {
        Shape shape("Square", 0.25f, 2.5f);
        save(shape);
    }
    // Loading of Shape object
    {
        Shape shape;
        shape.print();

        load(shape);

        shape.print();
    }

    return 0;
}
```
### Output:
```console
Unknown shape is lacated at: (0, 0)
Square shape is lacated at: (0.25, 2.5)
```
#### Notes:
In this case the ```load``` and ```save``` functions are just a division of the code into blocks.
It does not affect the library's performance in any way.

For ```WriteArchive``` objects, you may also using overloaded ```operator <<``` instead ```operator &```,
and also ```operator >>``` for ```ReadArchive``` objects.

Examples:
```
// saving objects to the archive
archive << shape << vector << animal << human;
// loading objects from the archive
archive >> shape >> vector >> animal >> human;

//depends on archive type
archive & shape & vector & animal & human;

// or operator()
archive(shape, vector, animal, human);
```

# License:
This library is an open source project licensed under: [MIT](https://opensource.org/licenses/MIT).
