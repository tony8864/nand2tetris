set -e

GREEN="\033[0;32m"
RESET="\033[0m"

PROJECTS=("compiler" "vm_translator" "assembler")

echo "==> Cleaning all build artifacts..."

for project in "${PROJECTS[@]}"; do
    echo -e "${GREEN}-> Cleaning $project${RESET}"
    make -C $project clean
done

rm -rf out

echo "==> Done!"