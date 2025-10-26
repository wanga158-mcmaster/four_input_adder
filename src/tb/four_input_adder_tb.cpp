#include "bits/stdc++.h"
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vfour_input_adder.h"

#define llong long long

using std::cout;

#define MAX_SIM_TIME 333

vluint64_t sim_time = 0;
Vfour_input_adder * dut;
VerilatedVcdC * m_t;

std::random_device rd;
std::mt19937 gen;
std::uniform_int_distribution<uint32_t> dist;

#define chk(a, b) chk_f(a, b, #b)

void chk_f(uint32_t expct, uint32_t exprm, std::string s) {
    cout << "@t=" << sim_time << " for " << s << ", EXPECTED: " << expct << ", EXPERIMENTAL: " << exprm;
    if (expct != exprm) {
        cout << ", ERR!";
    }
    cout << "\n";
}

struct transactional_item_in {
    uint32_t a0, a1, a2, a3, en;
};

struct transactional_item_out {
    uint32_t b, O_F, vld;
};

struct scoreboard {
    std::queue<transactional_item_in *> tx_in_dQ;

    void write_input(transactional_item_in * tx) {
        tx_in_dQ.push(tx);
    }

    void write_output(transactional_item_out * tx_out) {
        transactional_item_in * tx_in = tx_in_dQ.front();
        tx_in_dQ.pop();

        llong su = (llong)tx_in->a0 + (llong)tx_in->a1 + (llong)tx_in->a2 + (llong)tx_in->a3;
        
        llong b = tx_out->b;
        llong O_F = tx_out->O_F;
        
        chk(su & UINT32_MAX, b);
        std::string c = std::bitset<34>(su).to_string();
        chk(c[0] == '1' || c[1] == '1', O_F);
        
        delete tx_in;
        delete tx_out;
    }
};

struct driver {
    Vfour_input_adder * dut;

    driver(Vfour_input_adder * dut) : dut(dut) {}; // constructor
    
    void drive(transactional_item_in * tx) {
        dut->en = !(!(tx)); // set enable to = tx is not nullptr
        if (tx) {
            dut->a0 = tx->a0;
            dut->a1 = tx->a1;
            dut->a2 = tx->a2;
            dut->a3 = tx->a3;
        }
        delete tx;
    }
};

struct monitor_in {
    Vfour_input_adder * dut;
    scoreboard * scb;

    monitor_in(Vfour_input_adder * dut, scoreboard * scb) : dut(dut), scb(scb) { };

    void monitor() {
        if (dut->en) {
            transactional_item_in * tx = new transactional_item_in();
            tx->a0 = dut->a0;
            tx->a1 = dut->a1;
            tx->a2 = dut->a2;
            tx->a3 = dut->a3;

            scb->write_input(tx);
        }
    }
};

struct monitor_out {
    Vfour_input_adder * dut;
    scoreboard * scb;
    
    monitor_out(Vfour_input_adder * dut, scoreboard * scb) : dut(dut), scb(scb) { };

    void monitor() {
        if (dut->vld) {
            transactional_item_out * tx = new transactional_item_out();
            tx->b = dut->b;
            tx->O_F = dut->O_F;

            scb->write_output(tx);
        }
    }

};

transactional_item_in * transactional_input_in_defined() {
    static int cnt = 0;
    ++cnt;
    transactional_item_in * tx = new transactional_item_in();
    switch(cnt) {
        case 1:
            tx->a0 = 0;
            tx->a1 = 0;
            tx->a2 = 0;
            tx->a3 = 0;
        break;
        case 2:
            tx->a0 = 1;
            tx->a1 = 1;
            tx->a2 = 1;
            tx->a3 = 1;
        break;
        case 3:
            tx->a0 = 0;
            tx->a1 = 0;
            tx->a2 = 1;
            tx->a3 = 1;
        break;
        case 4:
            tx->a0 = 1;
            tx->a1 = 1;
            tx->a2 = 0;
            tx->a3 = 0;
        break;
        case 5:
            tx->a0 = UINT32_MAX;
            tx->a1 = UINT32_MAX;
            tx->a2 = UINT32_MAX;
            tx->a3 = UINT32_MAX;
        break;
    }
    return tx;
};

transactional_item_in * transactional_input_in_random() {
    static int cnt = 0;
    ++cnt;
    if (cnt & 1) { // drive an input every other time the transactional
        transactional_item_in * tx = new transactional_item_in{
            dist(gen), // set all inputs to random
            dist(gen),
            dist(gen),
            dist(gen)
        };
    return tx;
    } else {
        return nullptr;
    }
}

int main()
{
    // setup random number generator
    std::random_device rd;
    gen = std::mt19937(rd());
    dist = std::uniform_int_distribution<uint32_t> (0, UINT32_MAX);

    // Verilated::commandArgs(argc, argv);
    dut = new Vfour_input_adder;
    Verilated::traceEverOn(true);
    m_t = new VerilatedVcdC;
    dut->trace(m_t, 33);
    m_t->open("wf.vcd");

    transactional_item_in  * tx;
    driver * drv = new driver(dut);
    scoreboard * scb = new scoreboard();
    monitor_in * mn_in = new monitor_in(dut, scb);
    monitor_out * mn_out = new monitor_out(dut, scb);

    dut->clk = 1;
    dut->rst_n = 1;
    int pcc = 0; // posedge clock count

    while (sim_time < MAX_SIM_TIME) {
        dut->clk ^= 1;
        dut->eval();
        if (dut->clk) {
            ++pcc;
            if (pcc > 3) {
                if (pcc > 3 && pcc < 9) {
                    tx = transactional_input_in_defined();
                } else {
                    tx = transactional_input_in_random();
                }
                drv->drive(tx);
                mn_in->monitor();
                mn_out->monitor();
            }
        }
        m_t->dump(sim_time);
        sim_time++;
    }

    m_t->close();
    delete dut;
    delete drv;
    delete scb;
    delete mn_in;
    delete mn_out;

    exit(EXIT_SUCCESS);
}

/*

verilator --trace -cc --top four_input_adder -I../rtl four_input_adder.sv --exe four_input_adder_tb.cpp

make -C obj_dir -f Vfour_input_adder.mk Vfour_input_adder

./obj_dir/Vfour_input_adder

*/