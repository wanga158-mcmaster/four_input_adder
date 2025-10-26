#include "bits/stdc++.h"

using namespace std;

ofstream _OUT;

string get_binary(int x) {
    return bitset<6>(x).to_string();
}

void gen_lut(int set_bits) {
    int case_num = (1 << 6); // max number of cases possible
    string s;
    int set_cnt;
    _OUT << "case(d_in) // check sum = "<< set_bits << "\n";
    for (int i = 0; i < case_num; ++i) {
        s = get_binary(i);
        set_cnt = 0;
        for (const char & c : s) {
            set_cnt += (c == '1');
        }
        _OUT << "6'b"<< s << ": d_out = "<< (set_cnt == set_bits) << ";\n";
    }
    _OUT << "default: d_out = 0;\n";
    _OUT << "endcase\n";
}

int main()
{
    // open output file
    _OUT = ofstream("OUT.txt");

    // generate cases
    for (int set_bits = 0; set_bits <= 6; ++set_bits) {
        _OUT << "module check_"<< set_bits << "(\n";
        _OUT << "input logic [5:0] d_in,\n";
        _OUT << "output logic d_out\n";
        _OUT << ");\n\n";
        _OUT << "always_comb begin\n";
        gen_lut(set_bits);
        _OUT << "end\n";
        _OUT << "endmodule;\n\n\n";
    }

    _OUT.close();
    return 0;
}