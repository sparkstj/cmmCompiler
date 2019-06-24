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
move $s8, $sp
li $t0, 0
li $t1, 1
Label1:
li $t2, 1000
ble $t1, $t2, Label2
j Label3
Label2:
mul $t3, $t1, $t1
add $t4, $t3, $t1
add $t0, $t0, $t4
addi $t1, $t1, 1
j Label1
Label3:
move $a0, $t0
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $t5, 0
move $v0, $t5
jr $ra
