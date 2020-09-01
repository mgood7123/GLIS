if [[ $1 == "--help" || $1 == "-h" || $# == 0 ]]
    then
        echo "usage $0 <entity> # will use ${entity}.vhdl as the source name"
        echo "usage $0 <source_file> <entity>"
        exit 0
fi
if [[ $# == 1 ]]
    then
        source="${1}.vhdl"
        entity="$1"
        shift 1
elif [[ $# > 1 ]]
    then
        source="$1"
        entity="$2"
        shift 2
fi

ghdl -a "test.vhdl" &&
ghdl -e "$entity" &&
ghdl -r "$entity" $@
rm -rf *.cf
