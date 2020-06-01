#include <stdint.h>
#include "../util.h"
#include "../device/serial.h"
#include "../stdcall/pipe_include.h"

void echo(int argc, char *argv[])
{
    for (int i = 0; i < argc; i++) {
        thread *t = get_thread_ptr(get_cur_thread_tid());
        int s = pwait(*t->sid_ptr, EXIST);
        if (!s) {
            puts_serial("echo wait failed\n");
            return;
        }
        pwrite_await(argv[i], strlen(argv[i]));
        s = psignal(*t->sid_ptr, EXIST);
        if (!s) {
            puts_serial("echo signal failed\n");
        }
    }
}
