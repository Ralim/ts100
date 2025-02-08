#!/usr/bin/env bash
set -e
TRANSLATION_DIR="../Translations"
#TRANSLATION_SCRIPT="make_translation.py"

# AVAILABLE_LANGUAGES will be calculating according to json files in $TRANSLATION_DIR
AVAILABLE_LANGUAGES=()
BUILD_LANGUAGES=()
AVAILABLE_MODELS=("TS100" "TS80" "TS80P" "Pinecil" "MHP30" "Pinecilv2" "S60" "S60P" "T55" "S99" "TS101")
BUILD_MODELS=()

builder_info() {
    echo -e "
********************************************
               IronOS Firmware
        builder for Miniware + Pine64

                                    by Ralim
********************************************"
}

# Calculate available languages
for f in "$TRANSLATION_DIR"/translation_*.json; do
    AVAILABLE_LANGUAGES+=("$(echo "$f" | tr "[:lower:]" "[:upper:]" | sed "s/[^_]*_//" | sed "s/\.JSON//g")")
done

usage() {
    builder_info
    echo -e "
Usage : 
    $(basename "$0") [-l <LANG_CODES>] [-m <MODELS>] [-h]

Parameters :
    -l LANG_CODE : Force a specific language (${AVAILABLE_LANGUAGES[*]})
    -m MODEL     : Force a specific model (${AVAILABLE_MODELS[*]})
    -h           : Show this help message

Example : 
    $(basename "$0") -l EN -m TS100                     (Build one language and model)
    $(basename "$0") -l EN -m \"TS100 MHP30\"             (Build one language and multi models)
    $(basename "$0") -l \"DE EN\" -m \"TS100 MHP30\"        (Build multi languages and models)

INFO : 
    By default, without parameters, the build is for all platforms and all languages

" 1>&2
    exit 1
}

StartBuild() {
    read -n 1 -r -s -p $'Press Enter to start the building process...\n'
}

checkLastCommand() {
    if [ $? -eq 0 ]; then
        echo "    [Success]"
        echo "********************************************"
    else
        forceExit "checkLastCommand"
    fi
}

forceExit() {
    if [ -n "$*" ]; then
        echo -e "\n\n    [Error]: $*"
    else
        echo "    [Error]"
    fi
    echo "********************************************"
    echo " -- Stop on error --"
    exit 1
}

isInArray() {
    local value="$1"   # Save first argument in a variable
    shift              # Shift all arguments to the left (original $1 gets lost)
    local array=("$@") # Rebuild the array with rest of arguments

    for item in "${array[@]}"; do
        [[ $value == "$item" ]] && return 0
    done
    return 1
}

declare -a margs=()
declare -a largs=()

while getopts "h:l:m:" option; do
    case "${option}" in
    h)
        usage
        ;;
    l)
        IFS=' ' read -r -a largs <<<"${OPTARG}"
        ;;
    m)
        IFS=' ' read -r -a margs <<<"${OPTARG}"
        ;;
    *)
        usage
        ;;
    esac
done
shift $((OPTIND - 1))
builder_info

# Checking requested language
echo -n "Available languages :"
echo " ${AVAILABLE_LANGUAGES[*]}"
echo -n "Requested languages : "
if ((${#largs[@]})); then
    for i in "${largs[@]}"; do
        i=$(echo "${i}" | tr '[:lower:]' '[:upper:]')
        if isInArray "$i" "${AVAILABLE_LANGUAGES[@]}"; then
            echo -n "$i "
            BUILD_LANGUAGES+=("$i")
        else
            forceExit "Language '$i' is unknown. Check and use only from the available languages."
        fi
    done
    echo ""
fi
if [ -z "${BUILD_LANGUAGES[*]}" ]; then
    echo "    No custom languages selected."
    echo "    Building: [ALL LANGUAGES]"
    BUILD_LANGUAGES+=("${AVAILABLE_LANGUAGES[@]}")
fi
echo "********************************************"

# Checking requested model
echo -n "Available models :"
echo " ${AVAILABLE_MODELS[*]}"
echo -n "Requested models : "
if ((${#margs[@]})); then
    for i in "${margs[@]}"; do

        if [[ "$i" != "Pinecil" ]] && [[ "$i" != "Pinecilv2" ]]; then # Dirty. Need to adapt the Build process to use upper cases only
            i=$(echo "${i}" | tr '[:lower:]' '[:upper:]')
        fi

        if isInArray "$i" "${AVAILABLE_MODELS[@]}"; then
            echo -n "$i "
            BUILD_MODELS+=("$i")
        else
            forceExit "Model '$i' is unknown. Check and use only from the available models."
        fi
    done
    echo ""
fi

if [ -z "${BUILD_MODELS[*]}" ]; then
    echo "    No custom models selected."
    echo "    Building: [ALL MODELS]"
    BUILD_MODELS+=("${AVAILABLE_MODELS[@]}")
fi

echo "********************************************"

##
#StartBuild

if [ ${#BUILD_LANGUAGES[@]} -gt 0 ] && [ ${#BUILD_MODELS[@]} -gt 0 ]; then
    echo "Cleaning previous builds"
    rm -rf Hexfile/ >/dev/null
    rm -rf Objects/ >/dev/null
    make clean >/dev/null
    checkLastCommand

    for model in "${BUILD_MODELS[@]}"; do
        echo "Building firmware for $model in ${BUILD_LANGUAGES[*]}"
        make -j"$(nproc)" model="$model" "${BUILD_LANGUAGES[@]/#/firmware-}" >/dev/null
        checkLastCommand
    done
else
    forceExit "Nothing to build. (no model or language specified)"
fi
echo " -- Firmwares successfully generated --"
echo "End..."
