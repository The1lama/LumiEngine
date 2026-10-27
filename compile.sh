#!/bin/bash

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # Reset Color

if command -v xmake >/dev/null 2>&1; 
then 
  echo"" 
else 
  "${RED}Error: xmake not installed${NC}"
  read -rsp $'Press enter to continue...\n'
fi

xmake || { echo "${RED}Failed to compile program${NC}"; read -rsp $'Press enter to continue...\n'; }

echo "${GREEN}Compile succsessful program${NC}"

xmake run || { echo "${RED}Failed to Run program${NC}"; read -rsp $'Press enter to continue...\n'; }
