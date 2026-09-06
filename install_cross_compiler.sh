#!/usr/bin/env bash
#
# install_cross_compiler.sh
#
# Installa il cross compiler i686-elf (binutils + gcc) per sviluppo OS bare-metal
# su Debian/Debian-based (Mint incluso).
#
# Uso:
#   chmod +x install_cross_compiler.sh
#   ./install_cross_compiler.sh
#
# Al termine il compilatore sarà in: $HOME/opt/cross/bin/i686-elf-gcc
#

set -euo pipefail

# ----------------------------------------------------------------------------
# Configurazione
# ----------------------------------------------------------------------------
BINUTILS_VERSION="2.42"
GCC_VERSION="13.2.0"

export PREFIX="$HOME/opt/cross"
export TARGET="i686-elf"
export PATH="$PREFIX/bin:$PATH"

SRC_DIR="$HOME/src/cross-build"
JOBS="$(nproc)"

log()  { echo -e "\033[1;32m[+] $*\033[0m"; }
err()  { echo -e "\033[1;31m[!] $*\033[0m" >&2; }

# ----------------------------------------------------------------------------
# 1. Dipendenze di sistema
# ----------------------------------------------------------------------------
log "Installo dipendenze di sistema..."
sudo apt update
sudo apt install -y \
    build-essential \
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    libisl-dev \
    wget \
    curl

# ----------------------------------------------------------------------------
# 2. Preparazione directory
# ----------------------------------------------------------------------------
log "Preparo directory di lavoro in $SRC_DIR"
mkdir -p "$SRC_DIR"
mkdir -p "$PREFIX"
cd "$SRC_DIR"

# ----------------------------------------------------------------------------
# 3. Download sorgenti (se non già presenti)
# ----------------------------------------------------------------------------
if [ ! -f "binutils-${BINUTILS_VERSION}.tar.gz" ]; then
    log "Scarico binutils ${BINUTILS_VERSION}..."
    wget -q --show-progress "https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.gz"
else
    log "binutils ${BINUTILS_VERSION} già scaricato, salto."
fi

if [ ! -f "gcc-${GCC_VERSION}.tar.gz" ]; then
    log "Scarico gcc ${GCC_VERSION}..."
    wget -q --show-progress "https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz"
else
    log "gcc ${GCC_VERSION} già scaricato, salto."
fi

# ----------------------------------------------------------------------------
# 4. Estrazione
# ----------------------------------------------------------------------------
if [ ! -d "binutils-${BINUTILS_VERSION}" ]; then
    log "Estraggo binutils..."
    tar xf "binutils-${BINUTILS_VERSION}.tar.gz"
fi

if [ ! -d "gcc-${GCC_VERSION}" ]; then
    log "Estraggo gcc..."
    tar xf "gcc-${GCC_VERSION}.tar.gz"
fi

# ----------------------------------------------------------------------------
# 5. Build binutils
# ----------------------------------------------------------------------------
log "Compilo binutils (target=$TARGET)..."
rm -rf build-binutils
mkdir build-binutils
cd build-binutils

../binutils-${BINUTILS_VERSION}/configure \
    --target="$TARGET" \
    --prefix="$PREFIX" \
    --with-sysroot \
    --disable-nls \
    --disable-werror

make -j"$JOBS"
make install
cd "$SRC_DIR"

# ----------------------------------------------------------------------------
# 6. Verifica che as sia raggiungibile prima di buildare gcc
# ----------------------------------------------------------------------------
if ! command -v "${TARGET}-as" >/dev/null 2>&1; then
    err "${TARGET}-as non trovato in PATH ($PREFIX/bin). Interrompo."
    exit 1
fi
log "binutils installato correttamente: $(command -v ${TARGET}-as)"

# ----------------------------------------------------------------------------
# 7. Build gcc (solo compilatore C/C++ freestanding, senza libc host)
# ----------------------------------------------------------------------------
log "Compilo gcc (target=$TARGET) — può richiedere parecchi minuti..."
rm -rf build-gcc
mkdir build-gcc
cd build-gcc

../gcc-${GCC_VERSION}/configure \
    --target="$TARGET" \
    --prefix="$PREFIX" \
    --disable-nls \
    --enable-languages=c,c++ \
    --without-headers

make -j"$JOBS" all-gcc
make -j"$JOBS" all-target-libgcc
make install-gcc
make install-target-libgcc
cd "$SRC_DIR"

# ----------------------------------------------------------------------------
# 8. Verifica finale
# ----------------------------------------------------------------------------
log "Verifica installazione..."
"$PREFIX/bin/${TARGET}-gcc" --version | head -n1

# ----------------------------------------------------------------------------
# 9. Aggiunta al PATH permanente (se non già presente)
# ----------------------------------------------------------------------------
BASHRC="$HOME/.bashrc"
PATH_LINE='export PATH="$HOME/opt/cross/bin:$PATH"'

if ! grep -Fq "$PATH_LINE" "$BASHRC" 2>/dev/null; then
    log "Aggiungo il cross compiler al PATH in $BASHRC"
    {
        echo ""
        echo "# Cross compiler i686-elf (OS dev)"
        echo "$PATH_LINE"
    } >> "$BASHRC"
    log "Fatto. Esegui 'source ~/.bashrc' o apri un nuovo terminale."
else
    log "PATH già configurato in $BASHRC, salto."
fi

log "Installazione completata."
echo ""
echo "Compilatore disponibile in: $PREFIX/bin/${TARGET}-gcc"
echo "Assembler disponibile in:   $PREFIX/bin/${TARGET}-as"
echo "Linker disponibile in:      $PREFIX/bin/${TARGET}-ld"

echo "now installing qemu:"

sudo apt update
sudo apt install nasm qemu-system-x86

echo 
echo "you now should be able to run the build script"
