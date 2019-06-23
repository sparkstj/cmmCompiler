.data
_prompt: .asciiz "Enter an Integer"
_ret: .asciiz "\n"
.globl main
.text
read:
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
jr $ra
write:
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
move $v0, $0
jr $ra
fact:
addi $sp, $sp, -40
sw $ra, 36($sp)
sw $s8, 32($sp)
addi $s8, $sp, 40
sw $s0, -12($s8)
sw $s1, -16($s8)
sw $s2, -20($s8)
sw $s3, -24($s8)
sw $s4, -28($s8)
sw $s5, -32($s8)
sw $s6, -36($s8)
sw $s7, -40($s8)
lw $t0, 44($sp)
add $t1, $t0, $a3
add $t2, $t1, $a2
add $t3, $t2, $a0
lw $ra, -4($s8)
addi $sp, $s8, 0
lw $s8, -8($sp)
move $v0, $t3
jr $ra
main:
move $s8, $sp
li $t4, 0
li $t5, 1
li $t6, 2
li $t7, 3
li $s0, 4
addi $sp, $sp, -4
sw $a0, 0($sp)
lw $t8, -4($s8)
move $a0, $s0
move $a1, $t7
move $a2, $t6
move $a3, $t5
addi $sp, $sp, -4
sw $t0, 0($sp)
addi $sp, $sp, -4
sw $t1, 0($sp)
addi $sp, $sp, -4
sw $t2, 0($sp)
addi $sp, $sp, -4
sw $t3, 0($sp)
addi $sp, $sp, -4
sw $t4, 0($sp)
sw $t0, 0($sp)
addi $sp, $sp, -4
sw $ra, 0($sp)
jal fact
lw $ra, 0($sp)
addi $sp, $sp, 4
lw $a0, -4($s8)
move $t0, $v0
move $a0, $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $t1, 0
move $v0, $t1
jr $ra
