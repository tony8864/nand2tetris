set -e # Exit on any error

# ==== Check argument ====
if [ -z "$1" ]; then
    echo "Usage: $0 <input-folder-containing-jack-code>"
    exit 1
fi

USER_INPUT_DIR="$1"

# Project folders
ASSEMBLER_DIR="assembler"
TRANSLATOR_DIR="vm_translator"
COMPILER_DIR="compiler"

# Temp and output folders
TEMP1_DIR="tmp1"
TEMP2_DIR="tmp2"

GREEN="\033[0;32m"
YELLOW="\033[0;33m"
RESET="\033[0m"

# ==== PREP ====
mkdir -p "$TEMP1_DIR" "$TEMP2_DIR"

echo -e "${GREEN}===> Running Compiler (.jack -> .vm)${RESET}"
make -C "$COMPILER_DIR"
"$COMPILER_DIR/build/compiler" "$USER_INPUT_DIR"
mv out "$TEMP1_DIR/in"

echo -e "${GREEN}===> Running VM translator (.vm -> .asm)${RESET}"
make -C "$TRANSLATOR_DIR"
"$TRANSLATOR_DIR/build/vmtranslator" "$TEMP1_DIR/in"
mv out "$TEMP2_DIR/in"

echo -e "${GREEN}===> Running Assembler (.asm -> .hack)${RESET}"
make -C "$ASSEMBLER_DIR"
"$ASSEMBLER_DIR/build/assembler" "$TEMP2_DIR/in/exe.asm"

echo -e "${YELLOW}===> Cleaning up temporary folders${RESET}"
rm -rf "$TEMP1_DIR" "$TEMP2_DIR"
echo -e "${GREEN}===> Done! Final output is in out/ ${RESET}"