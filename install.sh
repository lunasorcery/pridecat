#!/bin/bash

# Define the colors of the pride flag
RED='\033[38;2;228;3;3m'
ORANGE='\033[38;2;255;140;0m'
YELLOW='\033[38;2;255;237;0m'
GREEN='\033[38;2;0;128;38m'
BLUE='\033[38;2;0;77;255m'
PURPLE='\033[38;2;117;7;135m'
NC='\033[0m' # No Color

# Print the Pridecat banner with pride flag colors
echo -e "${RED}mmmmm  mmmmm  mmmmm  mmmm   mmmmmm   mmm    mm  mmmmmmm${NC}"
echo -e "${ORANGE}#   \"# #   \"#   #    #   \"m #      m\"   \"   ##     #   ${NC}"
echo -e "${YELLOW}#mmm#\" #mmmm\"   #    #    # #mmmmm #       #  #    #   ${NC}"
echo -e "${GREEN}#      #   \"m   #    #    # #      #       #mm#    #    ${NC}"
echo -e "${BLUE}#      #    \" mm#mm  #mmm\"  #mmmmm  \"mmm\" #    #   #   ${NC}"
echo -e "${PURPLE}Welcome to the Pridecat installation script!${NC}"

printf "\n"

echo -e "${GREEN}Do you want a simple or advanced setup? (s/a; default: s)${NC}"
read setup_type

if [ "$setup_type" = "a" ]; then
    echo -e "${RED}Advanced setup selected. ${NC}"
    echo -e "${GREEN}Fetching forks...${NC}"
    repos=$(curl -s https://api.github.com/repos/lunasorcery/pridecat/forks | jq -r '.[].full_name')
    repos=("lunasorcery/pridecat" $repos)

    echo -e "${GREEN}Please select a repo (1 is the official repo):${NC}"
    select repo in "${repos[@]}"; do
        if [[ -n $repo ]]; then
            echo -e "${GREEN}Using $repo${NC}"
            break
        else
            echo -e "${RED}Invalid option${NC}"
            exit 1
        fi
    done

    echo -e "${GREEN}Fetching branches...${NC}"
    branches=$(curl -s https://api.github.com/repos/$repo/branches | jq -r '.[].name')
    if [ ${#branches[@]} -eq 1 ]; then
        branch=${branches[0]}
        echo -e "${GREEN}Only one branch found: $branch. Using it.${NC}"
    else
        echo -e "${GREEN}Please select a branch (1 is the main branch):${NC}"
        select branch in "${branches[@]}"; do
            if [[ -n $branch ]]; then
                echo -e "${GREEN}Using $branch${NC}"
                break
            else
                echo -e "${RED}Invalid option${NC}"
                exit 1
            fi
        done
    fi

    echo -e "${GREEN}Setting up the advanced setup...${NC}"
    mkdir .temp-pridecat
    cd .temp-pridecat
    echo -e "${GREEN}Downloading the files...${NC}"
    wget https://raw.githubusercontent.com/$repo/$branch/Makefile > /dev/null 2>&1
    wget https://raw.githubusercontent.com/$repo/$branch/main.cpp > /dev/null 2>&1
    echo -e "${GREEN}Building and installing...${NC}"
    make && sudo make install
    echo -e "${GREEN}Cleaning up...${NC}"
    cd ..
    rm -rf .temp-pridecat
    echo "Done!" | pridecat

else
    echo -e "${GREEN}Setting up the simple setup...${NC}"
    mkdir .temp-pridecat
    cd .temp-pridecat
    echo -e "${GREEN}Downloading the files...${NC}"
    wget https://raw.githubusercontent.com/lunasorcery/pridecat/main/Makefile > /dev/null 2>&1
    wget https://raw.githubusercontent.com/lunasorcery/pridecat/main/main.cpp > /dev/null 2>&1
    echo -e "${GREEN}Building and installing...${NC}"
    make && sudo make install
    echo -e "${GREEN}Cleaning up...${NC}"
    cd ..
    rm -rf .temp-pridecat
    echo "Done!" | pridecat
fi