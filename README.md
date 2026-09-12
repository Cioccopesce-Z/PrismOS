# Prism Kernel & Prism Bootloader 
kernel, bootloader and os (i hope) in base x86 32bit, i'll probably upgrade at some point to 64, built from scratch for learning purpose idk everything but i'm trying to collect as many knowlege as possible either from youtube and osdev.org

some view of the kernel splash screen
<img width="2880" height="1920" alt="Screenshot From 2026-09-12 12-10-44" src="https://github.com/user-attachments/assets/dacce29d-2f6d-46bb-9f53-b78c70dd99a7" />


under u can see the fegh manager being used to organize a segment of memory till idx 0x0000001e (30byte) i'm working on a stack manager and other application that should cover all the basics needs of an application, i will probably use fegh since it cant ecxeed for construcion the limit of its declaration, idk, also i will ad both the zinter and zcomply language but only for fun.
i also have written the printf function or the fprint in the stdf library and i need to add the string library and a file manager
<img width="2880" height="1920" alt="Screenshot From 2026-09-12 12-07-37" src="https://github.com/user-attachments/assets/d83aed4d-1414-4789-adb1-59da1649cd86" />

