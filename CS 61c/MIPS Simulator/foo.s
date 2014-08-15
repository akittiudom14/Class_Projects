.data
passstring:     .asciiz "all tests passed!\n"

.text

# _start is the program entry point.
.global _start
.ent    _start 
_start:

#
#  End of header
#

# Shifting operators
la $t0, 10
sll $t0, $t0, 2 # Expect $t0 to be 2, $8 = 0x00000028 (CHECK)
addiu $t1, $0, 15
srl $t1, $t1, 1 # Expect $t1 to be 7, $9 = 0x00000007 (CHECK)
addiu $t1, $0, -18
srl $t1, $t1, 1 # Expect $t1 not to be -9, $9 != 0xFFFFFFF7 (CHECK)
addiu $t2, $0, 15
sra $t2, $t2, 1 # Expect $t2 to be 7, $10 = 0x00000007 (CHECK)
addiu $t2, $0, -18
sra $t2, $t2, 1 # Expect $t2 to be -9, $10 = 0xFFFFFFF7 (CHECK)

# Jumping operators
la $t3,label1
jalr $t3

# Unsigned commands
addiu $t0, $0, 5
addiu $t1, $0, 4

addu $t2, $t0, $t1 # Expect $10 = 9 (CHECK)
subu $t2, $t0, $t1 # Expect $10 = 1 (CHECK)
and $t2, $t0, $t1 # Expect 00..0100 (CHECK)
or $t2, $t0, $t1 # Expect 00..0101 (CHECK)
xor $t2, $t0, $t1 # Expect 00..0001 (CHECK)
nor $t2, $t0, $t1 # Expect 11..1010 (CHECK)
addiu $t0, $0, -5
slt $t3, $t0, $t1 # $t3 should be 1 (CHECK)
sltu $t3, $t0, $t1 # $t3 should be 0 (CHECK)
jal label2

# I-type operators
addiu $t0, $0, -3 # $t0 is -3 (CHECK)
slti $t1, $t0, 5 # Expect $t1 to be 1 (-3 < 5) (CHECK)
sltiu $t1, $t0, 5 # Expect $t1 to be 0 (0xFFFFFFFD > 5) (CHECK)
j moreitype

# Multiplication tests
label1:
addiu $s0,$0, -32
addiu $s1,$0, 43
mult $s0,$s1

mfhi $s2 # Expect 0xFFFFFFFF (CHECK)
mflo $s3 # Expect 0xFFFFFAA0 (-1376) (CHECK)

multu $s0,$s1

mfhi $s2 # Expect 0x0000002A (CHECK)
mflo $s3 # Expect 0xFFFFFAA0 (CHECK)

jr $ra

# Test more I-type operators
moreitype:
addiu $s0, $0, -1
andi $t0, $s0, 0x00000F0F # Expect 0x00000F0F (CHECK)
ori $t0, $s0, 0 # Expect 0xFFFFFFFF (CHECK)
xori $t0, $s0, 0x0000F0F0 # Expect 0xFFFF0F0F (CHECK)
lui $t1, 0x0000F0F0 # Expect $9 = 0xF0F00000 (CHECK)
j loadtests

# Test beq
label2:
addiu $s0, $0, 3
addiu $s1, $0, 3
beq $s0, $s1, label3
j fail

# Loads
loadtests:
la $t0, basic
lw $t1, 0($t0) # $t1 should be 0xFFFFFC70 (CHECK)
lh $t2, 0($t0) # $t2 should be 0xFFFFFC70 (CHECK)
lb $t3, 0($t0) # $t3 should be 0x00000070 (CHECK)

lw $t4, 4($t0) # $t4 should be 0x01234567 (CHECK)
lh $t5, 2($t0) # $t5 should be 0xFFFFFFFF (CHECK)
lb $t6, 1($t0) # $t6 should be 0xFFFFFFFC (CHECK)

lhu $t5, 2($t0) # $t5 should be 0x0000FFFF (CHECK)
lbu $t6, 1($t0) # $t6 should be 0x000000FC (CHECK)

# Stores
lw $t7, 4($t0)
sb $t7, 0($t0)
lw $s0, 0($t0) # $s0 should be 0xFFFFFC67 (CHECK)
sh $t7, 0($t0)
lw $s0, 0($t0) # $s0 should be 0xFFFF4567 (CHECK)
sw $t7, 0($t0)
lw $s0, 0($t0) # $s0 should be 0x01234567 (CHECK)
sb $t7, 1($t0)
lw $s0, 0($t0) # $s0 should be 0x01236767 (CHECK)
sh $t7, 2($t0)
lw $s0, 0($t0) # $s0 should be 0x45676767 (CHECK)
j pass

# Test bne
label3:
bne $s0, $s1, fail
jr $ra

pass:
        la $a0, passstring
        li $v0, 4
        syscall
        b done

fail:
	ori $a0, %lo(str)
	ori $v0, $0, 4
	syscall

# exit the simulation (v0 = 10, syscall)
done:
	ori   $v0, $zero, 10
	syscall

.end _start

.data
basic: .word 0xFFFFFC70 # -912
anotherword: .word 0x01234567
str: .asciiz "Hello, world!\n"
