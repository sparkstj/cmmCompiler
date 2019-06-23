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
main:
move $fp, $sp
li $t0, 0
li $t1, 1
li $t2, 0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t3, $v0
move $t4, $t3
Label1:
blt $t2, $t4, Label2
j Label3
Label2:
add $t5, $t0, $t1
move $a0, $t1
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
move $t0, $t1
move $t1, $t5
addi $t2, $t2, 1
j Label1
Label3:
li $t6, 0
move $v0, $t6
jr $ra
