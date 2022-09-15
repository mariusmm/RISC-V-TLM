# Tests

Using riscv-arch-test (commit #acf857c68f3e5a476da24cff653b51176ac14dbf)

## Setup

+ Copy doc/riscv-arch-test/RISCV-TLM to riscv-arch-test/riscv-target/
+ Run tests

```make verify```

## Results
### RV 64

#### I Extension
+ Check add-01                    ... OK 
+ Check addi-01                   ... OK 
+ Check addiw-01                  ... OK 
+ Check addw-01                   ... OK 
+ Check and-01                    ... OK 
+ Check andi-01                   ... OK 
+ Check auipc-01                  ... OK 
+ Check beq-01                    ... OK 
+ Check bge-01                    ... OK 
+ Check bgeu-01                   ... OK 
+ Check blt-01                    ... OK 
+ Check bltu-01                   ... OK 
+ Check bne-01                    ... OK 
+ Check fence-01                  ... OK 
+ Check jal-01                    ... OK 
+ Check jalr-01                   ... OK 
+ Check lb-align-01               ... OK 
+ Check lbu-align-01              ... OK 
+ Check ld-align-01               ... OK 
+ Check lh-align-01               ... OK 
+ Check lhu-align-01              ... OK 
+ Check lui-01                    ... OK 
+ Check lw-align-01               ... OK 
+ Check lwu-align-01              ... OK 
+ Check or-01                     ... OK 
+ Check ori-01                    ... OK 
+ Check sb-align-01               ... OK 
+ Check sd-align-01               ... OK 
+ Check sh-align-01               ... OK 
+ Check sll-01                    ... OK 
+ Check slli-01                   ... OK 
+ Check slliw-01                  ... OK 
+ Check sllw-01                   ... OK 
+ Check slt-01                    ... OK 
+ Check slti-01                   ... OK 
+ Check sltiu-01                  ... OK 
+ Check sltu-01                   ... OK 
+ Check sra-01                    ... OK 
+ Check srai-01                   ... OK 
+ Check sraiw-01                  ... OK 
+ Check sraw-01                   ... OK 
+ Check srl-01                    ... OK 
+ Check srli-01                   ... OK 
+ Check srliw-01                  ... OK 
+ Check srlw-01                   ... OK 
+ Check sub-01                    ... OK 
+ Check subw-01                   ... OK 
+ Check sw-align-01               ... OK 
+ Check xor-01                    ... OK 
+ Check xori-01                   ... OK 

#### C Extension
+ Check cadd-01                   ... OK 
+ Check caddi-01                  ... OK 
+ Check caddi16sp-01              ... OK 
+ Check caddi4spn-01              ... OK 
+ Check caddiw-01                 ... OK 
+ Check caddw-01                  ... OK 
+ Check cand-01                   ... OK 
+ Check candi-01                  ... OK 
+ Check cbeqz-01                  ... OK 
+ Check cbnez-01                  ... OK 
+ Check cebreak-01                ... FAIL 
+ Check cj-01                     ... OK 
+ Check cjalr-01                  ... OK 
+ Check cjr-01                    ... OK 
+ Check cld-01                    ... OK 
+ Check cldsp-01                  ... OK 
+ Check cli-01                    ... OK 
+ Check clui-01                   ... OK 
+ Check clw-01                    ... OK 
+ Check clwsp-01                  ... OK 
+ Check cmv-01                    ... OK 
+ Check cnop-01                   ... OK 
+ Check cor-01                    ... OK 
+ Check csd-01                    ... OK 
+ Check csdsp-01                  ... OK 
+ Check cslli-01                  ... OK 
+ Check csrai-01                  ... OK 
+ Check csrli-01                  ... OK 
+ Check csub-01                   ... OK 
+ Check csubw-01                  ... OK 
+ Check csw-01                    ... OK 
+ Check cswsp-01                  ... OK 
+ Check cxor-01                   ... OK 

#### M Extension
+ Check div-01                    ... OK 
+ Check divu-01                   ... OK 
+ Check divuw-01                  ... OK 
+ Check divw-01                   ... OK 
+ Check mul-01                    ... OK 
+ Check mulh-01                   ... OK 
+ Check mulhsu-01                 ... OK 
+ Check mulhu-01                  ... OK 
+ Check mulw-01                   ... OK 
+ Check rem-01                    ... OK 
+ Check remu-01                   ... OK 
+ Check remuw-01                  ... OK 
+ Check remw-01                   ... OK 

#### Privilege
+ Check ebreak                    ... FAIL 
+ Check ecall                     ... FAIL 
+ Check misalign1-jalr-01         ... OK 
+ Check misalign2-jalr-01         ... FAIL 
+ Check misalign-beq-01           ... OK 
+ Check misalign-bge-01           ... OK 
+ Check misalign-bgeu-01          ... OK 
+ Check misalign-blt-01           ... OK 
+ Check misalign-bltu-01          ... OK 
+ Check misalign-bne-01           ... OK 
+ Check misalign-jal-01           ... OK 
+ Check misalign-ld-01            ... FAIL 
+ Check misalign-lh-01            ... FAIL 
+ Check misalign-lhu-01           ... FAIL 
+ Check misalign-lw-01            ... FAIL 
+ Check misalign-lwu-01           ... FAIL 
+ Check misalign-sd-01            ... FAIL 
+ Check misalign-sh-01            ... FAIL 
+ Check misalign-sw-01            ... FAIL 

#### Zfencei
+ Check Fencei                    ... OK 


### RV32

#### I Extension
+ Check add-01                    ... OK 
+ Check addi-01                   ... OK 
+ Check and-01                    ... OK 
+ Check andi-01                   ... OK 
+ Check auipc-01                  ... OK 
+ Check beq-01                    ... OK 
+ Check bge-01                    ... OK 
+ Check bgeu-01                   ... OK 
+ Check blt-01                    ... OK 
+ Check bltu-01                   ... OK 
+ Check bne-01                    ... OK 
+ Check fence-01                  ... OK 
+ Check jal-01                    ... OK 
+ Check jalr-01                   ... OK 
+ Check lb-align-01               ... OK 
+ Check lbu-align-01              ... OK 
+ Check lh-align-01               ... OK 
+ Check lhu-align-01              ... OK 
+ Check lui-01                    ... OK 
+ Check lw-align-01               ... OK 
+ Check or-01                     ... OK 
+ Check ori-01                    ... OK 
+ Check sb-align-01               ... OK 
+ Check sh-align-01               ... OK 
+ Check sll-01                    ... OK 
+ Check slli-01                   ... OK 
+ Check slt-01                    ... OK 
+ Check slti-01                   ... OK 
+ Check sltiu-01                  ... OK 
+ Check sltu-01                   ... OK 
+ Check sra-01                    ... OK 
+ Check srai-01                   ... OK 
+ Check srl-01                    ... OK 
+ Check srli-01                   ... OK 
+ Check sub-01                    ... OK 
+ Check sw-align-01               ... OK 
+ Check xor-01                    ... OK 
+ Check xori-01                   ... OK 


#### C Extension
+ Check cadd-01                   ... OK 
+ Check caddi-01                  ... OK 
+ Check caddi16sp-01              ... OK 
+ Check caddi4spn-01              ... OK 
+ Check cand-01                   ... OK 
+ Check candi-01                  ... OK 
+ Check cbeqz-01                  ... OK 
+ Check cbnez-01                  ... OK 
+ Check cebreak-01                ... FAIL 
+ Check cj-01                     ... OK 
+ Check cjal-01                   ... OK 
+ Check cjalr-01                  ... OK 
+ Check cjr-01                    ... OK 
+ Check cli-01                    ... OK 
+ Check clui-01                   ... OK 
+ Check clw-01                    ... OK 
+ Check clwsp-01                  ... OK 
+ Check cmv-01                    ... OK 
+ Check cnop-01                   ... OK 
+ Check cor-01                    ... OK 
+ Check cslli-01                  ... OK 
+ Check csrai-01                  ... OK 
+ Check csrli-01                  ... OK 
+ Check csub-01                   ... OK 
+ Check csw-01                    ... OK 
+ Check cswsp-01                  ... OK 
+ Check cxor-01                   ... OK 

#### M Extension
+ Check div-01                    ... OK 
+ Check divu-01                   ... OK 
+ Check mul-01                    ... OK 
+ Check mulh-01                   ... OK 
+ Check mulhsu-01                 ... OK 
+ Check mulhu-01                  ... OK 
+ Check rem-01                    ... OK 
+ Check remu-01                   ... OK 

#### Privilage
+ Check ebreak                    ... FAIL 
+ Check ecall                     ... FAIL 
+ Check misalign1-jalr-01         ... OK 
+ Check misalign2-jalr-01         ... FAIL 
+ Check misalign-beq-01           ... OK 
+ Check misalign-bge-01           ... OK 
+ Check misalign-bgeu-01          ... OK 
+ Check misalign-blt-01           ... OK 
+ Check misalign-bltu-01          ... OK 
+ Check misalign-bne-01           ... OK 
+ Check misalign-jal-01           ... OK 
+ Check misalign-lh-01            ... FAIL 
+ Check misalign-lhu-01           ... FAIL 
+ Check misalign-lw-01            ... FAIL 
+ Check misalign-sh-01            ... FAIL 
+ Check misalign-sw-01            ... FAIL 

### Zifencei Extension
+ Check Fencei                    ... OK 

