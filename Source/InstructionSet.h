#ifndef INSTRUCTIONSET_H_
#define INSTRUCTIONSET_H_

static const uint8_t nop=0x00;
static const uint8_t vreturn=0xb1;
static const uint8_t ireturn=0xac;

static const uint8_t iconst_0=0x03;
static const uint8_t iconst_1=0x04;
static const uint8_t iload=0x15;	//+index
static const uint8_t iaload=0x2e;
static const uint8_t aload=0x19;
static const uint8_t aload_0=0x2a;
static const uint8_t istore=0x36;
static const uint8_t astore=0x3a;
static const uint8_t iastore=0x4f;
static const uint8_t inew=0xbb;
static const uint8_t dup=0x59;

static const uint8_t sipush=0x11;

static const uint8_t getfield=0xb4;
static const uint8_t putfield=0xb5;
static const uint8_t iadd=0x60;
static const uint8_t isub=0x64;
static const uint8_t imul=0x68;

static const uint8_t getstatic=0xb2;
static const uint8_t invokevirtual=0xb6;
static const uint8_t invokespecial=0xb7;
static const uint8_t newarray=0xbc;	//+atype
static const uint8_t arraylength=0xbe;


static const uint8_t ifeq=0x99;
static const uint8_t ifgt=0x9d;
static const uint8_t ifle=0x9e;
static const uint8_t if_icmpge=0xa2;
static const uint8_t igoto=0xa7;

#endif /* INSTRUCTIONSET_H_ */
