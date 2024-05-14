/*!
 \file Trace.cpp
 \brief Basic TLM-2 Trace module
 \author Màrius Montón
 \date September 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstdio>
#include <iostream>
#include <termios.h>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/wait.h>

// Code partially taken from
// https://github.com/embecosm/esp1-systemc-tlm/blob/master/sysc-models/simple-soc/TermSC.h

#include "Trace.hpp"

namespace riscv_tlm::peripherals {

    void Trace::xtermLaunch(char *slaveName) const {
        char *arg;
        char *fin = &(slaveName[strlen(slaveName) - 2]);

        if (nullptr == strchr(fin, '/')) {
            arg = new char[2 + 1 + 1 + 20 + 1];
            sprintf(arg, "-S%c%c%d", fin[0], fin[1], ptMaster);
        } else {
            char *slaveBase = ::basename(slaveName);
            arg = new char[2 + strlen(slaveBase) + 1 + 20 + 1];
            sprintf(arg, "-S%s/%d", slaveBase, ptMaster);
        }

        char *argv[3];
        argv[0] = (char *) ("xterm");
        argv[1] = arg;
        argv[2] = nullptr;

        execvp("xterm", argv);
    }

    void Trace::xtermKill() {

        if (-1 != ptSlave) {        // Close down the slave
            close(ptSlave);            // Close the FD
            ptSlave = -1;
        }

        if (-1 != ptMaster) {        // Close down the master
            close(ptMaster);
            ptMaster = -1;
        }

        if (xtermPid > 0) {            // Kill the terminal
            kill(xtermPid, SIGKILL);
            waitpid(xtermPid, nullptr, 0);
        }
    }

    void Trace::xtermSetup() {
        ptMaster = open("/dev/ptmx", O_RDWR);

        if (ptMaster != -1) {
            grantpt(ptMaster);

            unlockpt(ptMaster);

            char *ptSlaveName = ptsname(ptMaster);
            ptSlave = open(ptSlaveName, O_RDWR);    // In and out are the same

            struct termios termInfo{};
            tcgetattr(ptSlave, &termInfo);

            termInfo.c_lflag &= ~ECHO;
            termInfo.c_lflag &= ~ICANON;
            tcsetattr(ptSlave, TCSADRAIN, &termInfo);

            xtermPid = fork();

            if (xtermPid == 0) {
                xtermLaunch(ptSlaveName);
            }
        }
    }

    Trace::Trace(sc_core::sc_module_name const &name) :
            sc_module(name), socket("socket") {

        socket.register_b_transport(this, &Trace::b_transport);

        xtermSetup();
    }

    Trace::~Trace() {
        xtermKill();
    }

    void Trace::b_transport(tlm::tlm_generic_payload &trans,
                            sc_core::sc_time &delay) {

        unsigned char *ptr = trans.get_data_ptr();
        delay = sc_core::SC_ZERO_TIME;

        ssize_t a = write(ptSlave, ptr, 1);
        (void) a;

        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }
}