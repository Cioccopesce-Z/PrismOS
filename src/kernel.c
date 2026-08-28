void kernel_main(){
    char *video = (char*) 0xB8000;
    video[0] = 'K';
    video[1] = 0x0F;   // bianco su sfondo nero
}