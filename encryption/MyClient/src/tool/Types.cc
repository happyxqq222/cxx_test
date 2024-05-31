
#include <memory>
#include <fstream>

template<>
struct std::default_delete<std::ifstream>{
    void operator()(ifstream* s) const{
        s->close();
    }
};
