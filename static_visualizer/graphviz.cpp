#include <string>
#include <graphviz/gvc.h>

using namespace std;

inline char *
string_to_char_ptr (const string &str)
{
    return const_cast<char * const> (str.c_str ());
}

int
_agsafeset (
    void * obj, const string &name,
    const string &value, const string &def
)
{
    return agsafeset (
        obj, string_to_char_ptr (name),
        string_to_char_ptr (value), string_to_char_ptr (def)
    );
}


