#include <task.h>

#include <bwio.h>

#define SL 6
#define FP 7
#define IP 8
#define LR 9

#define USER_MODE_FLAG 0x10

void task_create(Task *t, void (*code)()) {
   t->tid = 1; 
   t->sp = (int *) (t->stack + STACK_SIZE);
   t->sp -= 10; // Make room for 14 registers

   asm(
        "mrs %[status_register], cpsr"
        : [status_register] "=r" (t->spsr)
        :
   );
   t->spsr = ((t->spsr >> 5) << 5) | USER_MODE_FLAG;

   int i;
   for (i = 0; i < 6; ++i) {
       t->sp[i] = 0;
   }

   t->sp[SL] = (unsigned int) (t->stack + STACK_SIZE);
   t->sp[IP] = 0;
   t->sp[FP] = t->sp[SL];
   t->sp[LR] = (unsigned int) code; // TODO change this to an exit function

   t->pc = code;
}

void task_set_returnvalue (Task *t, unsigned int value) {
	t->return_value = value;
}

int *task_get_sp(Task *t) {
	return t->sp;
}

unsigned int task_get_spsr(Task *t) {
	return t->spsr;
}

void * task_get_pc(Task *t) {
	return t->pc;
}

void task_save_pc(Task *t, void *pc) {
	t->pc = pc;
	bwprintf(COM2, "Saved pc: %x\n", t->pc);
}

void task_save_sp(Task *t, int *sp) {
	t->sp = sp;
	bwprintf(COM2, "Saved sp: %x\n", t->sp);
}

void task_save_spsr(Task *t, unsigned int spsr) {
	t->spsr = spsr;
	bwprintf(COM2, "Saved spsr: %x\n", t->spsr);
}

void task_print(Task *t) {
    bwprintf(COM2, "TD address:%x Task id:%d, sp: %x, spsr: %x\n\r",
            t, t->tid, t->sp, t->spsr);
}
