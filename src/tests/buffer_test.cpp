#include <buffer.h>

int main()
{
    buffer_t buffer;
    buffer.resize(10);
    buffer.resize(5);
    buffer.resize(6);
    buffer.resize(5);
    buffer.resize(5);
    char * c, * d = (char*)1;
    c = nullptr;
    buffer.emplace_pointer(c);
    buffer.get_pointer(d);
    if (d != nullptr)
    {
        return 1;
    }
    return 0;
}
