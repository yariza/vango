mkdir process/build;
cd process/build; cmake .. && make ;

cd -;

mkdir render/build;
cd render/build; cmake .. && make ;

cd -;

for name in "$@"; do
    echo;
    echo $name;

    cd process/build;
    ./process -i "../../assets/autoimagesin/"$name".png" -s "../../assets/autostyles/"$name".yaml" -o "../../assets/autocanvas/"$name".yaml";

    cd -;

    cd render/build;
    ./render "../../assets/autocanvas/"$name".yaml" -s "../../assets/autostyles/"$name".yaml" -o "../../autoresults/"$name".png" -d 0;

    ./render "../../assets/autocanvas/"$name".yaml" -s "../../assets/autostyles/"$name".yaml" -o "../../autoresults/"$name"p.png" -d 0 -p;


    cd -;
done
