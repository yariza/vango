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
    ./process -i "assets/imagesin/"$name".png" -s "assets/styles/"$name".yaml" -o "assets/canvas/"$name".yaml";

    cd -;

    cd render/build;
    ./render "assets/canvas/"$name".yaml" -s "assets/styles/"$name".yaml" -o "../../results/"$name".png" -d 0;

    cd -;
done
