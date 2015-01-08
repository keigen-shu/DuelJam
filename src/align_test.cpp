#include <cassert>
#include <cstdio>
#include "align.h"

using namespace clan;

// Alignment enumerator behaviour unit test.
void test() {
    HAlign hs[3] = { HAlign::LEFT, HAlign::RIGHT, HAlign::CENTER };
    VAlign vs[3] = { VAlign::TOP, VAlign::BOTTOM, VAlign::MIDDLE };

    for(auto h : hs) {
    for(auto v : vs) {
        Alignment a = make_alignment(h, v);
        Alignment b = make_alignment(v, h);
        assert(a == b);

        assert(get_halign(a) == h);
        assert(get_valign(a) == v);

        uint8_t x, y, z; x = y = z = -1;
        x = *h;
        y = *v;
        z = *a;
        fprintf(stdout, "%d, %d, %d \n", x, y, z);
    }
    }
}

int main() {
    test();
}
